#ifndef VIDEORECORD_H
#define VIDEORECORD_H

#include <stdio.h>
#include <glad/glad.h>
#include <common.h>

namespace KooNan
{
    class VideoRecord
    {
	public:
        static FILE* ffmpeg;
        static int* recbuffer;
		
        static void RecordInit(unsigned int framerate,unsigned int width,unsigned int height,string output_name = "output.mp4")
        {
			char* cmd = new char[200];
			sprintf(cmd, "ffmpeg -r %u -f rawvideo -pix_fmt rgba -s %ux%u -i - "
				"-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip %s", framerate, width, height, output_name.c_str());
#ifdef _WIN64
            ffmpeg = _popen(cmd, "wb");
#elif _APPLE_
            ffmpeg = popen(cmd, "wb");
#endif
			delete[] cmd;
			recbuffer = new int[Common::SCR_WIDTH*Common::SCR_HEIGHT];
        }
        static void GrabFrame()
        {
            glReadPixels(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, recbuffer);

		    fwrite(recbuffer, sizeof(int)*Common::SCR_WIDTH*Common::SCR_HEIGHT, 1, ffmpeg);
        }
        static void EndRecord()
        {
            delete[] recbuffer;
#ifdef _WIN64
            _pclose(ffmpeg);
#elif _APPLE_
	        pclose(ffmpeg);
#endif

        }
    };
	FILE* VideoRecord::ffmpeg = NULL;
	int* VideoRecord::recbuffer = NULL;
}
#endif