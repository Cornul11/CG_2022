#version 330 core

#define M_PI 3.141593

in vec3 vertNormal;

out vec4 fNormal;
vec3 map(vec3 value, int inMin, int inMax, int outMin, int outMax) {
    return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}
void main()
{
    vec3 vertNormalMapped = map(vertNormal, -1, 1, 0, 1);
    fNormal = vec4(vertNormalMapped, 1.0);
}
