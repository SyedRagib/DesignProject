#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
in vec3 position;

in vec3 transform;

void main() {
	gl_Position = P * V * M * vec4(position + transform, 1.0);
}
