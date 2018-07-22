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
		Allocator(const Allocator &) throw() { };

		template<typename U>
		Allocator(const Allocator<U>&) throw() { };

		template<typename U>
		Allocator & operator = (const Allocator<U> & other) { return *this; }
		Allocator & operator = (const Allocator & other) { return *this; }
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

};

#endif // SCS_COMMON_H____