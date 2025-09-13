
#version 330

in vec2 fragPos;

out vec4 finalColor;

uniform vec2 circleCenter;
uniform float trailThickness;

void main()
{
    // Distance from fragment to circle center
    float dist = length(fragPos - circleCenter);

    // Outside the circle? Discard
    //if (dist > trailThickness)
    //    discard;

    // Gradient factor: 0 at center, 1 at edge
    float t = dist / trailThickness;

    // White -> Blue
    vec3 centerColor = vec3(1.0, 1.0, 1.0);
    vec3 edgeColor   = vec3(0.0, 0.0, 1.0);
    vec3 mixedColor  = mix(centerColor, edgeColor, t);

    finalColor = vec4(mixedColor, 1.0);
}