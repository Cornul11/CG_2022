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
out vec2 textureCoordinates;

void main()
{
    gl_Position = projectionTransform * modelTransform * vec4(vertCoordinates_in, 1.0);
    textureCoordinates = textureCoordinates_in;

    vec3 N = normalize(normalTransform * vertNormal_in);
    vec3 L = normalize(lightCoordinates - vertCoordinates_in);
    vec3 R = reflect(-L, vertNormal_in);
    vec3 V = normalize(-vertCoordinates_in);

    float Ia = 1.0;
    float Id = 1.0;
    float Is = 1.0;

    float ka = material.x;
    float kd = material.y;
    float ks = material.z;

    float p = exponent;

    float IA = Ia * ka;// I_A = I_a * k_a
    float ID = Id * kd * max(0, dot(N, L));// I_D = I_d * k_d * max(0, N * L)
    float IS = Is * ks * pow(max(0, dot(R, V)), p);// I_S = I_s * k_s * max(0, R * V) ^ p

    vertColor = vec3(1, 1, 1) * (IA + ID + IS);
}