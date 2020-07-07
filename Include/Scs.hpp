

// begin --- Scs.cpp --- 

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

// begin --- ScsInternal.h --- 

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
#ifndef SCS_INTERNAL_H____
#define SCS_INTERNAL_H____

// begin --- Scs.h --- 

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
#ifndef SCS_H____
#define SCS_H____

#include <memory>
#include <string>
#include <functional>

// Platform definitions
#if defined(_WIN32) || defined(_WIN64)
#define SCS_WINDOWS
#endif

#if defined(__linux__) || defined(__linux)
#define SCS_LINUX
#endif

#ifdef __APPLE__
#ifdef __MACH__
#define SCS_MACOS
#endif
#endif

namespace Scs
{
	// Client
	class IClient;
	using ClientPtr = std::shared_ptr<IClient>;

	/// Prototype for client server connection notification
	using ClientOnConnectFn = std::function<void(IClient &)>;

	/// Prototype for client disconnection notification
	using ClientOnDisconnectFn = std::function<void(IClient &)>;

	/// Prototype for receive data notification
	using ClientOnReceiveDataFn = std::function<void(IClient &, const void *, size_t)>;

	/// Prototype for client update notification
	using ClientOnUpdateFn = std::function<void(IClient &)>;

	/// Parameters for client creation
	/**
	A struct containing parameters related to client creation
	\sa CreateClient()
	*/
	struct ClientParams
	{
		std::string_view port;
		std::string_view address;
		double timeoutSeconds = 5.0;
	};

	class IClient
	{
	public:
		virtual ~IClient() {}

		virtual void Connect() = 0;
		virtual bool IsConnected() const = 0;
		virtual bool HasError() const = 0;

		virtual void OnConnect(ClientOnConnectFn onConnect) = 0;
		virtual void OnDisconnect(ClientOnDisconnectFn onDisconnect) = 0;
		virtual void OnReceiveData(ClientOnReceiveDataFn onReceiveData) = 0;
		virtual void OnUpdate(ClientOnUpdateFn onUpdate) = 0;

		virtual void Send(const void * data, size_t bytes) = 0;
	};

	ClientPtr CreateClient(const ClientParams & params);

	// Server
	class IServer;
	using ServerPtr = std::shared_ptr<IServer>;
	using ClientID = int32_t;


	/// Prototype for server start listening notification
	using ServerOnStartListeningFn = std::function<void(IServer &)>;

	/// Prototype for server client connection notification
	using ServerOnConnectFn = std::function<void(IServer &, ClientID)>;

	/// Prototype for server client disconnection notification
	using ServerOnDisconnectFn = std::function<void(IServer &, ClientID)>;

	/// Prototype for receive data notification
	using ServerOnReceiveDataFn = std::function<void(IServer &, ClientID, const void *, size_t)>;

	/// Prototype for server update notification
	using ServerOnUpdateFn = std::function<void(IServer &)>;

	/// Parameters for server creation
	/**
	A struct containing parameters related to server creation
	\sa CreateServer()
	*/
	struct ServerParams
	{
		std::string_view port;
		uint32_t maxConnections = 100;
		double timeoutSeconds = 15.0;
	};

	class IServer
	{
	public:
		virtual ~IServer() {}

		virtual void StartListening() = 0;
		virtual bool IsListening() const = 0;
		virtual bool HasError() const = 0;

		virtual void OnStartListening(ServerOnStartListeningFn onStartListening) = 0;
		virtual void OnConnect(ServerOnConnectFn onConnect) = 0;
		virtual void OnDisconnect(ServerOnDisconnectFn onDisconnect) = 0;
		virtual void OnReceiveData(ServerOnReceiveDataFn onReceiveData) = 0;
		virtual void OnUpdate(ServerOnUpdateFn onUpdate) = 0;

		virtual void DisconnectClient(ClientID clientId) = 0;
		virtual void Send(ClientID clientId, const void * data, size_t bytes) = 0;
		virtual void SendAll(const void * data, size_t bytes) = 0;
	};

	ServerPtr CreateServer(const ServerParams & params);

	/// Prototype for global memory allocation function callback
	using AllocFn = std::function<void *(size_t)>;

	/// Prototype for global memory re-allocation function callback
	using ReallocFn = std::function<void *(void *, size_t)>;

	/// Prototype for global memory free function callback
	using FreeFn = std::function<void(void *)>;

	/// Prototype for global logging function callback
	using LogFn = std::function<void(const char *)>;


	/// Initializes global SCS parameters
	/**
	A struct containing a number of global parameters affecting memory and logging
	\sa Initialize()
	*/
	struct InitParams
	{
		/// Logging function
		LogFn logFn;
		/// Alloc memory function
		AllocFn allocFn;
		/// Realloc memory function
		ReallocFn reallocFn;
		/// Free memory function
		FreeFn freeFn;
	};

	/// Initializes the Simple Client Server library
	/**
	Initializes the client server library.
	/return true on success, false on failure
	\sa ShutDown(), InitParams
	*/
	bool Initialize(const InitParams & params);
	void ShutDown();

} // namespace Scs


