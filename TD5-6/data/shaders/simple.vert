#version 330 core

in vec3 vtx_position;
in vec3 vtx_color;

uniform mat4 transformation;
uniform mat4 view_mat;
uniform mat4 proj_mat;
out vec3 var_color;

void main()
{
  var_color = vtx_position;
  gl_Position = proj_mat * view_mat * transformation * vec4(vtx_position, 1);
}
