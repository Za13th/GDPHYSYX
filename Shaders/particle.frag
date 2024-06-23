#version 330 core 

uniform vec3 color;

in vec3 fragPos;
out vec4 FragColor;

void main()
{
  FragColor = vec4(color,1.0);
}