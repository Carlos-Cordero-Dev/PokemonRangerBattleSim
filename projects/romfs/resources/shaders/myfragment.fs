#version 420 core

layout (location = 0) out vec4 tailTexture_out;

// Input interpolated from Geometry Shader
in VS_DATA {
    vec2 segStart_pixels;   // Segment start point in pixel coordinates (interpolated across quad)
    vec2 segEnd_pixels;     // Segment end point in pixel coordinates (interpolated across quad)
    float invMaxDist;       // Pre-computed inverse max distance (interpolated, but constant across quad)
} fs_in;

// Helper function: point to line segment distance (same as your original, but now uses vec2)
float pointToSegmentDist(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    vec2 ap = p - a;
    float abLenSq = dot(ab, ab);

    // Handle degenerate segment (point) case to avoid division by zero
    if (abLenSq < 0.0001) { // Small epsilon to check if segment is effectively a point
        return distance(p, a); // Distance to the point itself
    }

    float proj = dot(ap, ab) / abLenSq;
    proj = clamp(proj, 0.0, 1.0); // Clamp to segment bounds

    vec2 closestPoint = a + proj * ab;
    return distance(p, closestPoint); // Use distance() for length which includes sqrt
}

// Helper function: compute gradient color
vec3 ComputeGradient(vec2 p, vec2 start, vec2 end, float invMaxDist_val) {
    float distToSegment = pointToSegmentDist(p, start, end);
    // Use the passed-in invMaxDist_val
    float gradientFactor = clamp(distToSegment * invMaxDist_val, 0.0, 1.0);
    return mix(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 1.0), gradientFactor);
}

void main() {
    // gl_FragCoord.xy provides the current pixel's coordinate in screen space
    vec2 currentPixelPos = gl_FragCoord.xy;

    // Compute the gradient using the interpolated segment data and current pixel position
    tailTexture_out = vec4(ComputeGradient(
        currentPixelPos,
        fs_in.segStart_pixels,
        fs_in.segEnd_pixels,
        fs_in.invMaxDist
    ), 1.0); // Ensure alpha is 1.0 if you want it opaque, or use 0.0 for transparent background.
             // Given your `rlClearColor(0,0,0,0)` you likely want the background to be transparent.
             // The gradient itself should probably have an alpha channel from 0 to 1 based on its factor.
             // If you want a solid color, keep 1.0. If you want a fade, use gradientFactor for alpha.
             // e.g., tailTexture_out = vec4(ComputedColor, 1.0 - gradientFactor); or similar.
}