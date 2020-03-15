#version 330 core

layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;

uniform sampler2D skyBoxScreenTexture;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

void main()
{
//do test
//if pass
	vec3 color = texture(screenTexture, TexCoords).rgb;
	gAlbedoSpec = vec4(color, 1.0f);

}
