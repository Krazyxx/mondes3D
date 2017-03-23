#version 330 core

uniform mat4 obj_mat;
uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat3 normal_mat;

in vec2 vtx_tex;

in vec3 vtx_position;
in vec3 vtx_color;
in vec3 vtx_normal;

out vec3 v_normal;
out vec3 v_view;
out vec3 v_color;
out vec2 v_tex;

void main()
{
  v_color = vec3(vtx_tex, 0);
  v_tex = vtx_tex;
  
  v_normal = normalize(normal_mat * vtx_normal);
  vec4 p = view_mat * (obj_mat * vec4(vtx_position, 1.));
  v_view = normalize(-p.xyz);
  gl_Position = proj_mat * p;
  
}
