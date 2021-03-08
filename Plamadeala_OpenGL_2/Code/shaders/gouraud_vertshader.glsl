#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoordinates_in;

// Specify the Uniforms of the vertex shader
// uniform mat4 modelTransform; for example
uniform mat4 modelTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;
uniform vec3 lightCoordinates;
uniform vec3 material;
uniform int exponent;

// Specify the output of the vertex stage
out vec3 vertColor;
out vec3 vertNormal;
out vec2 textureCoordinates;

void main()
{
    gl_Position = projectionTransform * modelTransform * vec4(vertCoordinates_in, 1.0);

    vec3 lightDistance = normalize(lightCoordinates - vertCoordinates_in);
    vec3 normalizedVertex = normalize(-vertCoordinates_in);
    vec3 reflected = reflect(-lightDistance, vertNormal_in);
    float angle = max(dot(reflected, normalizedVertex), 0.05);
    float specular = pow(angle, exponent);

    vec3 normalTransVert = normalize(normalTransform * vertNormal_in);
    float diffuse = max(dot(normalTransVert, lightDistance), 0.05);

    vec3 unit = vec3(1, 1, 1);
    vertColor = material.x + unit * material.y * diffuse + material.z * specular;

    textureCoordinates = textureCoordinates_in;
//    float Ia, Id, Is;
//    vec3 lightAndMatAmb = vec3(0.2, 0.2, 0.2);
//    vec3 lightSpec = vec3(0.4, 0.4, 0.4);
//    vec3 lightDiff = vec3(0.3, 0.3, 0.3);
//    vec3 materialDiff = vec3(0.5, 0.5, 0.5);
//    Ia = dot(lightAndMatAmb, lightAndMatAmb);
//    Id = dot(lightDiff, lightSpec) * max(0, dot(vertNormal_in, lightMatrix));
//    Is = dot(lightSpec, lightAndMatAmb) * pow(max(0, dot(vertNormal_in, vec3(0, 0, -1))), exponent);
//
//    vertColor = materialDiff * (Ia + Id + Is);
//    textureCoordinates = textureCoordinates_in;
}
