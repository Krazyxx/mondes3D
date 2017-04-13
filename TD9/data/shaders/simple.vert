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
  vec3 AB = B - A;
  vec3 randAB = vec3(AB.xy, AB.z * 10 + 3.7);
  vec3 normal = normalize(cross(AB, randAB));
  vec3 binormal = normalize(cross(AB, normal));

  float angle = 2 * M_PI * uv.x;
  float x = r * cos(angle);
  float y = r * sin(angle);

  return y * normal + x * binormal + A + uv.y * AB;
}


vec3 circle(vec3 center, vec3 A, vec3 B, float angle, float radius, vec3 normal)
{
  vec3 AB = B - A;
  vec3 binormal = normalize(cross(AB, normal));

  float x = radius * cos(angle);
  float y = radius * sin(angle);

  return y * normal + x * binormal + center + AB;
}

vec3 bezier(float u, mat4x3 B, out mat3 F)
{
  mat4 m;
  m[0] = vec4(1,0,0,0);
  m[1] = vec4(-3,3,0,0);
  m[2] = vec4(3,-6,3,0);
  m[3] = vec4(-1,3,-3,1);
  
  vec4 t = vec4(1, u, u*u, u*u*u);
  vec4 tder1 = vec4(0, 1, 2*u, 3*u*u);
  vec4 tder2 = vec4(0, 0, 2, 6*u);

  vec3 Ta = normalize(B * m * tder1);
  vec3 Bi = normalize(cross(Ta, normalize(B * m * tder2)));
  vec3 No = normalize(cross(Ta, Bi));

  F = mat3(Ta, Bi, No);
  
  vec3 res = B * m * t;
  return res;
}

vec3 cylBezierYZ(vec2 uv, mat4x3 B, float r)
{
  mat3 F;
  vec3 center = bezier(uv.y, B, F);
  
  float angle = 2 * M_PI * uv.x;
  float x = r * cos(angle + uv.y * 20 * M_PI);
  float y = r * sin(angle + uv.y * 20 * M_PI);

  return F * vec3(0, x, y) + center;
}

/* === BEZIER 1 === 
vec3 bezier(float u, mat4x3 B)
{
  vec4 t = vec4(1, u, u*u, u*u*u);
  
  mat4 m;
  m[0] = vec4(1,0,0,0);
  m[1] = vec4(-3,3,0,0);
  m[2] = vec4(3,-6,3,0);
  m[3] = vec4(-1,3,-3,1);
  
  vec3 res = B * m * t;
  return res;
}


vec3 cylBezierYZ(vec2 uv, mat4x3 B, float r)
{
  vec3 center = bezier(uv.y, B);
  
  float angle = 2 * M_PI * uv.x;
  float x = r * cos(angle);
  float y = r * sin(angle);

  return vec3(x, y, 0) + center;
}
*/

void main()
{
  /* cylinder /
  vec3 A = vec3(0,1,5);
  vec3 B = vec3(2,2,0);
  float r = 5;
  vec3 position = cylinder(vtx_texcoord, A, B, r);
  /**/
  
  /* Bezier */
  mat4x3 B;
  B[0] = vec3(-1,0, 2);
  B[1] = vec3(-0.3, 0, 4);
  B[2] = vec3(0.3, 0, 1);
  B[3] = vec3(1, 0, -0.5);
  
  vec3 position = cylBezierYZ(vtx_texcoord, B, 0.1);
  /**/
  
  v_uv = vtx_texcoord;
  v_normal = normalize(normal_mat * vtx_normal);
  vec4 p = view_mat * (obj_mat * vec4(position, 1.));
  v_view = normalize(-p.xyz);
  gl_Position = proj_mat * p;
}
