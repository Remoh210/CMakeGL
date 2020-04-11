#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
	vec3 Normal;
	vec3 Tangent;
    mat3 TBN;
} vs_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 MVP;


void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz; 
    vs_out.TexCoords = aTexCoords;
    
  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vec3 T = normalize(normalMatrix * aTangent);
  vec3 N = normalize(normalMatrix * aNormal);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  vs_out.TBN = transpose(mat3(T, B, N));
  
  vs_out.Normal = (model * vec4(aNormal, 0.0)).xyz;
  vs_out.Tangent = (model * vec4(aTangent, 0.0)).xyz;

  gl_Position = MVP * vec4(aPos, 1.0f);	
}