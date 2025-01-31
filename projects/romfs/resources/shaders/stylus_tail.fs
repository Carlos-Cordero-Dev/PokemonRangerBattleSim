#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
//uniform sampler2D tailTexture;

//uniform vec2 start;
//uniform vec2 end;

//uniform vec2 topLeft;
//uniform vec2 topRight;
//uniform vec2 botLeft;
//uniform vec2 botRight;

// Output fragment color
out vec4 finalColor;


void main()
{
    // Texel color fetching from texture sampler

	//highp int x = int(gl_FragCoord.x);
	//highp int y = int(gl_FragCoord.y);
    //vec4 texelColor = texture(tailTexture, vec2(x,y));

	vec3 blackColor = vec3(1.0,0.0,0.0);
	
    finalColor = vec4(blackColor, 1.0);
}