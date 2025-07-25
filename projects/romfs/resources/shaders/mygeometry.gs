#version 420 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out; // Output a quad (4 vertices)

struct TopPointData {
    ivec2 start;
    ivec2 end;
    ivec2 topLeft;
    ivec2 topRight;
    ivec2 botLeft;
    ivec2 botRight;
};

// **IMPORTANT**: If your TopPointData struct in C++ has floats for positions,
// then the shader struct should also use vec2, not ivec2, to avoid data mismatches
// due to `std140` packing rules and potential precision loss.
// Let's assume for now they are ivec2 in C++ as you defined.
// If your C++ `TopPointData` uses `vec2` (float), use:
/*
struct TopPointData {
    vec2 start;
    vec2 end;
    vec2 topLeft;
    vec2 topRight;
    vec2 botLeft;
    vec2 botRight;
};
*/


const int KMAX_POINTS = 512; // Must match C++ KMAX_POINTS

layout(std140, binding = 1) uniform PointBuffer {
    TopPointData points[KMAX_POINTS];
};

in GS_DATA {
    int nodeIndex;
} gs_in[];

// We will pass pixel coordinates and other segment data to the fragment shader
out VS_DATA {
    vec2 segStart_pixels;   // Segment start point in pixel coordinates
    vec2 segEnd_pixels;     // Segment end point in pixel coordinates
    float invMaxDist;       // Pre-computed inverse max distance
} fs_out;

uniform vec2 screenResolution; // Passed from C++ (screenWidth, screenHeight)

// Helper function to convert pixel coordinates (0 to screenRes) to clip space (-1 to 1)
vec4 pixelToClip(vec2 pixelCoord, vec2 resolution) {
    // Map X from [0, res.x] to [-1, 1]
    float clipX = (pixelCoord.x / resolution.x) * 2.0 - 1.0;
    // Map Y from [0, res.y] to [-1, 1] (and flip Y because OpenGL positive Y is up, screen positive Y is down)
    float clipY = 1.0 - (pixelCoord.y / resolution.y) * 2.0;
    return vec4(clipX, clipY, 0.0, 1.0); // Z=0 for 2D, W=1 for homogeneous coordinates
}


void main() {
    int i = gs_in[0].nodeIndex; // The index of the current segment

    // Fetch the data for the current segment from the UBO
    TopPointData curr = points[i];

    // Calculate invMaxDist here, only once per segment (good optimization)
    // Make sure curr.topLeft and curr.botLeft are correctly populated on CPU.
    vec2 p_topLeft = vec2(curr.topLeft);
    vec2 p_botLeft = vec2(curr.botLeft);
    float maxDist = distance(p_topLeft, p_botLeft);
    float currentInvMaxDist = (maxDist > 0.0) ? (1.0 / maxDist) : 0.0;

    // Pass segment data to the fragment shader (will be interpolated)
    fs_out.segStart_pixels = vec2(curr.start);
    fs_out.segEnd_pixels = vec2(curr.end);
    fs_out.invMaxDist = currentInvMaxDist;

    // Emit the 4 vertices for the quad, converting them to clip space
    gl_Position = pixelToClip(vec2(curr.topLeft), screenResolution);
    EmitVertex();

    gl_Position = pixelToClip(vec2(curr.topRight), screenResolution);
    EmitVertex();

    gl_Position = pixelToClip(vec2(curr.botRight), screenResolution); // Order for triangle strip
    EmitVertex();

    gl_Position = pixelToClip(vec2(curr.botLeft), screenResolution); // Order for triangle strip
    EmitVertex();

    EndPrimitive();
}