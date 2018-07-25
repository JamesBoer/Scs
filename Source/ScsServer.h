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
			int32_t clientID;
			bool connected;
			SendQueue sendQueue;
			ReceiveQueue receiveQueue;
		};

		using ClientConnectionPtr = std::shared_ptr<ClientConnection>;

	public:
		Server(std::string_view port, ServerNotifyPtr notify);
		virtual ~Server();

		virtual void RunListener();
		void RunConnection(ClientConnectionPtr connection);

		void DisconnectClient(int32_t clientId) override;
		void Send(const void * data, size_t bytes, int32_t clientId) override;
		void SendAll(const void * data, size_t bytes) override;

	private:
		using ClientConnectionList = std::list<ClientConnectionPtr, Allocator<ClientConnectionPtr>>;
		enum class Status
		{
			Initial,
			Listening,
			Shutdown
		};

		ClientConnectionList m_connectionList;
		std::mutex m_connectionListMutex;
		SocketPtr m_listenerSocket;
		std::thread m_thread;
		std::condition_variable m_stateCondition;
		ServerNotifyPtr m_notifier;
		std::mutex m_notifierMutex;
		String m_port;
		int32_t m_maxClientId;
		Status m_status;
	};

}; // namespace Scs


#endif // SCS_SERVER_H____