#endif // SCS_H____


// end --- Scs.h --- 



#ifdef SCS_WINDOWS

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
// Exclude rarely-used stuff from Windows headers Windows Header Files:
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

// Link to winsock2 libraries
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SocketLastError       WSAGetLastError()
#define SCS_EWOULDBLOCK       WSAEWOULDBLOCK
#define SCS_EINPROGRESS       WSAEINPROGRESS
#define ScsInetNtoP           InetNtopA
#define ScsIoCtrl             ioctlsocket

// ssize_t is a POSIX type, not a general C++ type
typedef __int64          ssize_t;

#undef min
#undef max

#endif // SCS_WINDOWS

#if defined(SCS_LINUX) || defined(SCS_MACOS)
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR           (-1)
#define SD_RECEIVE             0x00
#define SD_SEND                0x01
#define SD_BOTH                0x02
#define TCP_NODELAY            0x0001
#define SocketLastError		   errno
#define SCS_EWOULDBLOCK        EWOULDBLOCK
#define SCS_EINPROGRESS        EINPROGRESS
#define ScsInetNtoP            inet_ntop
#define ScsIoCtrl              ioctl

#endif

#include <thread>
#include <vector>
#include <list>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdarg>

// begin --- ScsCommon.h --- 

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
#ifndef SCS_COMMON_H____
#define SCS_COMMON_H____

namespace Scs
{
	template<typename T>
	constexpr int unused(const T &) { return 0; }

	template<typename T, size_t s>
	constexpr size_t countof(T(&)[s]) { return s; }

	constexpr uint32_t MakeFourCC(char ch0, char ch1, char ch2, char ch3)
	{
		return ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8 |
			((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24)));
	}

	const uint32_t MAGIC_HEADER_VAL = MakeFourCC('s', 'c', 's', 'm');
	struct MessageHeader
	{
		uint32_t magic = MAGIC_HEADER_VAL;
		uint32_t size = 0;
	};

	void InitializeInternal(const InitParams & params);

	void LogWriteLine(const char * format, ...);

	void * Alloc(size_t bytes);
	void * Realloc(void * ptr, size_t bytes);
	void Free(void * ptr);

	// SCS allocator for use in STL containers
	template <typename T>
	class Allocator
	{
	public:
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;

		Allocator() throw() {};
		Allocator(const Allocator &) throw() { }

		template<typename U>
		Allocator(const Allocator<U>&) throw() { }

		template<typename U>
		Allocator & operator = ([[maybe_unused]] const Allocator<U> & other) { return *this; }
		Allocator & operator = ([[maybe_unused]] const Allocator & other) { return *this; }
		~Allocator() {}

		template <typename U>
		struct rebind { typedef Allocator<U> other; };

		pointer address(reference value) const { return &value; }
		const_pointer address(const_reference value) const { return &value; }

		pointer allocate(size_type n) { return static_cast<pointer> (Scs::Alloc(n * sizeof(value_type))); }
		pointer allocate(size_type n, const void *) { return static_cast<pointer> (Scs::Alloc(n * sizeof(value_type))); }
		void deallocate(void* ptr, size_type) { Scs::Free(static_cast<T*> (ptr)); }

		template<typename U, typename... Args>
		void construct(U* ptr, Args&&  ... args) { ::new ((void*)(ptr)) U(std::forward<Args>(args)...); }
		void construct(pointer ptr, const T& val) { new (static_cast<T*> (ptr)) T(val); }

		template<typename U>
		void destroy(U* ptr) { Scs::unused(ptr); ptr->~U(); }
		void destroy(pointer ptr) { Scs::unused(ptr); ptr->~T(); }

		size_type max_size() const { return std::numeric_limits<std::size_t>::max() / sizeof(T); }
	};

	template <typename T>
	bool operator == (const Allocator<T> &, const Allocator<T> &) { return true; }
	template <typename T>
	bool operator != (const Allocator<T> &, const Allocator<T> &) { return false; }

	using String = std::basic_string <char, std::char_traits<char>, Allocator<char>>;

	using Buffer = std::vector<uint8_t, Allocator<uint8_t>>;
	using BufferPtr = std::shared_ptr<Buffer>;

	BufferPtr CreateBuffer();

}

#endif // SCS_COMMON_H____


// end --- ScsCommon.h --- 



// begin --- ScsAddress.h --- 

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
#ifndef SCS_ADDRESS_H____
#define SCS_ADDRESS_H____

namespace Scs
{

	class Address
	{
	public:
		Address(const String & port, const String & address);
		Address(const String & port, bool passive);
		~Address();

        void Log() const;

		addrinfo * GetHead() const { return m_address; }
		addrinfo * GetCurrent() const { return m_current; }
		bool IsValid() { return m_current ? true : false; }
		bool Next();
		void Reset() { m_current = m_address; }
	private:
		addrinfo * m_address;
		addrinfo * m_current;
	};

