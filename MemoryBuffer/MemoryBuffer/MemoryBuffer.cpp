#include "stdafx.h"
#include "MemoryBuffer.h"

MemoryBuffer* MemoryBuffer::_instance = nullptr;

unsigned char* MemoryBuffer::GetBuffer(size_t size)
{
	if (currentSize < size)
	{
		delete largeBuffer;
		largeBuffer = new (unsigned char[size]);
	}
	return largeBuffer;
}

MemoryBuffer::MemoryBuffer(size_t initSize)
{
	currentSize = initSize;
	largeBuffer = new (unsigned char[currentSize]);
}

void MemoryBuffer::Init()
{
	if (!_instance)
		_instance = new MemoryBuffer(0);
}

void MemoryBuffer::Init(size_t initSize)
{
	if (!_instance)
		_instance = new MemoryBuffer(initSize);
}

MemoryBuffer* MemoryBuffer::Instance()
{
	return _instance;
}
