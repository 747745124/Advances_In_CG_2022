#version 330 core
in vec2 aTexCoords;

out vec4 fragColor;

uniform sampler2D Input;
const int blurSize = 4; // use size of noise texture (4x4)

void main() 
{
   vec2 texelSize = 1.0 / vec2(textureSize(Input, 0));
   vec4 result = vec4(0.0);
   int count = 0;
   for (int x = 0; x <= blurSize; ++x) 
   {
      for (int y = 0; y <= blurSize; ++y) 
      {
         vec2 offset = (vec2(-2.0) + vec2(float(x), float(y))) * texelSize;
         vec4 color = texture(Input, aTexCoords + offset);
         result += color;
         count += 1;
      }
   }
 
   fragColor = result / float(count);
}