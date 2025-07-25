#version 420

layout (location = 0) out vec4 tailTexture_out;

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

const int KMAX_POINTS = 512;

layout(std140, binding = 1) uniform PointBuffer {
    TopPointData points[KMAX_POINTS];
};

// Input uniform values
uniform sampler2D tailTexture_in;
uniform int node_count; 

bool pointInTriangle(ivec2 p, ivec2 v0, ivec2 v1, ivec2 v2) {

    // Calculate the area of the main triangle using integer arithmetic.
    int area = (v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y);

    // If the area is 0, the triangle is degenerate (a line or point).
    // In this case, the point is inside only if it lies on the line (or is the point).
    if (area == 0) {
        // Check if p is on the line segment v0v1 or v0v2 (or if all points are the same).
        if(v0 == v1 && v0 == v2) return p == v0; // All points are the same
        if (v0 == v1) return p == v0 || p == v2; // v0 and v1 are the same
        if (v0 == v2) return p == v0 || p == v1; // v0 and v2 are the same
        if (v1 == v2) return p == v1 || p == v0; // v1 and v2 are the same

        return ( (p.x - v0.x) * (v1.y - v0.y) - (v1.x - v0.x) * (p.y - v0.y) == 0 &&
                ( (p.x >= min(v0.x, v1.x) && p.x <= max(v0.x, v1.x) && p.y >= min(v0.y, v1.y) && p.y <= max(v0.y, v1.y)) ||
                  (p.x >= min(v0.x, v2.x) && p.x <= max(v0.x, v2.x) && p.y >= min(v0.y, v2.y) && p.y <= max(v0.y, v2.y)) ||
                  (p.x >= min(v1.x, v2.x) && p.x <= max(v1.x, v2.x) && p.y >= min(v1.y, v2.y) && p.y <= max(v1.y, v2.y))
              )
               );

    }

    // Calculate the areas of the three sub-triangles.
    int area1 = (v1.x - p.x) * (v2.y - p.y) - (v2.x - p.x) * (v1.y - p.y);
    int area2 = (v2.x - p.x) * (v0.y - p.y) - (v0.x - p.x) * (v2.y - p.y);
    int area3 = (v0.x - p.x) * (v1.y - p.y) - (v1.x - p.x) * (v0.y - p.y);

    // Check if the point is inside the triangle.  The signs of the sub-triangle areas
    // must all be the same as the sign of the main triangle area.
    return (area1 * area >= 0 && area2 * area >= 0 && area3 * area >= 0);
}

bool pointInQuad(ivec2 p, ivec2 v0, ivec2 v1, ivec2 v2, ivec2 v3) {

	//vec2 fp = vec2(p);  
    return pointInTriangle(p, v0, v1, v2) ||
           pointInTriangle(p, v0, v2, v3);

}

float pointToSegmentDist(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    vec2 ap = p - a;
    float abLenSq = dot(ab, ab);
    float proj = dot(ap, ab) / abLenSq;
    proj = clamp(proj, 0.0, 1.0);  // Clamp to segment bounds
    vec2 closestPoint = a + proj * ab;
    return length(p - closestPoint);
}

vec3 ComputeGradient(ivec2 p, TopPointData curr)
{
	// Compute distance from the current segment
	float distToSegment = pointToSegmentDist(vec2(p), vec2(curr.start), vec2(curr.end));

	// Define max distance for gradient effect
	float maxDist = distance(vec2(curr.topLeft), vec2(curr.botLeft));  // Approximate segment width

	// Compute gradient factor (0 = white, 1 = blue)
	float gradientFactor = clamp(distToSegment / maxDist, 0.0, 1.0);

	// Interpolate color from white (center) to blue (edges)
	return mix(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 1.0), gradientFactor);
}

const vec4 blackColor = vec4(0.0,0.0,0.0,0.0);
const vec4 whiteColor = vec4(1.0,1.0,1.0,0.0);
const vec4 redColor = vec4(1.0,0.0,0.0,1.0);
const vec4 purpleColor = vec4(1.0,0.0,1.0,1.0);
const vec4 blueColor = vec4(0.0, 0.0, 1.0,1.0);

void main()
{
    // Texel color fetching from texture sampler

	highp int x = int(gl_FragCoord.x);
	highp int y = int(gl_FragCoord.y);
	
    //vec4 texelColor = texture(tailTexture_in, vec2(x,y));

	tailTexture_out = whiteColor;
	
	ivec2 p = {x,y};
	
	if(node_count > 1)
	{
		for (int i = 0; i < node_count - 1; i++) {
		
		
		    if (i == node_count - 2) {
		
				// edge case for last segment
				
				TopPointData curr = points[i];
				if (pointInQuad(p, curr.topLeft, curr.topRight, curr.botRight, curr.botLeft)) {
					tailTexture_out = vec4(ComputeGradient(p,curr),1.0);
				}
				return; 
			}
		
            TopPointData curr = points[i];
			TopPointData next = points[i + 1];
			
			// Fill the current rectangle
            if (pointInQuad(p, curr.topLeft, next.topLeft, next.botLeft, curr.botLeft)) {
			
                tailTexture_out = vec4(ComputeGradient(p,curr),1.0);
            }
			
			if((x == curr.start.x && y == curr.start.y) || 
			   (x == curr.end.x && y == curr.end.y))
			{
				//color start and end purple
			
				tailTexture_out = purpleColor;
				return;
			}
			else 
			{
				//color corners red
				
				ivec2 pointsArray[4] = ivec2[4](
					curr.topLeft, 
					curr.topRight, 
					curr.botLeft, 
					curr.botRight
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