	using AddressPtr = std::shared_ptr<Address>;

	AddressPtr CreateAddress(const String & port, const String & address);
	AddressPtr CreateAddress(const String & port, bool passive);

} // namespace Scs

#endif // SCS_ADDRESS_H____


// end --- ScsAddress.h --- 



// begin --- ScsSocket.h --- 

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
#ifndef SCS_SOCKET_H____
#define SCS_SOCKET_H____

namespace Scs
{
	class Socket;

	typedef std::shared_ptr<Socket> SocketPtr;

	class Socket
	{
	public:
		Socket(AddressPtr address);
		Socket(AddressPtr address, SOCKET sckt);
		~Socket();

		// Accept a socket connection from the client, creating a new socket
		SocketPtr Accept();

		// Bind a socket (server connection listener)
		bool Bind(addrinfo * addr);

		// Connect client socket to server
		bool Connect();

		// Check to see if the socket has any error conditions
		bool IsInvalid() const;

		// Is socket readable? (indicates a connection request on server listener sockets)
		bool IsReadable() const;

		// Is socket writable? (indicates a connection on client sockets)
		bool IsWritable() const;

		// Begin listening for client connections on a socket in listener mode
		bool Listen();

		// Receive data
		size_t Receive(void * data, size_t bytes, uint32_t flags);

		// Send data
		bool Send(void * data, size_t bytes, uint32_t flags, size_t * bytesSent);

		// Set non-blocking mode on or off
		void SetNonBlocking(bool nonBlocking);

		// Set the Nagle algorithm
		void SetNagle(bool nagle);

	private:
		SOCKET m_socket;
		AddressPtr m_address;
		//addrinfo * m_currAddress;
	};

	SocketPtr CreateSocket(AddressPtr address);
	SocketPtr CreateSocket(AddressPtr address, SOCKET sckt);

} // namespace Scs

#endif // SCS_SOCKET_H____


// end --- ScsSocket.h --- 



// begin --- ScsSendQueue.h --- 

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
#ifndef SCS_SEND_QUEUE_H____
#define SCS_SEND_QUEUE_H____

namespace Scs
{
	// Message send queue
	class SendQueue
	{
	public:
		bool Empty() const;
		bool Send(SocketPtr socket);
		void Push(const void * data, size_t bytes);

	private:

		std::deque<BufferPtr, Allocator<BufferPtr>> m_queue;
		mutable std::mutex m_mutex;
		size_t m_bytesSent = 0;
	};

} // namespace Scs

#endif // SCS_SEND_QUEUE_H____


// end --- ScsSendQueue.h --- 



// begin --- ScsReceiveQueue.h --- 

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
#ifndef SCS_RECEIVE_QUEUE_H____
#define SCS_RECEIVE_QUEUE_H____

namespace Scs
{

	// Message receive queue
	class ReceiveQueue
	{
	public:
		BufferPtr Pop();
		void Push(const void * data, size_t bytes);

	private:

		std::queue<BufferPtr, std::deque<BufferPtr, Allocator<BufferPtr>>> m_queue;
		std::mutex m_mutex;
		size_t m_messageSize = 0;
		BufferPtr m_receiveBuffer;
	};

} // namespace Scs

#endif // SCS_RECEIVE_QUEUE_H____


// end --- ScsReceiveQueue.h --- 



// begin --- ScsClient.h --- 

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
#ifndef SCS_CLIENT_H____
#define SCS_CLIENT_H____

namespace Scs
{

	class Client : public IClient
	{
	public:
		Client(const ClientParams & params);
		virtual ~Client() override;

		void Connect() override;
		bool IsConnected() const override { return m_status == Status::Ready ? true : false; }
		bool HasError() const override { return m_error; }

		void OnConnect(ClientOnConnectFn onConnect) override { assert(m_status == Status::Initial); m_onConnect = onConnect; }
		void OnDisconnect(ClientOnDisconnectFn onDisconnect) override { assert(m_status == Status::Initial); m_onDisconnect = onDisconnect; }
		void OnReceiveData(ClientOnReceiveDataFn onReceiveData) override { assert(m_status == Status::Initial); m_onReceiveData = onReceiveData; }
		void OnUpdate(ClientOnUpdateFn onUpdate) override { assert(m_status == Status::Initial); m_onUpdate = onUpdate; }

		void Send(const void * data, size_t bytes) override;

	private:
		void Run();

		enum class Status
		{
			Initial,
			Connecting,
			ConnectionTimeout,
			Ready,
			Shutdown,
		};

		SocketPtr m_socket;
		std::thread m_thread;
		ClientOnConnectFn m_onConnect;
		ClientOnDisconnectFn m_onDisconnect;
		ClientOnReceiveDataFn m_onReceiveData;
		ClientOnUpdateFn m_onUpdate;
		String m_port;
		String m_address;
		long long m_timeoutMs;
		std::atomic<Status> m_status = Status::Initial;
		std::atomic_bool m_error = false;
		SendQueue m_sendQueue;
	};

} // namespace Scs

