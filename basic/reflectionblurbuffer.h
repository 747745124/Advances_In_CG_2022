#ifndef REFLECTIONBLURBUFFER_H
#define REFLECTIONBLURBUFFER_H

#include <glad/glad.h>
#include "common.h"

class ReflectionBlurBuffer
{
public:
	ReflectionBlurBuffer() { refblurbuffer_init(); }
	~ReflectionBlurBuffer() { cleanUp(); }
	void bindToWrite();
	void bindToRead();
	void bindTexture();

private:
	void refblurbuffer_init();
	void cleanUp();
	GLuint refblurbuffer;
	GLuint blurred_text;
};

void ReflectionBlurBuffer::refblurbuffer_init()
{
	unsigned SCR_WIDTH = Common::SCR_WIDTH;
	unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

	glGenFramebuffers(1, &refblurbuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refblurbuffer);

	glGenTextures(1, &blurred_text);
	glBindTexture(GL_TEXTURE_2D, blurred_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurred_text, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ReflectionBlurBuffer::cleanUp()
{
	glDeleteFramebuffers(1, &refblurbuffer);
	glDeleteTextures(1, &blurred_text);
}

void ReflectionBlurBuffer::bindToWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refblurbuffer);
}

void ReflectionBlurBuffer::bindToRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, refblurbuffer);
}

void ReflectionBlurBuffer::bindTexture()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurred_text);
}

#endif // !REFLECTIONBLUR_H
