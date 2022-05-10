#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "common.h"
namespace KooNan
{
    glm::mat4 Common::GetPerspectiveMat(Camera &cam)
    {
        return glm::perspective(glm::radians(cam.Zoom),
                                (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT,
                                Common::perspective_clipping_near, Common::perspective_clipping_far);
    }

    unsigned int Common::SCR_WIDTH = 1920;
	unsigned int Common::SCR_HEIGHT = 1080;
	float Common::perspective_clipping_near = 0.1f;
	float Common::perspective_clipping_far = 1000.0f;
	const std::string Common::saveFileName = "Save.json";
}