/*
The MIT License (MIT)

Copyright (c) 2018 James Boer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "ScsInternal.h"

using namespace Scs;


Server::Server(const ServerParams & params) :
	m_port(params.port),
	m_maxClientId(0),
	m_status(Status::Initial),
	m_error(false)
{
}

Server::~Server()
{
	LogWriteLine("Shutting down server.");
	m_status = Status::Shutdown;
	if (m_thread.joinable())
		m_thread.join();
}

void Server::DisconnectClient(ClientID clientId)
{
	std::lock_guard<std::mutex> lock(m_connectionListMutex);
	if (m_connectionList.empty())
		return;
	for (auto & connection : m_connectionList)
	{
		if (connection->clientID == clientId)
		{
			connection->connected = false;
			return;
		}
	}
}

void Server::RunListener()
{
	LogWriteLine("Server::RunListener()");
			
	m_stateCondition.notify_one();
	
	// Notify that we've started listening
	if (m_onStartListening)
	{
		std::lock_guard<std::mutex> lock(m_notifierMutex);
		m_onStartListening();
	}

	// Loop until we get a shutdown request
	while (m_status != Status::Shutdown)
	{
		// We don't want to burn too much CPU while idling.
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (m_status == Status::Initial)
		{
			// Create address structure, create socket, and set
			// socket to non-blocking mode
			LogWriteLine("Creating address...");
			AddressPtr address = CreateAddress(m_port, true);
			m_listenerSocket = CreateSocket(address);
			m_listenerSocket->SetNonBlocking(true);

			// Bind the socket to the specified address
			if (m_listenerSocket->Bind(address->GetCurrent()) == false)
			{
				LogWriteLine("Error binding sotck to specified address.");
				m_status = Status::Shutdown;
				m_error = true;
			}
			else
			{
				LogWriteLine("Listener socket created and bound to address.");
			}

			// Listen to this socket
			if (m_listenerSocket->Listen() == false)
			{
				LogWriteLine("Error listening to specified socket.");
				m_status = Status::Shutdown;
				m_error = true;
			}
			else
			{
				LogWriteLine("Server listening for client connection.");
				m_status = Status::Listening;
			}
		}
		else if (m_status == Status::Listening)
		{
			// Check to see if we've established a connection
			if (m_listenerSocket->IsReadable())
			{
				// Only accept a maxinum number of simultaneous connections
				if (m_connectionList.size() > MAX_CONNECTIONS)
					break;

				LogWriteLine("Server received connection request from client.");
				SocketPtr connectionSocket = m_listenerSocket->Accept();
				if (connectionSocket && connectionSocket->IsInvalid() == false)
				{
					// Create a connection data structure that contains everything
					// required to maintain a unique connection state to a client.
					auto connection = std::allocate_shared<ClientConnection>(Allocator<ClientConnection>(), *this);
					connection->clientID = ++m_maxClientId;
					connection->connected = true;
					connection->socket = connectionSocket;

					LogWriteLine("Server accepted connection request from client id %d.", connection->clientID);

					// Now lock and create a new thread for this connection
					if (m_onConnect)
					{
						std::lock_guard<std::mutex> lock(m_notifierMutex);
						m_onConnect(connection->clientID);
					}
					std::thread t([this, connection]() { this->RunConnection(connection); });
					connection->thread.swap(t);			
					std::lock_guard<std::mutex> lock(m_connectionListMutex);
					m_connectionList.push_back(connection);		
				}
			}
		}
	}

	// Wait for all connection threads to finish
	std::lock_guard<std::mutex> lock(m_connectionListMutex);
	for (auto & connection : m_connectionList)
		connection->thread.join();
	
	m_connectionList.clear();
	LogWriteLine("Closing server connection thread.");
}

void Server::RunConnection(ClientConnectionPtr connection)
{
	// Set timeout point
	auto timeoutTime = std::chrono::system_clock::now() + std::chrono::seconds(TIMEOUT_SECONDS);

	// Create a receive buffer
	BufferPtr receiveBuffer = CreateBuffer();
	receiveBuffer->reserve(RECEIVE_BUFFER_SIZE);

	// Each client connection runs in a separate thread, controlled
	// by this function.
	while (connection->connected && m_status != Status::Shutdown)
	{
		// Don't burn too much CPU while idling
		std::this_thread::sleep_for(std::chrono::microseconds(1));

		// Check for connection timeout.  We don't want to keep this connection thread
		// alive if we're not actively sending or receiving data to the client.
		if (std::chrono::system_clock::now() >= timeoutTime)
		{
			LogWriteLine("Client %d timed out. Closing connection.", connection->clientID);
			connection->connected = false;
			break;
		}

		if (m_onUpdate)
		{
			std::lock_guard<std::mutex> lock(m_notifierMutex);
			m_onUpdate();
		}

		// Check first to see if we can write to the socket
		if (connection->socket->IsWritable())
		{
			if (!connection->sendQueue.Empty())
			{
				if (!connection->sendQueue.Send(connection->socket))
				{
					LogWriteLine("Error sending data to client.  Shutting down connection.");
					connection->connected = false;
					break;
				}

				// We need to throttle our connection transmission rate
				std::this_thread::sleep_for(std::chrono::milliseconds(SEND_THROTTLE_MS));

				// Reset timeout
				timeoutTime = std::chrono::system_clock::now() + std::chrono::seconds(TIMEOUT_SECONDS);
			}
		}

		// Check for incoming data
		if (connection->socket->IsReadable())
		{
			// Read data from socket
			receiveBuffer->resize(receiveBuffer->capacity());
			size_t bytesReceived = connection->socket->Receive(receiveBuffer->data(), receiveBuffer->size(), 0);
			if (bytesReceived)
			{
				// Push data into the receive queue
				receiveBuffer->resize(bytesReceived);
				connection->receiveQueue.Push(receiveBuffer->data(), receiveBuffer->size());
				receiveBuffer->clear();

				// Process data in receive queue
				BufferPtr receivedData = connection->receiveQueue.Pop();
				while (receivedData)
				{
					if (m_onReceiveData)
					{
						std::lock_guard<std::mutex> lock(m_notifierMutex);
						m_onReceiveData(connection->clientID, receivedData->data(), receivedData->size());
					}
					receivedData = connection->receiveQueue.Pop();
				}		

				// Reset timeout
				timeoutTime = std::chrono::system_clock::now() + std::chrono::seconds(TIMEOUT_SECONDS);		
			}
		}
	}

	// We're shutting down, so make sure all sockets are disconnected
	// and the connection data structure is removed from the connection
	// list and deleted.
	if (m_onDisconnect)
	{
		std::lock_guard<std::mutex> lock(m_notifierMutex);
		m_onDisconnect(connection->clientID);
	}
	connection->socket = nullptr;
	LogWriteLine("Closing client %d connection thread.", connection->clientID);
}

void Server::Send(ClientID clientId, const void * data, size_t bytes)
{
	std::lock_guard<std::mutex> lock(m_connectionListMutex);
	if (m_connectionList.empty())
		return;
	for (auto itr = m_connectionList.begin(); itr != m_connectionList.end(); ++itr)
	{
		if ((*itr)->clientID == clientId)
		{
			(*itr)->sendQueue.Push(data, bytes);
		}
	}
}

void Server::SendAll(const void * data, size_t bytes)
{
	std::lock_guard<std::mutex> lock(m_connectionListMutex);
	if (m_connectionList.empty())
		return;
	BufferPtr buffer = CreateBuffer();
	buffer->insert(buffer->end(), static_cast<const uint8_t *>(data), static_cast<const uint8_t *>(data) + bytes);
	for (auto itr = m_connectionList.begin(); itr != m_connectionList.end(); ++itr)
	{
		(*itr)->sendQueue.Push(data, bytes);
	}
}

void Server::StartListening()
{
	std::thread t([this]() { this->RunListener(); });
	m_thread.swap(t);

	// Lock until the async initialize function is finished
	std::unique_lock<std::mutex> lock(m_connectionListMutex);
	m_stateCondition.wait_for(lock, std::chrono::milliseconds(1), [this]() { return m_status != Status::Listening;  });
}

ServerPtr Scs::CreateServer(const ServerParams & params)
{
	return std::allocate_shared<Server>(Allocator<Server>(), params);
}
