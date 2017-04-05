#version 330 core

#define M_PI 3.14159265358979323846

uniform mat4 obj_mat;
uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat3 normal_mat;

in vec3 vtx_position;
in vec3 vtx_normal;
in vec2 vtx_texcoord;

out vec3 v_normal;
out vec3 v_view;
out vec2 v_uv;

vec3 cylinder(vec2 uv, vec3 A, vec3 B, float r)
{
  float angle = 2 * M_PI * uv.x;
  float x = r * cos(angle);
  float y = r * sin(angle);
  
  float dist = length(B-A);
  return vec3(x, y, uv.y * dist);
}

vec3 bezier(float u, Matrix<int, 3, 4> B)
{
  vec4 vec_u = vec4(1, u, u * u, u * u * u);
  
  mat4 m = mat4(vec4(1,0,0,0),
		vec4(-3,3,0,0),
		vec4(3,-6,3,0),
		vec4(-1,3,-3,1));
  
  vec4 res = B * m * vec_u;
  return vec3(res);
}

vec3 cylBezierYZ(vec2 uv, vec3 B[4], float r)
{
  return vec3(0,0,0);
}

void main()
{
  // cylinder
  //vec3 A = vec3(8,5,5);
  //vec3 B = vec3(5,0,0);
  //float r = 1;
  //vec3 position = cylinder(vtx_texcoord, A, B, r);
  

  // Bezier

  
  Matrix<int, 3, 4> B;
   
  B << vec3(  -1, 0,    2)
    << vec3(-0.3, 0,    4)
    << vec3( 0.1, 0,    1)
    << vec3(   1, 0, -0.5);
  
  vec3 position = bezier(2.3, B);
  
  v_uv = vtx_texcoord;
  v_normal = normalize(normal_mat * vtx_normal);
  vec4 p = view_mat * (obj_mat * vec4(position, 1.));
  v_view = normalize(-p.xyz);
  gl_Position = proj_mat * p;
}
