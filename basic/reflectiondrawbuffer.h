#ifndef REFLECTIONDRAWBUFFER_H
#define REFLECTIONDRAWBUFFER_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan {
	class ReflectionDrawBuffer {
	public:
		ReflectionDrawBuffer() { refdrawbuffer_init(); }
		~ReflectionDrawBuffer() { cleanUp(); }
		void bindToWrite();
		void bindToRead();
		void bindTexture();
	private:
		void refdrawbuffer_init();
		void cleanUp();
		GLuint refdrawbuffer;
		GLuint draw_text;
	};

	void ReflectionDrawBuffer::refdrawbuffer_init()
	{
		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

		glGenFramebuffers(1, &refdrawbuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refdrawbuffer);

		glGenTextures(1, &draw_text);
		glBindTexture(GL_TEXTURE_2D, draw_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, draw_text, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ReflectionDrawBuffer::cleanUp()
	{
		glDeleteFramebuffers(1, &refdrawbuffer);
		glDeleteTextures(1, &draw_text);
	}

	void ReflectionDrawBuffer::bindToWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refdrawbuffer);
	}

	void ReflectionDrawBuffer::bindToRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, refdrawbuffer);
	}

	void ReflectionDrawBuffer::bindTexture()
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, draw_text);
	}
}
#endif