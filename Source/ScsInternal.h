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

#include "Scs.h"

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
#define LAIR_EWOULDBLOCK      WSAEWOULDBLOCK

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
#include <errno.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR           (-1)
#define SD_RECEIVE             0x00
#define SD_SEND                0x01
#define SD_BOTH                0x02
#define TCP_NODELAY            0x0001
#define SocketLastError		   errno
#define LAIR_EWOULDBLOCK       EWOULDBLOCK

#endif

#include <thread>
#include <vector>
#include <list>
#include <queue>
#include <mutex>
#include <assert.h>

#include "ScsCommon.h"
#include "ScsAddress.h"
#include "ScsSendQueue.h"
#include "ScsReceiveQueue.h"
#include "ScsSocket.h"
#include "ScsClient.h"
#include "ScsServer.h"

namespace Scs
{
	const size_t SEND_BUFFER_SIZE = 1024 * 64;
	const uint32_t SEND_THROTTLE_MS = 10;
	const size_t RECEIVE_BUFFER_SIZE = 1024 * 128;
}

#endif // SCS_INTERNAL_H____
