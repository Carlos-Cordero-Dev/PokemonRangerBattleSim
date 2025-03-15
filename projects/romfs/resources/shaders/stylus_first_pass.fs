#version 100 //(version used for openglES2)

precision mediump float;

uniform sampler2D topPointDataTexture;
uniform int textureWidth;
uniform int node_count;

vec2 UnpackVec2FromRGBA8(vec4 texel) {
    float x = texel.r * 255.0 + texel.b * 255.0 * 256.0;
    float y = texel.g * 255.0 + texel.a * 255.0 * 256.0;
    return vec2(x, y);
}

vec2 FetchPoint(int index) {
    float x = (float(index) + 0.5) / float(textureWidth);
    return UnpackVec2FromRGBA8(texture2D(topPointDataTexture, vec2(x, 0.5)));
}

bool pointInTriangle(vec2 p, vec2 v0, vec2 v1, vec2 v2) {
    float area = (v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y);
    if (area == 0.0) return false;

    float area1 = (v1.x - p.x) * (v2.y - p.y) - (v2.x - p.x) * (v1.y - p.y);
    float area2 = (v2.x - p.x) * (v0.y - p.y) - (v0.x - p.x) * (v2.y - p.y);
    float area3 = (v0.x - p.x) * (v1.y - p.y) - (v1.x - p.x) * (v0.y - p.y);

    return (area1 * area >= 0.0 && area2 * area >= 0.0 && area3 * area >= 0.0);
}

bool pointInQuad(vec2 p, vec2 v0, vec2 v1, vec2 v2, vec2 v3) {
    return pointInTriangle(p, v0, v1, v2) || pointInTriangle(p, v0, v2, v3);
}

float pointToSegmentDist(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    vec2 ap = p - a;
    float proj = clamp(dot(ap, ab) / dot(ab, ab), 0.0, 1.0);
    return length(p - (a + proj * ab));
}

vec3 ComputeGradient(vec2 p, vec2 start, vec2 end, vec2 topLeft, vec2 botLeft) {
    float distToSegment = pointToSegmentDist(p, start, end);
    float maxDist = distance(topLeft, botLeft);
    float gradientFactor = clamp(distToSegment / maxDist, 0.0, 1.0);
    return mix(vec3(1.0), vec3(0.0, 0.0, 1.0), gradientFactor);
}

void main() {
    vec2 p = gl_FragCoord.xy;
    vec4 whiteColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 redColor = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 purpleColor = vec4(1.0, 0.0, 1.0, 1.0);

    gl_FragColor = whiteColor;
    
	highp int x = int(gl_FragCoord.x);
	highp int y = int(gl_FragCoord.y);
	
	bool found = false;

    if (node_count > 1) {
        for (int i = 0; i < node_count - 1; i++) {

            vec2 start = UnpackVec2FromRGBA8(texture2D(topPointDataTexture, vec2(float(i) / float(textureWidth), 0.0)));
            vec2 end = UnpackVec2FromRGBA8(texture2D(topPointDataTexture, vec2(float(i + 1) / float(textureWidth), 0.0)));
            vec2 topLeft = UnpackVec2FromRGBA8(texture2D(topPointDataTexture, vec2(float(i + 2) / float(textureWidth), 0.0)));
            vec2 topRight = UnpackVec2FromRGBA8(texture2D(topPointDataTexture, vec2(float(i + 3) / float(textureWidth), 0.0)));
            vec2 botLeft = UnpackVec2FromRGBA8(texture2D(topPointDataTexture, vec2(float(i + 4) / float(textureWidth), 0.0)));
            vec2 botRight = UnpackVec2FromRGBA8(texture2D(topPointDataTexture, vec2(float(i + 5) / float(textureWidth), 0.0)));

            // Handle last segment separately
            if (i == node_count - 2) {
                if (pointInQuad(p, topLeft, topRight, botRight, botLeft)) {
                    //gl_FragColor = vec4(ComputeGradient(p, start, end, topLeft, botLeft), 1.0);
                    found = true;
                }
            } else {
                vec4 nextTexel1 = texture2D(topPointDataTexture, vec2(float(i + 6) / float(textureWidth), 0.0));
                vec4 nextTexel2 = texture2D(topPointDataTexture, vec2(float(i + 7) / float(textureWidth), 0.0));

                vec2 nextTopLeft = UnpackVec2FromRGBA8(nextTexel1);
                vec2 nextBotLeft = UnpackVec2FromRGBA8(nextTexel2);

                if (pointInQuad(p, topLeft, nextTopLeft, nextBotLeft, botLeft)) {
                    //gl_FragColor = vec4(ComputeGradient(p, start, end, topLeft, botLeft), 1.0);
                    found = true;
                }
            }

            // Mark start and end points in purple
            if ((x == int(start.x) && y == int(start.y)) || 
                (x == int(end.x) && y == int(end.y))) {
                gl_FragColor = purpleColor;
                found = true;
            } else {
                // Mark corners in red
                vec2 pointsArray[4];
                pointsArray[0] = topLeft;
                pointsArray[1] = topRight;
                pointsArray[2] = botLeft;
                pointsArray[3] = botRight;

                for (int j = 0; j < 4; j++) {
                    if (x == int(pointsArray[j].x) && y == int(pointsArray[j].y)) {
                        gl_FragColor = redColor;
                        found = true;
                    }
                }
            }
        }
    }

    if (!found) {
        //gl_FragColor = whiteColor; // Ensure output is always set
    }
}