#endif // SCS_CLIENT_H____


// end --- ScsClient.h --- 



// begin --- ScsServer.h --- 

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
		ClientID m_maxClientId = 0;
		std::atomic<Status> m_status = Status::Initial;
        std::atomic_bool m_shutDown = false;
		std::atomic_bool m_error = false;
	};

} // namespace Scs


#endif // SCS_SERVER_H____


// end --- ScsServer.h --- 



namespace Scs
{
	const uint32_t SEND_THROTTLE_MS = 10;
	const size_t SEND_BUFFER_SIZE = 1024 * 64;
	const size_t RECEIVE_BUFFER_SIZE = 1024 * 128;
}

#endif // SCS_INTERNAL_H____


// end --- ScsInternal.h --- 



using namespace Scs;


#ifdef SCS_WINDOWS
// Static data
static WSADATA s_wsaData;
#endif


bool Scs::Initialize(const InitParams & params)
{
	InitializeInternal(params);
#ifdef SCS_WINDOWS
	// Initialize Winsock v2.2
	int result = WSAStartup(MAKEWORD(2,2), &s_wsaData);
	if (result != 0)
	{
		LogWriteLine("WSAStartup failed: %d", result);
		return false;
	}
#endif
	return true;
}

void Scs::ShutDown()
{
#ifdef SCS_WINDOWS
	WSACleanup();
#endif
}






// end --- Scs.cpp --- 



// begin --- ScsAddress.cpp --- 

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

using namespace Scs;


Address::Address(const String & port, const String & address)
{
	// Client setup

	// Set up structures
	m_address = NULL;
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // Note: Windows fails to connect if left unspecified
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;

	// Resolve the local address (if required) and port to be used by the socket
	const char * addressStr = address.size() > 0 ? address.c_str() : nullptr;
	int result = getaddrinfo(addressStr, port.c_str(), &hints, &m_address);
	if (result != 0)
	{
		LogWriteLine("getaddrinfo failed: %d", result);
	}
	m_current = m_address;
	Log();
}

Address::Address(const String & port, bool passive)
{
	// Server setup

	// Set up structures
	m_address = NULL;
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // Note: Windows fails to connect if left unspecified
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;
	if (passive)
		hints.ai_flags = AI_PASSIVE;

	// Resolve the port to be used by the socket
	int result = getaddrinfo(nullptr, port.c_str(), &hints, &m_address);
	if (result != 0)
	{
		LogWriteLine("getaddrinfo failed: %d", result);
	}
	m_current = m_address;
	Log();
}

Address::~Address()
{
	freeaddrinfo(m_address);
}

void Address::Log() const
{
	LogWriteLine("Logging addresses...");
	char ipstr[INET6_ADDRSTRLEN];
	auto curr = m_address;
	while (curr)
	{
        void *addr;
        const char *ipver;		
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (curr->ai_family == AF_INET) // IPv4
		{ 
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)curr->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } 
		else // IPv6
		{ 
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)curr->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        ScsInetNtoP(curr->ai_family, addr, ipstr, countof(ipstr));
        LogWriteLine("  %s: %s", ipver, ipstr);
	
		// Advance to the next address
		curr = curr->ai_next;
	}
}

bool Address::Next()
{
	if (!m_current)
		return false;
	m_current = m_current->ai_next;
	return m_current ? true : false;
}

AddressPtr Scs::CreateAddress(const String & port, const String & address)
{
	return std::allocate_shared<Address>(Allocator<Address>(), port, address);
}

AddressPtr Scs::CreateAddress(const String & port, bool passive)
{
	return std::allocate_shared<Address>(Allocator<Address>(), port, passive);
}





// end --- ScsAddress.cpp --- 



// begin --- ScsClient.cpp --- 

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

using namespace Scs;

Client::Client(const ClientParams & params) :
	m_port(params.port),
	m_address(params.address),
	m_timeoutMs(static_cast<long long>(params.timeoutSeconds * 1000.0))
{
}

Client::~Client()
{
	m_status = Status::Shutdown;
	if (m_thread.joinable())
		m_thread.join();
}

void Client::Connect()
{
	m_status = Status::Shutdown;
	if (m_thread.joinable())
		m_thread.join();
	m_status = Status::Initial;
	m_error = false;;
	m_thread = std::thread([this]() { this->Run(); });
}

