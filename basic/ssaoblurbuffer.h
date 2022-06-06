#ifndef SSAOBLURBUFFER_H
#define SSAOBLURBUFFER_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan {
	class SSAOBlurBuffer {
	public:
		SSAOBlurBuffer() { aoblurbuffer_init(); }
		~SSAOBlurBuffer() { cleanUp(); }
		void bindToWrite();
		void bindToRead();
		void bindTexture();
	private:
		void aoblurbuffer_init();
		void cleanUp();
		GLuint aoblurbuffer;
		GLuint blurred_text1,blurred_text2;
	};

	void SSAOBlurBuffer::aoblurbuffer_init()
	{
		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

		glGenFramebuffers(1, &aoblurbuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, aoblurbuffer);

		glGenTextures(1, &blurred_text1);
		glBindTexture(GL_TEXTURE_2D, blurred_text1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurred_text1, 0);

		glGenTextures(1, &blurred_text2);
		glBindTexture(GL_TEXTURE_2D, blurred_text2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, blurred_text2, 0);

		GLuint drawbufs[] = { GL_COLOR_ATTACHMENT0 ,GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, drawbufs);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SSAOBlurBuffer::cleanUp()
	{
		glDeleteFramebuffers(1, &aoblurbuffer);
		glDeleteTextures(1, &blurred_text1);
		glDeleteTextures(1, &blurred_text2);
	}

	void SSAOBlurBuffer::bindToWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, aoblurbuffer);
	}

	void SSAOBlurBuffer::bindToRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, aoblurbuffer);
	}

	void SSAOBlurBuffer::bindTexture()
	{
		//GL_TEXTURE0-GL_TEXTURE4 is used by gbuffer
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, blurred_text1);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, blurred_text2);
	}
}

#endif