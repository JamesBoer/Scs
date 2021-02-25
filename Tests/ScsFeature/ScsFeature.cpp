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

#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <cstring>
#include <assert.h>
#include <stdexcept>
#include "../../Source/Scs.h"

#ifndef NDEBUG
#define REQUIRE assert
#else
#define REQUIRE(x) { if (!(x)) throw new std::runtime_error("Failure for condition: " #x); }
#endif

using namespace Scs;


int main(int argc, char ** argv)
{

	// Initialize client-server library
	InitParams params;
	Initialize(params);

	// Create a server
	ServerParams serverParams;
	serverParams.port = "5656";
	serverParams.maxConnections = 100;
	serverParams.timeoutSeconds = 15.0;
	auto server = CreateServer(serverParams);

	// Handler for when server connects to client
	std::atomic<uint32_t> serverConnections = 0;
	server->OnConnect([&] (const IServer &, int32_t) { ++serverConnections; });

	// Start listening for client connections
	server->StartListening();

	// Start up maxConnections number of clients, and ensure they all connect to the server
	std::vector<std::thread> threads;
	std::atomic<uint32_t> clientConnections = 0;
	for (uint32_t i = 0; i < serverParams.maxConnections; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		threads.emplace_back(std::thread([&] () {

			// Create a client
			ClientParams clientParams;
			clientParams.address = "127.0.0.1";
			clientParams.port = "5656";
			clientParams.timeoutSeconds = 5.0;
			auto client = CreateClient(clientParams);

			// Handler for when client connects
			client->OnConnect([&] (const IClient &) { ++clientConnections; });

			// Attempt to make a connection
			client->Connect();

			// Wait until client has connected or signalled an error
			while (!client->IsConnected() && !client->HasError())
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}));
	}

	// Wait for server to register max connections
	auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(static_cast<long long>(serverParams.timeoutSeconds * 1000.0f));
	while (serverConnections < serverParams.maxConnections)
	{
		REQUIRE(!server->HasError());
		if (std::chrono::system_clock::now() > timeout)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// Join all threads
	for (auto & t : threads)
		t.join();

	// Both client and server connections should equal max connections
	REQUIRE(clientConnections == serverParams.maxConnections);
	REQUIRE(serverConnections == serverParams.maxConnections);

	// Shut down client-server library
	ShutDown();

    return 0;
}

