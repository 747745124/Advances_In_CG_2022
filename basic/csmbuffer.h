#ifndef CSMBUFFER_H
#define CSMBUFFER_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan {
	class CSMBuffer {
	public:
		CSMBuffer() { csmbuffer_init(); }
		~CSMBuffer() { cleanUp(); }
		void bindToWrite(int);
		void bindToRead();
		void bindTexture();
		unsigned viewPortSize[6];
	private:
		void csmbuffer_init();
		void cleanUp();
		GLuint csmbuffer;
		//Todo: use a global const int to initialize
		GLuint shadowMap[3];
		static const unsigned CASCACDE_SHADOW_TEXTURE_UNIT0 = GL_TEXTURE6;
	};
	void CSMBuffer::csmbuffer_init()
	{
		unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;
		viewPortSize[0] = SCR_WIDTH * 2;
		viewPortSize[1] = SCR_HEIGHT * 2;
		viewPortSize[2] = SCR_WIDTH;
		viewPortSize[3] = SCR_HEIGHT;
		viewPortSize[4] = SCR_WIDTH / 2;
		viewPortSize[5] = SCR_HEIGHT / 2;
		glGenFramebuffers(1, &csmbuffer);
		glGenTextures(sizeof(shadowMap) / sizeof(shadowMap[0]), shadowMap);
		

		glBindTexture(GL_TEXTURE_2D, shadowMap[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, viewPortSize[0], viewPortSize[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, shadowMap[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, viewPortSize[2], viewPortSize[3], 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, shadowMap[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, viewPortSize[4], viewPortSize[5], 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, csmbuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap[0], 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CSMBuffer::bindToWrite(int Index)
	{
		assert(Index < sizeof(shadowMap) / sizeof(shadowMap[0]));
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, csmbuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap[Index], 0);
	}

	void CSMBuffer::bindToRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, csmbuffer);
	}

	void CSMBuffer::bindTexture()
	{
		for (int i = 0; i < sizeof(shadowMap) / sizeof(shadowMap[0]); i++)
		{
			glActiveTexture(CASCACDE_SHADOW_TEXTURE_UNIT0+i);
			glBindTexture(GL_TEXTURE_2D, shadowMap[i]);
		}
	}

	void CSMBuffer::cleanUp()
	{
		glDeleteFramebuffers(1, &csmbuffer);
		glDeleteTextures(sizeof(shadowMap) / sizeof(shadowMap[0]), shadowMap);
	}

}
#endif // !CSMBUFFER_H
