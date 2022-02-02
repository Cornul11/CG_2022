#version 330 core

#define M_PI 3.141593

in vec3 vertNormal;

out vec4 fNormal;

// function for mapping a range onto another
// found here: https://stackoverflow.com/a/5735770/3826046
vec3 map(vec3 vec, int inMin, int inMax, int outMin, int outMax)
{
    return (vec - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void main()
{
    vec3 mappedVertNormal = map(vertNormal, -1, 1, 0, 1);
    fNormal = vec4(mappedVertNormal, 1.0);
}
