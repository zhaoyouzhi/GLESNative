#version 300 es

uniform mat4 u_mvpMatrix;
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_TexCoordinate;
out vec2 v_TexCoordinate;

void main() {
    gl_Position = u_mvpMatrix * a_position;
//    gl_Position = a_position;
     v_TexCoordinate = a_TexCoordinate;
}
