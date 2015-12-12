#include "Buffer.h"
#include <GL\glew.h>

Buffer::Buffer(int t)
{
	this->Type = t;
	GLuint id;
	glGenBuffers(1, &id);
	this->Id = id;
}

Buffer::~Buffer()
{

}

void Buffer::Bind()
{
	glBindBuffer(this->Type, this->Id);
}

void Buffer::Unbind()
{
	glBindBuffer(this->Type, 0);
}

void Buffer::Dispose()
{
	GLuint id = this->Id;
	glDeleteBuffers(1, &id);
}

void Buffer::BufferData(void* data, int count, int size, int hint)
{
	this->Count = count;
	glBindBuffer(this->Type, this->Id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * size, data, hint);
	glBindBuffer(this->Type, 0);
}
