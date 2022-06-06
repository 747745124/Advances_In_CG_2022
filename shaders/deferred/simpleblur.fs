#version 330 core
in vec2 aTexCoords;

out vec4 fragColor;

uniform sampler2D Input1;
uniform sampler2D Input2;
layout (location = 0) out vec4 output1;
layout (location = 1) out vec4 output2;
const int blurSize = 4; // use size of noise texture (4x4)

void main() 
{
   vec2 texelSize = 1.0 / vec2(textureSize(Input1, 0));
   vec4 result1 = vec4(0.0);
   vec4 result2 = vec4(0.0);
   int count = 0;
   for (int x = 0; x <= blurSize; ++x) 
   {
      for (int y = 0; y <= blurSize; ++y) 
      {
         vec2 offset = (vec2(-2.0) + vec2(float(x), float(y))) * texelSize;
         vec4 color1 = texture(Input1, aTexCoords + offset);
         vec4 color2 = texture(Input2, aTexCoords + offset);
         result1 += color1;
         result2 += color2;
         count += 1;
      }
   }
 
   output1 = result1 / float(count);
   output2 = result2 / float(count);
}