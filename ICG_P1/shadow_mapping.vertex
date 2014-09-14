// Used for shadow lookup
varying vec4 ShadowCoord;
varying vec3 lightDir;
varying vec3 normal;

uniform mat4 DepthBiasMVP;
	
void main()
{
	vec4 vVertex = gl_ModelViewMatrix * gl_Vertex;
	lightDir = vec3(gl_LightSource[0].position.xyz - vVertex.xyz);
	normal = gl_NormalMatrix * gl_Normal;
	ShadowCoord = DepthBiasMVP * gl_Vertex;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

