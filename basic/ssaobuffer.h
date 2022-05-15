#ifndef SSAOBUFFER_H
#define SSAOBUFFER_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan
{
	class SSAOBuffer {
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
		GLuint ao_text;
	};

	void SSAOBuffer::aobuffer_init()
	{
		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

		glGenFramebuffers(1, &aobuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, aobuffer);

		glGenTextures(1, &ao_text);
		glBindTexture(GL_TEXTURE_2D, ao_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ao_text, 0);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SSAOBuffer::cleanUp()
	{
		glDeleteFramebuffers(1, &aobuffer);
		glDeleteTextures(1, &ao_text);
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
		//GL_TEXTURE0-GL_TEXTURE3 is used by gbuffer
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao_text);
	}

}


#endif