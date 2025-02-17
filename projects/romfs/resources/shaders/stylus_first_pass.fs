#version 430

layout (location = 0) out vec3 tailTexture_out;

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;


struct TopPointData {
    ivec2 start;
    ivec2 end;
    ivec2 topLeft;
    ivec2 topRight;
    ivec2 botLeft;
    ivec2 botRight;
};

layout(std430, binding = 1) buffer PointBuffer {
    TopPointData points[];
};

// Input uniform values
uniform sampler2D tailTexture_in;
uniform int node_count; 

// Function to check if a point (p) is inside a triangle (v0, v1, v2)
bool pointInTriangle(ivec2 p, ivec2 v0, ivec2 v1, ivec2 v2) {
    int dX = p.x - v2.x;
    int dY = p.y - v2.y;
    int dX21 = v2.x - v1.x;
    int dY12 = v1.y - v2.y;
    int D = dY12 * (v0.x - v2.x) + dX21 * (v0.y - v2.y);
    int s = dY12 * dX + dX21 * dY;
    int t = (v2.y - v0.y) * dX + (v0.x - v2.x) * dY;

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
	vec3 purpleColor = vec3(1.0,0.0,1.0);
	vec3 whiteColor = vec3(1.0,1.0,1.0);

	tailTexture_out = whiteColor;
	
	ivec2 p = {x,y};
	
	if(node_count > 1)
	{
		for (int i = 0; i < node_count - 1; i++) {
		
			TopPointData point = points[i];
			

			if (pointInTriangle(p, point.topLeft, point.topRight, point.botRight) || 
				pointInTriangle(p, point.topLeft, point.botLeft, point.botRight)) {
				
				//fill square formed by corners black
				
				//tailTexture_out = blackColor;
			}
			
			if((x == point.start.x && y == point.start.y) || 
			   (x == point.end.x && y == point.end.y))
			{
				//color start and end purple
			
				tailTexture_out = purpleColor;
			}
			else 
			{
				//color corners red
				
				ivec2 pointsArray[4] = ivec2[4](
					point.topLeft, 
					point.topRight, 
					point.botLeft, 
					point.botRight
				);

				for (int j = 0; j < 4; j++) {
					if (x == pointsArray[j].x && y == pointsArray[j].y) {
						tailTexture_out = redColor;
						return;
					}
				}
			}
			
		}
	}

}