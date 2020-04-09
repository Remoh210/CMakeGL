#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
//layout (location = 3) out vec3 gDepth;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
	vec3 Normal;
	vec3 Tangent;
	mat3 TBN;
} fs_in;

uniform bool bUseDiffuseColour;
uniform vec4 DiffuseColour;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;



vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(fs_in.Normal);
    vec3 Tangent = normalize(fs_in.Tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(texture_normal1, fs_in.TexCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

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
		 gNormal = CalcBumpedNormal();
	}
   
    // store specular intensity in gAlbedoSpec's alpha component
   
}