#version 330

in vec2 fragPos;

out vec4 finalColor;

// Uniforms you already set from C++
uniform float trailThickness;   // your line thickness
uniform vec2 lineStart;         // line start
uniform vec2 lineEnd;           // line end

void main()
{
    // Vector from start to end
    vec2 lineDir = lineEnd - lineStart;
    float lineLen = length(lineDir);
    vec2 lineNorm = normalize(lineDir);

    // Vector from start to current fragment
    vec2 toFrag = fragPos - lineStart;

    // Projection of frag onto line (distance along line)
    float proj = dot(toFrag, lineNorm);

    // Clamp inside line segment
    //if (proj < 0.0 || proj > lineLen)
    //    discard;

    // Perpendicular distance to line
    float dist = abs(toFrag.x * lineNorm.y - toFrag.y * lineNorm.x);

    // If outside thickness, discard
    //if (dist > trailThickness)
    //    discard;

    // Gradient factor: 0 at center, 1 at edge
    float t = dist / trailThickness;

    // White in the middle → Blue at the edges
    vec3 centerColor = vec3(1.0, 1.0, 1.0);   // white
    vec3 edgeColor   = vec3(0.0, 0.0, 1.0);   // blue
    vec3 mixedColor  = mix(centerColor, edgeColor, t);

    finalColor = vec4(mixedColor, 1.0);
}