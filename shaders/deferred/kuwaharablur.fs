#version 330 core

#define MAX_SIZE 5
#define MAX_KERNEL_SIZE ((MAX_SIZE * 2 + 1) * (MAX_SIZE * 2 + 1))

uniform sampler2D Input;
uniform sampler2D Mask;

uniform vec2 parameters;

out vec4 fragColor;
in vec2 aTexCoords;
vec2 texSize  = textureSize(Input, 0).xy;
vec2 texCoord = gl_FragCoord.xy / texSize;

int i = 0;
int j = 0;
int count = 0;

vec3  valueRatios = vec3(0.3, 0.59, 0.11);

float values[MAX_KERNEL_SIZE];

vec4  color       = vec4(0.0);
vec4  meanTemp    = vec4(0.0);
vec4  mean        = vec4(0.0);
float valueMean   =  0.0;
float variance    =  0.0;
float minVariance = -1.0;

void findMean(int i0, int i1, int j0, int j1) {
  meanTemp = vec4(0);
  count    = 0;

  for (i = i0; i <= i1; ++i) {
    for (j = j0; j <= j1; ++j) {
      color = texture( Input,(gl_FragCoord.xy + vec2(i, j)) / texSize);
      meanTemp += color;
      values[count] = dot(color.rgb, valueRatios);
      count += 1;
    }
  }

  meanTemp.rgb /= count;
  valueMean = dot(meanTemp.rgb, valueRatios);

  for (i = 0; i < count; ++i) {
    variance += pow(values[i] - valueMean, 2);
  }

  variance /= count;

  if (variance < minVariance || minVariance <= 0) {
    mean = meanTemp;
    minVariance = variance;
  }
}

void main() {
  fragColor = texture(Input, texCoord);
  float mask = texture(Mask, texCoord).x;
  int size = int(parameters.x);
  if (size <= 0|| mask < 0.1) { return; }
  findMean(-size, 0, -size, 0);
  findMean(0, size, 0, size);
  findMean(-size, 0, 0, size);
  findMean(0, size, -size, 0);
  fragColor.rgb = mean.rgb;
}