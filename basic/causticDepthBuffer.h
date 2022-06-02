#ifndef CAUSTICDEPTHBUFFER_H
#define CAUSTICDEPTHBUFFER_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan {
	class CausticDepthBuffer {
	public:
		CausticDepthBuffer() { causticdepthbuffer_init(); }
		~CausticDepthBuffer() { cleanUp(); }
		void bindToWrite();
		void bindToRead();
		void bindTexture();
	private:
		void causticdepthbuffer_init();
		void cleanUp();
		GLuint causticdepthbuffer;
		GLuint causticdepthmap;
	};
	void CausticDepthBuffer::causticdepthbuffer_init()
	{
		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;
		glGenFramebuffers(1, &causticdepthbuffer);
		glGenTextures(1, &causticdepthmap);

		glBindTexture(GL_TEXTURE_2D, causticdepthmap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, causticdepthbuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, causticdepthmap, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void CausticDepthBuffer::cleanUp()
	{
		glDeleteFramebuffers(1, &causticdepthmap);
		glDeleteTextures(1, &causticdepthmap);
	}

	void CausticDepthBuffer::bindToWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, causticdepthbuffer);
	}

	void CausticDepthBuffer::bindToRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, causticdepthbuffer);
	}

	void CausticDepthBuffer::bindTexture()
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, causticdepthmap);
	}
}

#endif // !CAUSTICDEPTHBUFFER_H
