#ifndef SSREFLECTUVBUFFER_H
#define SSREFLECTUVBUFFER_H

#include <glad/glad.h>
#include "common.h"

class SSReflectUVBuffer
{
public:
	SSReflectUVBuffer() { reflectionbuffer_init(); }
	~SSReflectUVBuffer() { cleanUp(); }
	void bindToWrite();
	void bindToRead();
	void bindTexture();

private:
	void reflectionbuffer_init();
	void cleanUp();
	GLuint refbuffer;
	GLuint rTexcoord_text;
};

void SSReflectUVBuffer::reflectionbuffer_init()
{
	unsigned SCR_WIDTH = Common::SCR_WIDTH;
	unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

	glGenFramebuffers(1, &refbuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refbuffer);

	glGenTextures(1, &rTexcoord_text);
	glBindTexture(GL_TEXTURE_2D, rTexcoord_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rTexcoord_text, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void SSReflectUVBuffer::cleanUp()
{
	glDeleteFramebuffers(1, &refbuffer);
	glDeleteTextures(1, &rTexcoord_text);
}

void SSReflectUVBuffer::bindToWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refbuffer);
}

void SSReflectUVBuffer::bindToRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, refbuffer);
}

void SSReflectUVBuffer::bindTexture()
{
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rTexcoord_text);
}

#endif // !REFLECTION_BUFFER_H
