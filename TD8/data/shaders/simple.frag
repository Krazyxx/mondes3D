#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec3 v_view;
in vec2 v_tex;

uniform vec3 lightDir;
uniform sampler2D tex2D_0; // normal
uniform sampler2D tex2D_1; // diffuse
uniform sampler2D tex2D_2;

out vec4 out_color;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  vec3 res = vec3(0,0,0);
  float dc = max(0,dot(n,l));
  if (dc > 0) {
    res = dCol * dc;
  }
  return res;
}

void main(void) {
  float ambient = 0.5;
  float shininess = 50;
  vec3 spec_color = vec3(1,1,1);

  vec3 normal = (texture(tex2D_0, v_tex).rgb - 0.5) * 2;
  vec3 diffuse = vec3(1,1,1);
  //vec4 diffuse = texture(tex2D_1, v_tex);

  //diffuse = mix(vec4(0, 0, 0, 0), diffuse, max(dot(normalize(v_normal), lightDir), 0));
  mat3 tbnvMatrix = transpose(mat3(v_tangent, v_bitangent, v_normal));
  
  /* == Earth  == /
  vec3 earth_color = texture(tex2D_0, v_tex).rgb;
  vec3 clouds_color = texture(tex2D_1, v_tex).rgb;
  vec3 night_color = texture(tex2D_2, v_tex).rgb;
  vec3 color = mix(earth_color, clouds_color, clouds_color.r);
  color = mix(night_color, color, max(dot(normalize(v_normal), lightDir), 0));
  /**/
  
  //out_color = vec4(ambient * diffuse.rgb + blinn(normalize(normal), tbnvMatrix * normalize(v_view), tbnvMatrix * normalize(lightDir), diffuse.rgb, spec_color, shininess), 1.0);
  out_color = vec4(v_normal, 1);
}
