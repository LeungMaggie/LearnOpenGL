#version 430 core
out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D tex;

void main()
{
  vec3 texCol = texture(tex, texCoord).rgb;
  fragColor = vec4(texCol, 1.0);
}
