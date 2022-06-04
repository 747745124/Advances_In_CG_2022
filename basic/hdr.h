#ifndef HDRBUFFER_H
#define HDRBUFFER_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan
{
	class HDRProcessor
	{
	public:
		HDRProcessor() { hdr_init(); }
		~HDRProcessor() { cleanUp(); }
		void bindToWrite();
		void bindToRead();
		void bindTexture();
		void bindFramebuffer();

	private:
		void hdr_init();
		void cleanUp();
		GLuint hdrBuffer;
		GLuint color_text;
		GLuint hdr_depth;
	};

	void HDRProcessor::hdr_init()
	{

		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

		glGenFramebuffers(1, &hdrBuffer);

		glGenTextures(1, &color_text);
		glBindTexture(GL_TEXTURE_2D, color_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_text, 0);

		glGenRenderbuffers(1, &hdr_depth);
		glBindRenderbuffer(GL_RENDERBUFFER, hdr_depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

		glBindFramebuffer(GL_FRAMEBUFFER, hdrBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_text, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdr_depth);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void HDRProcessor::cleanUp()
	{
		glDeleteFramebuffers(1, &hdrBuffer);
		glDeleteTextures(1, &color_text);
	}

	void HDRProcessor::bindToWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrBuffer);
	}

	void HDRProcessor::bindToRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrBuffer);
	}

	void HDRProcessor::bindTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, color_text);
	}
}

#endif