void Client::Run()
{
	// Create address from port and address strings
	AddressPtr address = CreateAddress(m_port, m_address);

	// Create a receive buffer
	BufferPtr receiveBuffer = CreateBuffer();
	receiveBuffer->reserve(RECEIVE_BUFFER_SIZE);

	// Create a receive queue
	ReceiveQueue receiveQueue;

	// Mark initial time
	auto statusTime = std::chrono::system_clock::now();

	// Loop until we get a shutdown request
	while (m_status != Status::Shutdown)
	{
		// We don't want to burn too much CPU while idling.
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (m_status == Status::Initial)
		{
			m_socket = CreateSocket(address);
			m_socket->SetNonBlocking(true);
			if (!m_socket->Connect())
			{
				LogWriteLine("Error connecting client socket.");
				m_status = Status::Shutdown;
				m_error = true;
				break;
			}
			m_status = Status::Connecting;
			statusTime = std::chrono::system_clock::now();

            // On macOS, without this slight delay after a connect attempt, the socket returns immediate
            // success on IsWriteable(), even if no connection is present.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else if (m_status == Status::Connecting)
		{
			if (std::chrono::system_clock::now() > statusTime + std::chrono::milliseconds(m_timeoutMs))
			{
				if (address->Next())
				{
					m_status = Status::Initial;
					statusTime = std::chrono::system_clock::now();
					LogWriteLine("Client failed to connect - trying next address.");
				}
				else
				{
					m_status = Status::ConnectionTimeout;
					LogWriteLine("Client failed to connect.");
				}
			}
			else
			{
				if (m_socket->IsWritable())
				{
                    m_status = Status::Ready;
                    LogWriteLine("Client established connection with server.");
					if (m_onConnect)
						m_onConnect(*this);
				}
			}
		}
		else if (m_status == Status::ConnectionTimeout)
		{
			// TODO: Wait some period and try again for a limited number of tries.  For now, we just fail
			LogWriteLine("Client retry not yet implemented.  Shutting down client.");
			m_error = true;
			m_status = Status::Shutdown;
		}
		else if (m_status == Status::Ready)
		{
			if (m_onUpdate)
				m_onUpdate(*this);

			// Check first to see if we can write to the socket
			if (m_socket->IsWritable())
			{
				if (!m_sendQueue.Empty())
				{
					if (!m_sendQueue.Send(m_socket))
					{
						LogWriteLine("Error sending data to server.  Shutting down connection.");
						m_status = Status::Shutdown;
						break;
					}

					// We need to throttle our connection transmission rate
					std::this_thread::sleep_for(std::chrono::milliseconds(SEND_THROTTLE_MS));
				}
			}

			// Check for incoming data
			if (m_socket->IsReadable())
			{
				// Read data from socket
				receiveBuffer->resize(receiveBuffer->capacity());
				size_t bytesReceived = m_socket->Receive(receiveBuffer->data(), receiveBuffer->capacity(), 0);
				if (bytesReceived)
				{
					// Push data into the receive queue
					receiveBuffer->resize(bytesReceived);
					receiveQueue.Push(receiveBuffer->data(), receiveBuffer->size());
					receiveBuffer->clear();

					// Process data in receive queue
					BufferPtr receivedData = receiveQueue.Pop();
					while (receivedData)
					{
						if (m_onReceiveData)
							m_onReceiveData(*this, receivedData->data(), receivedData->size());
						receivedData = receiveQueue.Pop();
					}
				}
			}
		}
	}

	if (m_onDisconnect)
		m_onDisconnect(*this);
	m_socket = nullptr;
}

void Client::Send(const void * data, size_t bytes)
{
	m_sendQueue.Push(data, bytes);
}

ClientPtr Scs::CreateClient(const ClientParams & params)
{
	return std::allocate_shared<Client>(Allocator<Client>(), params);
}


// end --- ScsClient.cpp --- 



// begin --- ScsCommon.cpp --- 

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

using namespace Scs;


static_assert(sizeof(MessageHeader) == 8, "Message header is incompatible size.");

const size_t BufferSize = 1024;

static InitParams s_params;
static std::mutex s_mutex;


static void DefaultWriteLine(const char * output)
{
	printf("%s", output);
}

void * DefaultAlloc(size_t bytes)
{
	return malloc(bytes);
}

void * DefaultRealloc(void * ptr, size_t bytes)
{
	return realloc(ptr, bytes);
}

void DefaultFree(void * ptr)
{
	free(ptr);
}

BufferPtr Scs::CreateBuffer()
{
	return std::allocate_shared<Buffer>(Allocator<Buffer>());
}

void Scs::InitializeInternal(const InitParams & params)
{
	s_params = params;
	if (!s_params.logFn)
		s_params.logFn = &DefaultWriteLine;
	if (!s_params.allocFn || !s_params.reallocFn || !s_params.freeFn)
	{
		// You must define all memory functions or none
		assert(!s_params.allocFn && !s_params.reallocFn && !s_params.freeFn);
		s_params.allocFn = &DefaultAlloc;
		s_params.reallocFn = &DefaultRealloc;
		s_params.freeFn = &DefaultFree;
	}
}

void Scs::LogWriteLine(const char * format, ...)
{
	std::unique_lock<std::mutex> lock(s_mutex);
	va_list argptr;
	va_start(argptr, format);
	char buffer[BufferSize];
#if defined(SCS_WINDOWS)
	_vsnprintf_s(buffer, BufferSize, _TRUNCATE, format, argptr);
#else
	vsnprintf(buffer, BufferSize, format, argptr);
#endif
	size_t len = strlen(buffer);
	if (len < BufferSize - 2)
	{
		buffer[len] = '\n';
		buffer[len + 1] = 0;
	}
	s_params.logFn(buffer);
	va_end(argptr);
}

void * Scs::Alloc(size_t bytes)
{
	// Initialize must be called before library is used
	assert(s_params.allocFn);
	return s_params.allocFn(bytes);
}

void * Scs::Realloc(void * ptr, size_t bytes)
{
	assert(s_params.reallocFn);
	return s_params.reallocFn(ptr, bytes);
}

void Scs::Free(void * ptr)
{
	assert(s_params.freeFn);
	s_params.freeFn(ptr);
}


// end --- ScsCommon.cpp --- 



// begin --- ScsReceiveQueue.cpp --- 

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

using namespace Scs;


BufferPtr ReceiveQueue::Pop()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_queue.empty())
		return nullptr;
	BufferPtr buffer = m_queue.front();
	m_queue.pop();
	return buffer;
}

