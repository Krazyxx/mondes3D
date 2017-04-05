#version 330 core

in vec3 v_normal;
in vec3 v_view;
in vec3 v_col;

uniform vec3 lightDir;
uniform int wireframe;

out vec4 out_color;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
	vec3 halfD = (v +l)/2;
	float spec = pow(max(dot(n, halfD), 0), s);
	return max(dot(n, l), 0.0) * (dCol + sCol * spec);

}

void main(void) {
  float ambient = 0.4;
  float shininess = 50;
  vec3 spec_color = vec3(0,0,1);

  vec3 blinnColor = blinn(normalize(v_normal), normalize(v_view), lightDir, vec3(0, 0, 1), spec_color, shininess);

  if(wireframe==1)
    out_color = vec4(1,1,1,1);
  else
    out_color = vec4(ambient * vec3(0, 0, 1) + blinnColor, 1.0);
}
