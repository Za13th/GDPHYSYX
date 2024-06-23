#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

out vec3 fragPos;

void main(){
    gl_Position = projection * view * transform * vec4(aPos, 1.0);
    fragPos = vec3(transform * vec4(aPos, 1.0));
}

