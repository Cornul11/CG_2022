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
    vec3 lightDistance = normalize(lightCoordinates - vertCoordinates);
    vec3 normalizedVertex = normalize(-vertCoordinates);
    vec3 reflected = reflect(-lightDistance, vertNormal);
    float angle = max(dot(reflected, normalizedVertex), 0.05);
    float specular = pow(angle, exponent);

    float diffuse = max(dot(vertNormal, lightDistance), 0.05);

    vec3 unit = vec3(1, 1, 1);
    fColor = vec4(material.x + unit * material.y * diffuse + material.z * specular, 1.0) * texture(textureColor, textureCoordinates);
}