#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 v_tex;

uniform vec3 lightDir;
uniform sampler2D tex2D_0;
uniform sampler2D tex2D_1;
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
  float ambient = 1;
  float shininess = 50;
  vec3 spec_color = vec3(1,1,1);

  //vec3 color =  texture(tex2D_0, v_tex).rgb;
  /* == Earth  == */
  vec3 earth_color = texture(tex2D_0, v_tex).rgb;
  vec3 clouds_color = texture(tex2D_1, v_tex).rgb;
  vec3 night_color = texture(tex2D_2, v_tex).rgb;
  vec3 color = mix(earth_color, clouds_color, clouds_color.r);
  color = mix(night_color, color, max(dot(normalize(v_normal), lightDir), 0));
  /**/
  
  out_color = vec4(ambient * color + blinn(normalize(v_normal),normalize(v_view), lightDir, color, spec_color, shininess),1.0);
  
}
