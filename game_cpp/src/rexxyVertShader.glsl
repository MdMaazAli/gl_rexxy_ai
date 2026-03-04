#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertColor;
layout(location = 3) in vec3 aNormal;

out vec3 ourColor;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection*view*model*vec4(aPos,1.0f);
    ourColor = vertColor;
    FragPos = vec3(view*model*vec4(aPos,1.0f));
    Normal = mat3(transpose(inverse(view*model)))*aNormal;
}