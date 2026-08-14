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

#pragma once
#ifndef SCS_SERVER_H____
#define SCS_SERVER_H____

namespace Scs
{

	class Server : public IServer
	{
	private:
		struct ClientConnection
		{
			ClientConnection(const Server & svr) :
				server(svr),
				clientID(-1),
				connected(false)
				{}
			const Server & server;
			std::thread thread;
			SocketPtr socket;
			ClientID clientID;
			bool connected;
			SendQueue sendQueue;
			ReceiveQueue receiveQueue;
		};

		using ClientConnectionPtr = std::shared_ptr<ClientConnection>;

	public:
		Server(const ServerParams & params);
		virtual ~Server() override;

		void StartListening() override;
		bool IsListening() const override { return m_status == Status::Listening ? true : false; }
		bool HasError() const override { return m_error; }

		void OnStartListening(ServerOnStartListeningFn onStartListening) override { assert(m_status == Status::Initial);  m_onStartListening = onStartListening; }
		void OnConnect(ServerOnConnectFn onConnect) override { assert(m_status == Status::Initial);  m_onConnect = onConnect; }
		void OnDisconnect(ServerOnDisconnectFn onDisconnect) override { assert(m_status == Status::Initial);  m_onDisconnect = onDisconnect; }
		void OnReceiveData(ServerOnReceiveDataFn onReceiveData) override { assert(m_status == Status::Initial);  m_onReceiveData = onReceiveData; }
		void OnUpdate(ServerOnUpdateFn onUpdate) override { assert(m_status == Status::Initial);  m_onUpdate = onUpdate; }

		void DisconnectClient(ClientID clientId) override;
		void Send(ClientID clientId, const void * data, size_t bytes) override;
		void SendAll(const void * data, size_t bytes) override;

	private:
		void RunListener();
		void RunConnection(ClientConnectionPtr connection);

		using ClientConnectionList = std::list<ClientConnectionPtr, Allocator<ClientConnectionPtr>>;
		enum class Status
		{
			Initial,
			Listening,
		};

		ClientConnectionList m_connectionList;
		std::mutex m_connectionListMutex;
		SocketPtr m_listenerSocket;
		std::thread m_thread;
		std::condition_variable m_stateCondition;
		ServerOnStartListeningFn m_onStartListening;
		ServerOnConnectFn m_onConnect;
		ServerOnDisconnectFn m_onDisconnect;
		ServerOnReceiveDataFn m_onReceiveData;
		ServerOnUpdateFn m_onUpdate;
		std::mutex m_notifierMutex;
		String m_port;
		uint32_t m_maxConnections;
		long long m_timeoutMs;
		uint32_t m_updateMs;
		std::chrono::time_point<std::chrono::steady_clock> m_lastUpdate;
		ClientID m_maxClientId = 0;
		std::atomic<Status> m_status = Status::Initial;
        std::atomic_bool m_shutDown = false;
		std::atomic_bool m_error = false;
	};

} // namespace Scs


#endif // SCS_SERVER_H____
