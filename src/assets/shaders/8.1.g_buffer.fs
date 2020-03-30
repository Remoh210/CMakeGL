#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
//layout (location = 3) out vec3 gDepth;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
	vec3 Normal;
	mat3 TBN;
} fs_in;

uniform bool bUseDiffuseColour;
uniform vec4 DiffuseColour;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = fs_in.FragPos;

    // also store the per-fragment normals into the gbuffer
    

    // and the diffuse per-fragment color
	if(bUseDiffuseColour)
	{
		gAlbedoSpec.rgb = DiffuseColour.rgb;
		gNormal = normalize(fs_in.Normal * 2.0 - 1.0);
	}
	else
	{
		 gAlbedoSpec.rgb = texture(texture_diffuse1, fs_in.TexCoords).rgb;
		 gAlbedoSpec.a = texture(texture_specular1, fs_in.TexCoords).r;
		 vec3 normal = normalize(texture(texture_normal1, fs_in.TexCoords).rgb * 2.0 - 1.0);
		 gNormal = normalize(fs_in.TBN * normal);
	}
   
    // store specular intensity in gAlbedoSpec's alpha component
   
}