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
