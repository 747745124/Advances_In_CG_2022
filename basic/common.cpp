#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "common.h"

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

void Common::FilterCreation(double gauss[][21])
{
    using namespace std;
    constexpr int smooth_kernel_size = 21;
    // initialising standard deviation to 1.0
    double sigma = 1.0;
    int K = 1;
    // sum is for normalization
    double sum = 0.0;

    for (int i = 0; i < smooth_kernel_size; i++)
    {
        for (int j = 0; j < smooth_kernel_size; j++)
        {
            double x = i - (smooth_kernel_size - 1) / 2.0;
            double y = j - (smooth_kernel_size - 1) / 2.0;
            gauss[i][j] = K * exp(((pow(x, 2) + pow(y, 2)) / ((2 * pow(sigma, 2)))) * (-1));
            sum += gauss[i][j];
        }
    }
    for (int i = 0; i < smooth_kernel_size; i++)
    {
        for (int j = 0; j < smooth_kernel_size; j++)
        {
            gauss[i][j] /= sum;
        }
    }
}