void ReceiveQueue::Push(const void * data, size_t bytes)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	const uint8_t * dataBytes = static_cast<const uint8_t *>(data);
	while (bytes)
	{
		// Check to see if this is a new message, and if so, prepare the buffer and write out the header structure
		if (m_messageSize == 0)
		{
			assert(bytes >= sizeof(MessageHeader));
			MessageHeader header;
			memcpy(&header, data, sizeof(MessageHeader));
			if (header.magic != MAGIC_HEADER_VAL)
			{
				LogWriteLine("Transmission error.  Magic header mismatch.");
				return;
			}
			m_messageSize = header.size;
			if (!m_receiveBuffer)
				m_receiveBuffer = CreateBuffer();
			m_receiveBuffer->clear();
			bytes -= sizeof(MessageHeader);
			dataBytes += sizeof(MessageHeader);
		}

		// Don't write out more than the message size yet
		auto bytesToWrite = bytes;
		auto maxBytes = m_messageSize - m_receiveBuffer->size();
		if (bytesToWrite > maxBytes)
			bytesToWrite = maxBytes;

		// Write data into the receive buffer and update our counts
		m_receiveBuffer->insert(m_receiveBuffer->end(), dataBytes, dataBytes + bytesToWrite);
		assert(bytesToWrite <= bytes);
		bytes -= bytesToWrite;
		dataBytes += bytesToWrite;

		if (m_receiveBuffer->size() >= m_messageSize)
		{
			if (m_receiveBuffer->size() > m_messageSize)
			{
				LogWriteLine("Transmission error.  Message exceeds expected size.");
				break;
			}
			else
			{
				m_queue.push(m_receiveBuffer);
			}
			m_receiveBuffer = nullptr;
			m_messageSize = 0;
		}
	}
}




// end --- ScsReceiveQueue.cpp --- 



// begin --- ScsSendQueue.cpp --- 

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

using namespace Scs;


bool SendQueue::Empty() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_queue.empty();
}

bool SendQueue::Send(SocketPtr socket)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	assert(!m_queue.empty());
	auto buffer = m_queue.front();
	size_t bytesSent = 0;
	if (!socket->Send(buffer->data() + m_bytesSent, buffer->size() - m_bytesSent, 0, &bytesSent))
		return false;
	m_bytesSent += bytesSent;
	if (m_bytesSent == buffer->size())
	{
		m_queue.pop_front();
		m_bytesSent = 0;
	}
	return true;
}

void SendQueue::Push(const void * data, size_t bytes)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// Track data and bytes remaining to queue
	uint8_t * ptrData = (uint8_t *)data;
	size_t bytesRemaining = bytes + sizeof(MessageHeader);
	bool firstWrite = true;
	while (bytesRemaining)
	{
		size_t bytesToWrite = std::min(bytesRemaining, SEND_BUFFER_SIZE);
		BufferPtr buffer = CreateBuffer();
		if (firstWrite)
		{
			// Single message sizes over 4GB aren't supported, which is a
			// ridiculous size for a single TCP/IP message anyhow.
			assert(bytes < 0xFFFFFFFF);
			MessageHeader header;
			header.size = static_cast<uint32_t>(bytes);	
			buffer->insert(buffer->end(), reinterpret_cast<uint8_t *>(&header), reinterpret_cast<uint8_t *>(&header) + sizeof(MessageHeader));
			bytesRemaining -= sizeof(MessageHeader);
			bytesToWrite -= sizeof(MessageHeader);
			firstWrite = false;
		}
		buffer->insert(buffer->end(), ptrData, ptrData + bytesToWrite);
		m_queue.push_back(buffer);
		ptrData += bytesToWrite;
		bytesRemaining -= bytesToWrite;
	}
	assert(bytesRemaining == 0);
}




// end --- ScsSendQueue.cpp --- 



// begin --- ScsServer.cpp --- 

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

using namespace Scs;


Server::Server(const ServerParams & params) :
	m_port(params.port),
	m_maxConnections(params.maxConnections),
	m_timeoutMs(static_cast<long long>(params.timeoutSeconds * 1000.0))
{
}

