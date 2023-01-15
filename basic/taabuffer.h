#ifndef TAABUFFER_H
#define TAABUFFER_H

#include <glad/glad.h>
#include "common.h"

class TAABuffer
{
public:
	TAABuffer() { taabuffer_init(); }
	~TAABuffer() { cleanUp(); }
	void bindToWrite();
	void bindToRead();
	void bindTexture();
	void copyToLast();

private:
	void taabuffer_init();
	void cleanUp();
	GLuint taabuffer;
	GLuint currFrame_text, lastFrame_text, depth_text;
};
void TAABuffer::taabuffer_init()
{
	unsigned SCR_WIDTH = Common::SCR_WIDTH;
	unsigned SCR_HEIGHT = Common::SCR_HEIGHT;
	glGenFramebuffers(1, &taabuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, taabuffer);

	glGenTextures(1, &currFrame_text);
	glBindTexture(GL_TEXTURE_2D, currFrame_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, currFrame_text, 0);

	glGenTextures(1, &lastFrame_text);
	glBindTexture(GL_TEXTURE_2D, lastFrame_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lastFrame_text, 0);

	glGenTextures(1, &depth_text);
	glBindTexture(GL_TEXTURE_2D, depth_text);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_text, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TAABuffer::copyToLast()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	bindToWrite();
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlitFramebuffer(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TAABuffer::cleanUp()
{
	glDeleteFramebuffers(1, &taabuffer);
	glDeleteTextures(1, &lastFrame_text);
	glDeleteTextures(1, &currFrame_text);
	glDeleteTextures(1, &depth_text);
}

void TAABuffer::bindToWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, taabuffer);
}

void TAABuffer::bindToRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, taabuffer);
}

void TAABuffer::bindTexture()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, currFrame_text);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lastFrame_text);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depth_text);
}

#endif // !TAABUFFER_H
