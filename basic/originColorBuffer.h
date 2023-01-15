#ifndef ORIGINCOLORBUFFER_H
#define ORIGINCOLORBUFFER_H

#include <glad/glad.h>
#include "common.h"

class OriginColorBuffer
{
public:
	OriginColorBuffer() { origincolorbuffer_init(); }
	~OriginColorBuffer() { cleanUp(); }
	void bindToWrite();
	void bindToRead();
	void bindTexture();

private:
	void origincolorbuffer_init();
	void cleanUp();
	GLuint origincolorbuffer;
	GLuint rColor_text;
};

void OriginColorBuffer::origincolorbuffer_init()
{
	unsigned SCR_WIDTH = Common::SCR_WIDTH;
	unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

	glGenFramebuffers(1, &origincolorbuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, origincolorbuffer);

	glGenTextures(1, &rColor_text);
	glBindTexture(GL_TEXTURE_2D, rColor_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rColor_text, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OriginColorBuffer::cleanUp()
{
	glDeleteFramebuffers(1, &origincolorbuffer);
	glDeleteTextures(1, &rColor_text);
}

void OriginColorBuffer::bindToWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, origincolorbuffer);
}

void OriginColorBuffer::bindToRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, origincolorbuffer);
}

void OriginColorBuffer::bindTexture()
{
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, rColor_text);
}

#endif // !ORIGINCOLORBUFFER_H
