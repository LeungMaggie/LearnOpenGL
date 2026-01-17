#version 330 core
out vec4 fragColor;

in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

void main()
{
  // fragColor = texture(texture1, texCoord) * vec4(ourColor, 1.0);
  // fragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
  fragColor = mix(texture(texture1, texCoord), texture(texture2, vec2(-texCoord.x, texCoord.y)), mixValue);
}
