#version 330

layout (location = 0) out vec3 tailTexture_out;

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D tailTexture_in;

uniform vec2 start;
uniform vec2 end;

uniform vec2 topLeft;
uniform vec2 topRight;
uniform vec2 botLeft;
uniform vec2 botRight;

void main()
{
    // Texel color fetching from texture sampler

	highp int x = int(gl_FragCoord.x);
	highp int y = int(gl_FragCoord.y);
	
    vec4 texelColor = texture(tailTexture_in, vec2(x,y));

	vec3 blackColor = vec3(0.0,0.0,0.0);
	vec3 redColor = vec3(1.0,0.0,0.0);
	vec3 whiteColor = vec3(1.0,1.0,1.0);
	
	if(start.x == x && start.y == y)
	{
		tailTexture_out = redColor;
	}
	else if(end.x == x && end.y == y)
	{
		tailTexture_out = redColor;
	}
	else if(texelColor.xyz == whiteColor)
	{
		tailTexture_out = whiteColor;
	}

	else tailTexture_out = blackColor;
}