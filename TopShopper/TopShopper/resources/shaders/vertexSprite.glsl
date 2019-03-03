
#version 410

layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec2 VertexUV;


out vec2 uv;

void main()
{

    gl_Position = vec4(VertexPosition.x, VertexPosition.y, 0.0, 1.0);

    uv = VertexUV;

}
