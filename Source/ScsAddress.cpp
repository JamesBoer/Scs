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



