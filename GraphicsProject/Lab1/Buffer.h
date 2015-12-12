#pragma once
class Buffer
{
public:
	int Id;
	int Type;
	int Count;

	Buffer(int);
	~Buffer();	

	void Bind();
	void Unbind();
	void Dispose();

	void BufferData(void*, int, int, int);
};

