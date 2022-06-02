#ifndef CAUSTICSMAPBUFFER_H
#define CAUSTICSMAPBUFFER_H

#include <glad/glad.h>
#include "common.h"
namespace KooNan {
	class CausticMapBuffer {
	public:
		CausticMapBuffer() { causticmapbuf_init(); }
		~CausticMapBuffer() { cleanUp(); }
		void bindToWrite();
		void bindToRead();
		void bindTexture();
	private:
		void causticmapbuf_init();
		void cleanUp();
		GLuint causmapbuffer;
		GLuint caustic_text;
	};

	void CausticMapBuffer::causticmapbuf_init()
	{
		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

		glGenFramebuffers(1, &causmapbuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, causmapbuffer);

		glGenTextures(1, &caustic_text);
		glBindTexture(GL_TEXTURE_2D, caustic_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, caustic_text, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void CausticMapBuffer::cleanUp()
	{
		glDeleteFramebuffers(1, &causmapbuffer);
		glDeleteTextures(1, &caustic_text);
	}

	void CausticMapBuffer::bindToRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, causmapbuffer);
	}
	void CausticMapBuffer::bindTexture()
	{
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, caustic_text);
	}
	void CausticMapBuffer::bindToWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, causmapbuffer);
	}
}

#endif // !CAUSTICSMAPBUFFER_H
