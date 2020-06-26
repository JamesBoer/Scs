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
