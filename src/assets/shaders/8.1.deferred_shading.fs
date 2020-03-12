#version 330 core
layout (location = 2) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

uniform mat4 VP;
uniform mat4 PreviousVP;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
        // attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;        
    }
    //FragColor = vec4(lighting, 1.0);



	float zOverW = texture(gDepth, TexCoords).r; 
	// H is the viewport position at this pixel in the range -1 to 1.   
	vec4 H = vec4(TexCoords.x * 2 - 1, (1 - TexCoords.y) * 2 - 1, zOverW, 1); 
	// Transform by the view-projection inverse.    
	vec4 D = H * inverse(VP);
	// Divide by w to get the world position.  
	vec4 worldPos = D / D.w;

	// Current viewport position    
	vec4 currentPos = H;
	// Use the world position, and transform by the previous view-projection matrix.    
	vec4 previousPos = worldPos * PreviousVP;
	// Convert to nonhomogeneous points [-1,1] by dividing by w. 
	previousPos /= previousPos.w; 
	// Use this frame's position and last frame's to compute the pixel velocity.    
	vec2 velocity = vec2((currentPos.xy - previousPos.xy)/2.f);

	// Get the initial color at this pixel.
	vec2 NewTexCoor = TexCoords;
	vec4 color = texture(gAlbedoSpec, NewTexCoor); 
	//NewTexCoor += velocity; 
	for(int i = 1; i < 3; ++i, NewTexCoor += velocity) 
	{   
    // Sample the color buffer along the velocity vector.    
    vec4 currentColor = texture(gAlbedoSpec, NewTexCoor);   
    // Add the current color to our color sum.   
    color += currentColor; 
	}   
	// Average all of the samples to get the final blur color.    
	FragColor = color / 3; 

}
