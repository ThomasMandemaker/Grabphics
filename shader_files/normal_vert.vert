#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec3 rotation;

void main()
{
    gl_Position = vec4(aPos.x * rotation.x, aPos.y, aPos.z * rotation.z, 1.0);
}
