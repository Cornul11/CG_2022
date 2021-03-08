#version 330 core

// Define constants
#define M_PI 3.141593


// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertCoordinates;
in vec3 vertNormal;
in vec2 textureCoordinates;


// Specify the Uniforms of the fragment shaders
uniform vec3 lightCoordinates;
uniform vec3 material;
uniform sampler2D textureColor;
uniform int exponent;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    vec3 N = vertNormal;
    vec3 L = normalize(lightCoordinates - vertCoordinates);
    vec3 R = reflect(-L, N);
    vec3 V = normalize(-vertCoordinates);

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

    vec3 rgb = vec3(1, 1, 1) * (IA + ID + IS);
    fColor = vec4(rgb, 1.0) * texture(textureColor, textureCoordinates);
}