#ifndef HDR_H
#define HDR_H

#include <glad/glad.h>
#include "common.h"

namespace KooNan
{
    class Postprocessor
    {
    public:
        Postprocessor() { post_init(); }
        ~Postprocessor() { cleanUp(); }
        void bindToWrite();
        void bindToRead();
        void bindTexture();

    private:
        void post_init();
        void cleanUp();
        GLuint processbuffer;
        GLuint process_text;
    };

    void Postprocessor::post_init()
    {
        unsigned SCR_WIDTH = Common::SCR_WIDTH;
        unsigned SCR_HEIGHT = Common::SCR_HEIGHT;

        glGenFramebuffers(1, &processbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, processbuffer);

        glGenTextures(1, &process_text);
        glBindTexture(GL_TEXTURE_2D, process_text);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, process_text, 0);

        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Postprocessor::cleanUp()
    {
        glDeleteFramebuffers(1, &processbuffer);
    }

    void Postprocessor::bindToWrite()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, processbuffer);
    }

    void Postprocessor::bindToRead()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, processbuffer);
    }

    void Postprocessor::bindTexture()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, process_text);
    }
}

#endif // !HDR_H