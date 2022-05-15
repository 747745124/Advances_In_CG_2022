#ifndef GBUFFER_H
#define GBUFFER_H
#include <glad/glad.h>
#include "common.h"

namespace KooNan {
	class GBuffer {
	public:
		GBuffer() { gbuffer_init(); }
		~GBuffer() { cleanUp(); }
		void bindToRead();
		void bindToWrite();
		void bindTexture();
	private:
		void gbuffer_init();
		void cleanUp();
		GLuint gbuffer;
		GLuint gPos_text, gNorm_text, gAlbeSpec_text, gDepth_buf, gReflect_mask;
	};

	void GBuffer::gbuffer_init()
	{
		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;
		//Generate and bind buffer
		glGenFramebuffers(1, &gbuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer);
		//Position
		glGenTextures(1, &gPos_text);
		glBindTexture(GL_TEXTURE_2D, gPos_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPos_text, 0);
		//Normal
		glGenTextures(1, &gNorm_text);
		glBindTexture(GL_TEXTURE_2D, gNorm_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNorm_text, 0);
		//Diffuse and specular color
		glGenTextures(1, &gAlbeSpec_text);
		glBindTexture(GL_TEXTURE_2D, gAlbeSpec_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbeSpec_text, 0);

		//Reflection mask
		glGenTextures(1, &gReflect_mask);
		glBindTexture(GL_TEXTURE_2D, gReflect_mask);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gReflect_mask, 0);

		GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		//Depth
		glGenRenderbuffers(1, &gDepth_buf);
		glBindRenderbuffer(GL_RENDERBUFFER, gDepth_buf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth_buf);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		
		//Restore to default
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GBuffer::cleanUp()
	{
		glDeleteFramebuffers(1, &gbuffer);
		GLuint texts[] = { gPos_text ,gNorm_text ,gAlbeSpec_text,gReflect_mask };
		glDeleteTextures(sizeof(texts) / sizeof(GLuint), texts);
		glDeleteRenderbuffers(1, &gDepth_buf);
	}

	void GBuffer::bindToRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer);
	}
	void GBuffer::bindTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPos_text);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNorm_text);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbeSpec_text);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gReflect_mask);
	}
	void GBuffer::bindToWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gbuffer);
	}
}

#endif // !GBUFFER_H
