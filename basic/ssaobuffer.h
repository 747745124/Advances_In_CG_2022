#ifndef SSAOBUFFER_H
#define SSAOBUFFER_H

#include <glad/glad.h>
#include "common.h"

class SSAOBuffer
{
public:
	SSAOBuffer() { aobuffer_init(); }
	~SSAOBuffer() { cleanUp(); }
	void bindToWrite();
	void bindToRead();
	void bindTexture();

private:
	void aobuffer_init();
	void cleanUp();
	GLuint aobuffer;
	GLuint bent_normal_text, bounce_text;
};

void SSAOBuffer::aobuffer_init()
{
	unsigned SCR_WIDTH = Common::SCR_WIDTH;
	unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

	glGenFramebuffers(1, &aobuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, aobuffer);

	glGenTextures(1, &bent_normal_text);
	glBindTexture(GL_TEXTURE_2D, bent_normal_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bent_normal_text, 0);

	glGenTextures(1, &bounce_text);
	glBindTexture(GL_TEXTURE_2D, bounce_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bounce_text, 0);

	GLuint drawbufs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, drawbufs);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOBuffer::cleanUp()
{
	glDeleteFramebuffers(1, &aobuffer);
	glDeleteTextures(1, &bent_normal_text);
}

void SSAOBuffer::bindToWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, aobuffer);
}

void SSAOBuffer::bindToRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, aobuffer);
}

void SSAOBuffer::bindTexture()
{
	// GL_TEXTURE0-GL_TEXTURE4 is used by gbuffer
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, bent_normal_text);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, bounce_text);
};

#endif