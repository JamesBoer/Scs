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
#include <random>
#include <chrono>
#include <thread>
#include <cstring>
#include "../../External/Clara/clara.hpp"
#include "../../Source/Scs.h"

#ifdef SCS_WINDOWS
#include <conio.h>
char GetChar()
{
	return static_cast<char>(_getch());
}

char GetCharEcho()
{
	return static_cast<char>(_getche());
}
#else
#include <termios.h>
/* Initialize new terminal i/o settings */
static void initTermios(int echo, termios * told, termios * tnew)
{
	tcgetattr(0, told); /* grab old terminal i/o settings */
	*tnew = *told; /* make new settings same as old settings */
	tnew->c_lflag &= ~ICANON; /* disable buffered i/o */
	tnew->c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
	tcsetattr(0, TCSANOW, tnew); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
static void resetTermios(termios * told)
{
	tcsetattr(0, TCSANOW, told);
}

/* Read 1 character - echo defines echo mode */
static char getch_mode(int echo)
{
	struct termios told, tnew;
	char ch;
	initTermios(echo, &told, &tnew);
	ch = getchar();
	resetTermios(&told);
	return ch;
}

/* Read 1 character without echo */
char GetChar()
{
	return getch_mode(0);
}

/* Read 1 character with echo */
char GetCharEcho()
{
	return getch_mode(1);
}

#endif

using namespace Scs;
using namespace clara;


const uint32_t MIN_PAYLOAD_SIZE = 1;
const uint32_t MAX_PAYLOAD_SIZE = 1024 * 1024 * 4;

const uint32_t MIN_PAYLOAD_INTERVAL_MS = 0;
const uint32_t MAX_PAYLOAD_INTERVAL_MS = 2000;


static void Generate(std::mt19937 & rng, std::vector<uint8_t> & buffer)
{
	// Fill up a test buffer of random size with random data. 
	auto rndVal = std::uniform_real_distribution<double>(0.0, 1.0f)(rng);
	auto size = static_cast<uint32_t>((rndVal * rndVal * rndVal) * (MAX_PAYLOAD_SIZE - MIN_PAYLOAD_SIZE)) + MIN_PAYLOAD_SIZE;
	buffer.clear();
	buffer.reserve(size);
	auto ud = std::uniform_int_distribution<uint32_t>(0, 255);
	for (uint32_t i = 0; i < size; ++i)
		buffer.push_back(static_cast<uint8_t>(ud(rng)));
}

int main(int argc, char ** argv)
{
	// Handle command-line options
	std::string port;
	std::string address;
	bool showHelp = false;
	auto parser = 
		Opt(port, "port")["-p"]["--port"]("Port over which to send IP packets") |
		Opt(address, "address")["-a"]["--address"]("Address of server") |
		Help(showHelp);

	auto result = parser.parse(Args(argc, argv));
	if (!result)
	{
		std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
		return 1;
	}
	else if (showHelp || port.empty())
	{
		parser.writeToStream(std::cout);
		std::cout << 
			"\nIf just a port if specifed, a test server will be started." << 
			"\nWith both a port and address specified, a test client will be started.";
		return 0;
	}

	// Initialize client-server library
	InitParams params;
	Initialize(params);

	// If we have an address, start up a client
	if (!address.empty())
	{
		std::cout << "Press ESC key to exit client test...\n\n";

		// Data for generating payloads with random content and timings
		std::mt19937 rng;
		std::vector<uint8_t> buffer;

		// Start payload timer
		auto now = std::chrono::system_clock::now();
		auto nextPayload = now;

		// Seed random number generator
		uint64_t time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
		rng.seed(static_cast<uint32_t>(time));

		// Create a client
		ClientParams params;
		params.address = address;
		params.port = port;
		auto client = CreateClient(params);

		// Handler for when client connects
		client->OnConnect([&](IClient & client)
		{
			Generate(rng, buffer);
			client.Send(buffer.data(), buffer.size());
			std::cout << "Sending random buffer of " << buffer.size() << " bytes to server.\n";
		});

		// Handler for data received from server
		client->OnReceiveData([&](IClient & client, const void * data, size_t bytes)
		{
			// Check to see if the data received is equal to what was sent out
			auto cmpVal = std::memcmp(data, buffer.data(), std::min(bytes, buffer.size()));
			if (cmpVal == 0)
				std::cout << "Received identical message response from server.\n";
			else
				std::cout << "Received unrecognized message response from server.\n";
			buffer.clear();
			auto intervalTimeMs = std::uniform_int_distribution<uint32_t>(MIN_PAYLOAD_INTERVAL_MS, MAX_PAYLOAD_INTERVAL_MS)(rng);
			nextPayload = std::chrono::system_clock::now() + std::chrono::milliseconds(intervalTimeMs);
		});

		// Handler for per-cycle update
		client->OnUpdate([&](IClient & client)
		{
			// If we don't have buffer data, fill it and send it to the server.  Otherwise, we're waiting
			// for a response from the server.
			if (buffer.empty())
			{
				// Check to see if payload timer has elapsed, and send more random data to the server
				auto now = std::chrono::system_clock::now();
				if (now >= nextPayload)
				{
					Generate(rng, buffer);
					client.Send(buffer.data(), buffer.size());
					std::cout << "Sending random buffer of " << buffer.size() << " bytes to server.\n";			
				}
			}
		});

		// Attempt to make a connection
		client->Connect();

		// Wait until ESC is pressed
		while (GetChar() != 27) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
	}
	// Otherwise, start up a server
	else
	{
		std::cout << "Press ESC key to exit server test...\n\n";

		// Create a server
		ServerParams params;
		params.port = port;
		auto server = CreateServer(params);

		// Handler for incoming data
		server->OnReceiveData([](IServer & server, ClientID clientId, const void * data, size_t bytes)
		{
			std::cout << "Received message of " << bytes << " bytes from client " << clientId << ".\n";
			server.Send(clientId, data, bytes);
		});

		// Start listening for client connections
		server->StartListening();

		// Wait until ESC is pressed
		while (GetChar() != 27) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
	}

	// Shut down client-server library
	ShutDown();

    return 0;
}

