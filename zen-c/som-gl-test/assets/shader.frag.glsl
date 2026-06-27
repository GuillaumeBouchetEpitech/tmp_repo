#version 300 es

precision lowp float;

uniform lowp sampler2D u_tex;

in vec2 v_uv;

out vec4 o_fragColor;

void main()
{
  o_fragColor = texture(u_tex, v_uv).rgba;
}
