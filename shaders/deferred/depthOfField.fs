#version 330 core

uniform sampler2D focusTexture;
uniform sampler2D blurTexture;
uniform sampler2D positionTexture;

in vec2 TexCoord;

uniform vec2 mouseFocus;
uniform int enabled;

out vec4 fragColor;

void main() {
    float minDistance = 8.0;
    float maxDistance = 12.0;

    vec4 focusColor = texture(focusTexture, TexCoord);
    return;
    fragColor = focusColor;
    if(enabled == 1)
        return;

    vec4 position = texture(positionTexture, TexCoord);
    if(position.a <= 0) {
        return;
    }

    vec4 blurColor = texture(blurTexture, TexCoord);
    vec4 focusPoint = texture(positionTexture, mouseFocus);

    float blur = smoothstep(minDistance, maxDistance, length(position - focusPoint));
    fragColor = mix(blurColor, focusColor, blur);
    return;
}