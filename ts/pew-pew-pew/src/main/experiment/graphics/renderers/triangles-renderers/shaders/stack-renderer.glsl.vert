
#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;
// uniform mat4 u_viewMatrix;
// uniform mat4 u_modelMatrix;
// uniform mat3 u_modelMatrix;

in vec3 a_vertex_position;
in vec4 a_vertex_color;
in vec3 a_vertex_normal;

out vec4 v_color;
out vec3 v_worldSpacePosition;
out vec3 v_worldSpaceNormal;

void main(void)
{
  v_color = a_vertex_color;
  v_worldSpacePosition = a_vertex_position;
  v_worldSpaceNormal = a_vertex_normal;

  gl_Position = u_composedMatrix * vec4(a_vertex_position, 1.0);
}
