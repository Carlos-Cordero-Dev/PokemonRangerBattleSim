#version 430

layout (location = 0) out vec3 tailTexture_out;

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;


struct TopPointData {
    vec2 start;
    vec2 end;
    vec2 topLeft;
    vec2 topRight;
    vec2 botLeft;
    vec2 botRight;
};

layout(std430, binding = 1) buffer PointBuffer {
    TopPointData points[];
};

// Input uniform values
uniform sampler2D tailTexture_in;
uniform int node_count; 

// Function to check if a point (p) is inside a triangle (v0, v1, v2)
bool pointInTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2) {
    float dX = p.x - v2.x;
    float dY = p.y - v2.y;
    float dX21 = v2.x - v1.x;
    float dY12 = v1.y - v2.y;
    float D = dY12 * (v0.x - v2.x) + dX21 * (v0.y - v2.y);
    float s = dY12 * dX + dX21 * dY;
    float t = (v2.y - v0.y) * dX + (v0.x - v2.x) * dY;

    if (D < 0) return (s <= 0) && (t <= 0) && (s + t >= D);
    return (s >= 0) && (t >= 0) && (s + t <= D);
}

void main()
{
    // Texel color fetching from texture sampler

	highp int x = int(gl_FragCoord.x);
	highp int y = int(gl_FragCoord.y);
	
    vec4 texelColor = texture(tailTexture_in, vec2(x,y));

	vec3 blackColor = vec3(0.0,0.0,0.0);
	vec3 redColor = vec3(1.0,0.0,0.0);
	vec3 whiteColor = vec3(1.0,1.0,1.0);

	tailTexture_out = whiteColor;
	
	vec2 p = gl_FragCoord.xy;
	
	if(node_count > 1)
	{
		for (int i = 0; i < node_count; i++) {
			if (pointInTriangle(p, points[i].topLeft, points[i].topRight, points[i].botRight) || 
				pointInTriangle(p, points[i].topLeft, points[i].botLeft, points[i].botRight)) {
				tailTexture_out = blackColor;
				return;
			}
		}
	}

}