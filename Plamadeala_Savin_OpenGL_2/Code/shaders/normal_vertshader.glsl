#version 330 core

#define M_PI 3.141593

layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;

uniform mat4 modelTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

out vec3 vertNormal;

void main()
{
    gl_Position = projectionTransform * modelTransform * vec4(vertCoordinates_in, 1.0);
    vertNormal = normalize(normalTransform * vertNormal_in);
}
