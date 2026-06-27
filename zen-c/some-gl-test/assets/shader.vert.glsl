#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3 a_position;
in vec2 a_texcoord;

out vec2 v_uv;

void main()
{
  gl_Position = u_composedMatrix * vec4(a_position, 1);
  // gl_Position = vec4(a_position, 1);
  v_uv = a_texcoord;
}
