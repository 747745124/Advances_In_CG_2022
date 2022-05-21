#ifndef SSRBUFFER_H
#define SSRBUFFER_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan {
	class SSRBuffer {
	public:
		SSRBuffer() { reflectionbuffer_init(); }
		~SSRBuffer() { cleanUp(); }
		void bindToWrite();
		void bindToRead();
		void bindTexture();
	private:
		void reflectionbuffer_init();
		void cleanUp();
		GLuint refbuffer;
		GLuint rColor_text, rTexcoord_text, rDepth_buf;
	};

	void SSRBuffer::reflectionbuffer_init()
	{
		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

		glGenFramebuffers(1, &refbuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refbuffer);

		glGenTextures(1, &rColor_text);
		glBindTexture(GL_TEXTURE_2D, rColor_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rColor_text, 0);

		glGenTextures(1, &rTexcoord_text);
		glBindTexture(GL_TEXTURE_2D, rTexcoord_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rTexcoord_text, 0);

		GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		glGenRenderbuffers(1, &rDepth_buf);
		glBindRenderbuffer(GL_RENDERBUFFER, rDepth_buf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rDepth_buf);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void SSRBuffer::cleanUp()
	{
		glDeleteFramebuffers(1, &refbuffer);
		GLuint texts[] = { rColor_text ,rTexcoord_text };
		glDeleteTextures(sizeof(texts) / sizeof(GLuint), texts);
		glDeleteRenderbuffers(1, &rDepth_buf);
	}

	void SSRBuffer::bindToWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refbuffer);
	}

	void SSRBuffer::bindToRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, refbuffer);
	}

	void SSRBuffer::bindTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rColor_text);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rTexcoord_text);
	}
}


#endif // !REFLECTION_BUFFER_H
