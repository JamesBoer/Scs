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
	template<typename T>
	constexpr const int unused(const T &) { return 0; }

	template<typename T, size_t s>
	constexpr size_t countof(T(&)[s]) { return s; }

	// Client
	class IClient;

	class IClientNotify 
	{
	public:
		virtual ~IClientNotify() {}
		virtual void OnConnect(IClient * client)				{ unused(client); }
		virtual void OnDisconnect()								{ }
		virtual void OnReceiveData(void * data, size_t bytes)	{ unused(data); unused(bytes); }
		virtual void OnUpdate()									{ }
	};

	using ClientNotifyPtr = std::shared_ptr<IClientNotify>;

	class IClient
	{
	public:
		virtual ~IClient() {}
		virtual void Send(const void * data, size_t bytes) = 0;
	};

	using ClientPtr = std::shared_ptr<IClient>;
	ClientPtr CreateClient(std::string_view port, std::string_view address, ClientNotifyPtr notify);

	// Server
	class IServer;
	using ServerPtr = std::shared_ptr<IServer>;

	class IServerNotify
	{
	public:
		virtual ~IServerNotify() {}
		virtual void OnStartListening(IServer * server)								{ unused(server); }
		virtual void OnConnect(int32_t clientId)									{ unused(clientId); }
		virtual void OnDisconnect(int32_t clientId)									{ unused(clientId); }
		virtual void OnReceiveData(int32_t clientId, void * data, size_t bytes)		{ unused(clientId); unused(data); unused(bytes); }
		virtual void OnUpdate(int32_t clientId)										{ unused(clientId); }
	};

	using ServerNotifyPtr = std::shared_ptr<IServerNotify>;

	class IServer
	{
	public:
		virtual ~IServer() {}
		virtual void DisconnectClient(int32_t clientId) = 0;
		virtual void Send(const void * data, size_t bytes, int32_t clientId) = 0;
		virtual void SendAll(const void * data, size_t bytes) = 0;
	};

	ServerPtr CreateServer(std::string_view port, ServerNotifyPtr notify);
  
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

}; // namespace Scs


#endif // SCS_H____