#version 460 core

out vec2 vUV;

void main()
{
    // Triangle pattern for fullscreen rendering
    // (-1, -1)
    // ( 3, -1)
    // (-1,  3)

    vec2 position;

    if (gl_VertexID == 0)
        position = vec2(-1.0, -1.0);
    else if (gl_VertexID == 1)
        position = vec2( 3.0, -1.0);
    else
        position = vec2(-1.0,  3.0);

    vUV = position * 0.5 + 0.5;

//    gl_Position = vec4(position, 0.0, 1.0);
    gl_Position = vec4(position, 1.0, 1.0);
}