Server::~Server()
{
	LogWriteLine("Shutting down server.");
    m_shutDown = true;
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
		m_onStartListening(*this);
	}

	// Loop until we get a shutdown request
	while (!m_shutDown)
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
                m_shutDown = true;
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
                m_shutDown = true;
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
			// Update callback function called while listening
			if (m_onUpdate)
			{
				std::lock_guard<std::mutex> lock(m_notifierMutex);
				m_onUpdate(*this);
			}

			// Check to see if we've established a connection
			if (m_listenerSocket->IsReadable())
			{
				// Only accept a maxinum number of simultaneous connections
				if (m_connectionList.size() >= m_maxConnections)
				{
					LogWriteLine("Warning: Reached max connections (%u), so new connection has been discarded.", m_maxConnections);
					break;
				}

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
						m_onConnect(*this, connection->clientID);
					}					
					connection->thread = std::thread([this, connection]() { this->RunConnection(connection); });
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
	auto timeoutTime = std::chrono::system_clock::now() + std::chrono::milliseconds(m_timeoutMs);

	// Create a receive buffer
	BufferPtr receiveBuffer = CreateBuffer();
	receiveBuffer->reserve(RECEIVE_BUFFER_SIZE);

	// Each client connection runs in a separate thread, controlled
	// by this function.
	while (connection->connected && !m_shutDown)
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
				timeoutTime = std::chrono::system_clock::now() + std::chrono::milliseconds(m_timeoutMs);
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
						m_onReceiveData(*this, connection->clientID, receivedData->data(), receivedData->size());
					}
					receivedData = connection->receiveQueue.Pop();
				}		

				// Reset timeout
				timeoutTime = std::chrono::system_clock::now() + std::chrono::milliseconds(m_timeoutMs);
			}
		}
	}

	// We're shutting down, so make sure all sockets are disconnected
	// and the connection data structure is removed from the connection
	// list and deleted.
	if (m_onDisconnect)
	{
		std::lock_guard<std::mutex> lock(m_notifierMutex);
		m_onDisconnect(*this, connection->clientID);
	}
	connection->socket = nullptr;
	LogWriteLine("Closing client %d connection thread.", connection->clientID);
}

void Server::Send(ClientID clientId, const void * data, size_t bytes)
{
	std::lock_guard<std::mutex> lock(m_connectionListMutex);
	if (m_connectionList.empty())
		return;
	for (auto connection : m_connectionList)
	{
		if (connection->clientID == clientId)
			connection->sendQueue.Push(data, bytes);
	}
}

void Server::SendAll(const void * data, size_t bytes)
{
	std::lock_guard<std::mutex> lock(m_connectionListMutex);
	if (m_connectionList.empty())
		return;
	BufferPtr buffer = CreateBuffer();
	buffer->insert(buffer->end(), static_cast<const uint8_t *>(data), static_cast<const uint8_t *>(data) + bytes);
	for (auto connection : m_connectionList)
		connection->sendQueue.Push(data, bytes);
}

void Server::StartListening()
{
	m_thread = std::thread([this]() { this->RunListener(); });

	// Lock until the async initialize function is finished
	std::unique_lock<std::mutex> lock(m_connectionListMutex);
	m_stateCondition.wait_for(lock, std::chrono::milliseconds(1), [this]() { return m_status != Status::Listening;  });
}

ServerPtr Scs::CreateServer(const ServerParams & params)
{
	return std::allocate_shared<Server>(Allocator<Server>(), params);
}


// end --- ScsServer.cpp --- 



// begin --- ScsSocket.cpp --- 

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

#ifdef SCS_WINDOWS
// FD_SET macro triggers this warning
#pragma warning( disable : 4127 )
#endif // SCS_WINDOWS

using namespace Scs;


Socket::Socket(AddressPtr address) :
	m_socket(INVALID_SOCKET),
	m_address(address)
{
	// Create the SOCKET
	if (!m_address)
	{
		LogWriteLine("Null socket error");
		return;
	}
	addrinfo * addr = m_address->GetCurrent();
	if (!addr)
	{
		LogWriteLine("Null socket address error");
		return;
	}
	m_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		LogWriteLine("Error at socket(): %ld", SocketLastError);
	}

}

Socket::Socket(AddressPtr address, SOCKET sckt) :
	m_socket(sckt),
	m_address(address)
{
}

Socket::~Socket()
{
	if (m_socket != INVALID_SOCKET)
	{
		shutdown(m_socket, SD_SEND);
#ifdef SCS_WINDOWS
		closesocket(m_socket);
#else
        close(m_socket);
#endif
	}
}

SocketPtr Socket::Accept()
{
	SOCKET newSocket = INVALID_SOCKET;
	newSocket = accept(m_socket, nullptr, nullptr);
	if (newSocket == INVALID_SOCKET)
	{
		LogWriteLine("Socket accept failed with error: %d", SocketLastError);
		return nullptr;
	}
	return CreateSocket(m_address, newSocket);
}

