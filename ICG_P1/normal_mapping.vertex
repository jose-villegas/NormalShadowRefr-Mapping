#define MAX_LIGHTS 8
#define NUM_LIGHTS 2

varying vec3 lightDir[MAX_LIGHTS];
varying vec3 halfVector[MAX_LIGHTS];
varying float att[MAX_LIGHTS];

void main()
{
	int i;

	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;
	
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz);
	vec3 b = cross(n, t) * gl_MultiTexCoord1.w;
		
	mat3 tbnMatrix = mat3(t.x, b.x, n.x,
						  t.y, b.y, n.y,
						  t.z, b.z, n.z);

	for(i = 0; i < NUM_LIGHTS; i++)
	{
		lightDir[i] = gl_LightSource[i].position.xyz - vVertex;
		lightDir[i] = tbnMatrix * lightDir[i];

		

		halfVector[i] = gl_LightSource[i].halfVector.xyz - vVertex;
		halfVector[i] = tbnMatrix * halfVector[i];

		float d = length(lightDir[i]);
		att[i] = 1.0 / ( gl_LightSource[i].constantAttenuation + (gl_LightSource[i].linearAttenuation*d) + (gl_LightSource[i].quadraticAttenuation * d * d) );

		// Directional Light
		if(gl_LightSource[i].position.w == 0)
		{
			lightDir[i] = tbnMatrix * gl_LightSource[i].position.xyz;
			att[i] = 1.0;
		}
	}
}