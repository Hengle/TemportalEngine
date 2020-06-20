#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ModelViewProjection {
	mat4 view;
	mat4 proj;
} mvp;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in mat4 modelMatrix; // slots [2,5]

layout(location = 0) out vec3 fragColor;

void main()
{
	gl_Position = mvp.proj * mvp.view * modelMatrix * vec4(position, 1.0);
	fragColor = color;
}
