#pragma once
class MemoryBuffer
{
private:
	size_t currentSize;
	unsigned char* largeBuffer;
	static MemoryBuffer* _instance;
	MemoryBuffer(size_t);
public:
	unsigned char* GetBuffer(size_t);
	static void Init();
	static void Init(size_t);
	static MemoryBuffer* Instance();
};