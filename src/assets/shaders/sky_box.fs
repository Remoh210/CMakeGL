#version 330 core

out vec4 FragColor;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    gAlbedoSpec.rgb = texture(skybox, TexCoords).rgb;
	gAlbedoSpec.a = 0;
}