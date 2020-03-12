#version 330 core
layout (location = 2) out vec4 FinalFragColor;

in vec2 TexCoords;

uniform sampler2D gRenderedTex;
uniform sampler2D gDepthTex;

uniform mat4 VP;
uniform mat4 PreviousVP;
uniform int BlurCycleCount;

void main()
{

	float zOverW = texture(gDepthTex, TexCoords).r;
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
	vec2 velDif = currentPos.xy - previousPos.xy;
	vec2 velocity = velDif / 2.f;
	
	// Get the initial color at this pixel.
	vec2 NewTexCoor = TexCoords;
	vec4 color = texture(gRenderedTex, NewTexCoor);
	//NewTexCoor += velocity;
	for(int i = 1; i < BlurCycleCount; ++i, NewTexCoor += velocity)
	{
    // Sample the color buffer along the velocity vector.
    vec4 currentColor = texture(gRenderedTex, NewTexCoor);
    // Add the current color to our color sum.
    color += currentColor;
	}
	//Average all of the samples to get the final blur color.
	FinalFragColor = color / BlurCycleCount;



}