bool Socket::Bind(addrinfo * addr)
{
	// Setup the TCP listening socket
	int result = bind(m_socket, addr->ai_addr, (int)addr->ai_addrlen);
	int lastError = SocketLastError;
	if (result == SOCKET_ERROR && lastError != SCS_EWOULDBLOCK && lastError != EAGAIN && lastError != SCS_EINPROGRESS)
	{
		LogWriteLine("Bind failed with error: %d", lastError);
		return false;
	}
	return true;
}

bool Socket::Connect()
{
	if (m_socket == INVALID_SOCKET)
	{
		LogWriteLine("Invalid socket");
		return false;
	}

	// Connect to server.
	addrinfo * addr = m_address->GetCurrent();
	int result = connect(m_socket, addr->ai_addr, (int)addr->ai_addrlen);
	if (result == 0)
		return true;
	int lastError = SocketLastError;
	if (result == SOCKET_ERROR && lastError != SCS_EWOULDBLOCK && lastError != EAGAIN && lastError != SCS_EINPROGRESS)
	{
		LogWriteLine("Socket connect failed: %d", lastError);
		return false;
	}
	return true;
}

bool Socket::IsInvalid() const
{
	if (m_socket == INVALID_SOCKET)
		return true;
	fd_set socketSet;
	FD_ZERO(&socketSet);
	FD_SET(m_socket, &socketSet);
	timeval tmval;
	tmval.tv_sec = 0;
	tmval.tv_usec = 1000;
	int result = select(static_cast<int>(m_socket + 1), nullptr, nullptr, &socketSet, &tmval);
	int lastError = SocketLastError;
	if (result == SOCKET_ERROR)
	{
		LogWriteLine("Socket IsInvalid() failed: %d", lastError);
		return false;
	}
	return (result == 1) ? true : false;
}

bool Socket::IsReadable() const
{
	fd_set socketSet;
	FD_ZERO(&socketSet);
	FD_SET(m_socket, &socketSet);
	timeval tmval;
	tmval.tv_sec = 0;
	tmval.tv_usec = 1000;
	int result = select(static_cast<int>(m_socket + 1), &socketSet, nullptr, nullptr, &tmval);
	int lastError = SocketLastError;
	if (result == SOCKET_ERROR)
	{
		LogWriteLine("Socket IsReadable() failed: %d", lastError);
		return false;
	}
	return (result == 1) ? true : false;
}

bool Socket::IsWritable() const
{
	fd_set socketSet;
	FD_ZERO(&socketSet);
	FD_SET(m_socket, &socketSet);
	timeval tmval;
	tmval.tv_sec = 0;
	tmval.tv_usec = 1000;
	int result = select(static_cast<int>(m_socket + 1), nullptr, &socketSet, nullptr, &tmval);
	int lastError = SocketLastError;
	if (result == SOCKET_ERROR)
	{
		LogWriteLine("Socket IsWritable() failed: %d", lastError);
		return false;
	}
	return (result == 1) ? true : false;
}

bool Socket::Listen()
{
	// Connect to server.
	int result = listen(m_socket, SOMAXCONN);
	int lastError = SocketLastError;
	if (result == SOCKET_ERROR && lastError != SCS_EWOULDBLOCK && lastError != EAGAIN && lastError != SCS_EINPROGRESS)
	{
		LogWriteLine("Socket listen failed: %d", lastError);
		return false;
	}
	return true;
}

size_t Socket::Receive(void * data, size_t bytes, uint32_t flags)
{
	auto received = recv(m_socket, static_cast<char *>(data), static_cast<int>(bytes), flags);
	if (received == SOCKET_ERROR || received <= 0)
		return 0;
	return static_cast<size_t>(received);
}

bool Socket::Send(void * data, size_t bytes, uint32_t flags, size_t * bytesSent)
{
	assert(bytesSent);
	ssize_t sent = send(m_socket, static_cast<const char*>(data), static_cast<int>(bytes), flags);
	int lastError = SocketLastError;
	if (sent == SOCKET_ERROR || sent <= 0)
	{
		LogWriteLine("Socket send failed: %d", lastError);
		return false;
	}
	*bytesSent += static_cast<size_t>(sent);
	return true;
}

void Socket::SetNonBlocking(bool nonBlocking)
{
	// Set socket to non-blocking
	u_long mode = nonBlocking ? 1 : 0;
	ScsIoCtrl(m_socket, FIONBIO, &mode);
}

void Socket::SetNagle(bool nagle)
{
	// Turn nagling on and off for this socket
	uint32_t flag = nagle ? 1 : 0;
    setsockopt(m_socket, SOL_SOCKET, TCP_NODELAY, (char *) &flag, sizeof(int));
}

SocketPtr Scs::CreateSocket(AddressPtr address)
{
	return std::allocate_shared<Socket>(Allocator<Address>(), address);
}

SocketPtr Scs::CreateSocket(AddressPtr address, SOCKET sckt)
{
	return std::allocate_shared<Socket>(Allocator<Address>(), address, sckt);
}




// end --- ScsSocket.cpp --- 

