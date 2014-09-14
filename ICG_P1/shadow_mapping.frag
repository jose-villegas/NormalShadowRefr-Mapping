uniform sampler2D shadowMap;

varying vec4 ShadowCoord;
varying vec3 lightDir;
varying vec3 normal;

void main()
{	
	vec3 n = normalize(normal);
	vec3 l = normalize(lightDir);
	float cosTheta = clamp( dot( n,l ), 0,1 );
	float bias = 0.005*tan(acos(cosTheta));
	bias = clamp(bias, 0,0.01);

	vec4 color = vec4(0, 0, 0, 1);
	
	float visibility = 1.0;
		
	if ( texture( shadowMap, ShadowCoord.st ).z  <  ShadowCoord.z - bias)
	{
		visibility = 0.5;
	}

	color += gl_FrontMaterial.ambient;
	color += gl_FrontMaterial.diffuse;
	color += gl_FrontMaterial.specular;
		
	gl_FragColor =	color * visibility;
	  
}

