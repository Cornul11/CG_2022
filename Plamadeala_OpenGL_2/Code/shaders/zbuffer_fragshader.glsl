#version 330 core

out vec4 fColor;

float near = 0.1;
float far  = 100.0;

// implementation imported from
// source: https://learnopengl.com/Advanced-OpenGL/Depth-testing

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = linearizeDepth(gl_FragCoord.z) / far;
    fColor = vec4(vec3(depth), 1.0);
}