varying vec3 varNormal, varEyeDir;
#define MAX_LIGHTS 8
#define NUM_LIGHTS 4
varying vec3 lightDirection[MAX_LIGHTS];
varying float att[MAX_LIGHTS];

void main()
{
	gl_Position = ftransform();
	varNormal = gl_NormalMatrix * gl_Normal;
	vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;
	varEyeDir = -vVertex.xyz;
	int i;
	float d;
	for (i=0; i<NUM_LIGHTS; ++i){
		lightDirection[i] = vec3(gl_LightSource[i].position.xyz - vVertex.xyz);
		d = length(lightDirection[i]);
		att[i] = 1.0 / ( gl_LightSource[i].constantAttenuation + (gl_LightSource[i].linearAttenuation*d) + (gl_LightSource[i].quadraticAttenuation*d*d) );
	}
}