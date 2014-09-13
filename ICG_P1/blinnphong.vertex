#define MAX_LIGHTS 8
#define NUM_LIGHTS 2

varying vec3 normal;
varying vec3 eyeVec;
varying vec3 lightDir[MAX_LIGHTS];
varying float att[MAX_LIGHTS];

void main()
{
	int i; 

	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;	
	vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;
	eyeVec = -vVertex.xyz;

	normal = normalize(gl_NormalMatrix * gl_Normal);


	for(i = 0; i < NUM_LIGHTS; i++)
	{
		// Light Direction
		lightDir[i] = vec3(gl_LightSource[i].position.xyz - vVertex);
		
		// Calculate Attenuation
		float d = length(lightDir[i]);
		att[i] = 1.0 / ( gl_LightSource[i].constantAttenuation + (gl_LightSource[i].linearAttenuation*d) + (gl_LightSource[i].quadraticAttenuation * d * d) );	

		// Directional Light
		if(gl_LightSource[i].position.w == 0)
		{
			lightDir[i] = gl_LightSource[i].position.xyz;
			att[i] = 1.0;
		}
	}	 
}
