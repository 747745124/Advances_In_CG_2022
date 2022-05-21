#ifndef VIDEORECORD_H
#define VIDEORECORD_H

#include <stdio.h>
#include <glad/glad.h>
#include "common.h"
#include <string>

namespace KooNan
{
    class VideoRecord
    {
	public:
        static FILE* ffmpeg;
        static int* recbuffer;
		
        static void RecordInit(unsigned int framerate,unsigned int width,unsigned int height,std::string output_name = "output.mp4");
        
        static void GrabFrame();

        static void EndRecord();
    };
	
}
#endif