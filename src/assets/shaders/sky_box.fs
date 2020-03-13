#version 450

in vec3 vsViewDirection;

out vec4 fragment_color;

uniform samplerCube skybox;

void main()
{
    fragment_color = texture(skybox, vsViewDirection);
}