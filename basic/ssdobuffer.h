#ifndef SSDOBUFFER_H
#define SSDOBUFFER_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan
{
    class SSDOBuffer {
        public:
        SSDOBuffer(){dobuffer_init();}
        ~SSDOBuffer(){cleanUp();}
        void bindToWrite();
        void bindToRead();
        void bindTexture();
        private:
        void dobuffer_init();
        void cleanUp();
        GLuint dobuffer;
        GLuint do_text;
    };

    void SSDOBuffer::dobuffer_init()
    {
        unsigned SCR_WIDTH = Common::SCR_WIDTH;
		unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

		glGenFramebuffers(1, &dobuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dobuffer);

		glGenTextures(1, &do_text);
		glBindTexture(GL_TEXTURE_2D, do_text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, do_text, 0);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSDOBuffer::cleanUp()
    {
        glDeleteFramebuffers(1,&dobuffer);
        glDeleteTextures(1,&do_text);
    }

    void SSDOBuffer::bindToWrite()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,dobuffer);
    }

    void SSDOBuffer::bindToRead()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER,dobuffer);
    }

    void SSDOBuffer::bindTexture()
    {
        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_2D,do_text);
    }
}
#endif