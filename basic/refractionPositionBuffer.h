#ifndef REFRACTIONPOSITIONBUFFER_H
#define REFRACTIONPOSITIONBUFFER_H

#include <glad/glad.h>
#include "common.h"

class RefractionPositionBuffer
{
public:
	RefractionPositionBuffer() { refposbuffer_init(); }
	~RefractionPositionBuffer() { cleanUp(); }
	void bindToWrite();
	void bindToRead();
	void bindTexture();

private:
	void refposbuffer_init();
	void cleanUp();
	GLuint refposbuffer;
	GLuint pos_text;
};

void RefractionPositionBuffer::refposbuffer_init()
{
	unsigned SCR_WIDTH = Common::SCR_WIDTH;
	unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

	glGenFramebuffers(1, &refposbuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refposbuffer);

	glGenTextures(1, &pos_text);
	glBindTexture(GL_TEXTURE_2D, pos_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pos_text, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RefractionPositionBuffer::cleanUp()
{
	glDeleteFramebuffers(1, &refposbuffer);
	glDeleteTextures(1, &pos_text);
}

void RefractionPositionBuffer::bindToRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, refposbuffer);
}
void RefractionPositionBuffer::bindTexture()
{
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, pos_text);
}
void RefractionPositionBuffer::bindToWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refposbuffer);
}

#endif // !REFRACTIONPOSITIONBUFFER_H
