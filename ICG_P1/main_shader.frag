#define MAX_LIGHTS 8
#define NUM_LIGHTS 2

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform float materialAlpha;

varying vec3 normal;
varying vec3 eyeVec;

varying vec3 lightDir[MAX_LIGHTS];
varying vec3 lightSpotDir[MAX_LIGHTS];
varying vec3 halfVector[MAX_LIGHTS];
varying float att[MAX_LIGHTS];

uniform int bEnableBumpMapping = 0;
uniform int bEnableShadowMapping = 0;
uniform int bEnableReflection = 0;
uniform int bEnableRefraction = 0;

// Shadow Mapping
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
varying vec4 ShadowCoord[NUM_LIGHTS];

// Cube Mapping Environment Reflection
uniform samplerCube CubeMap;
varying vec3 Refl;

void main()
{
	int i;
	float visibility = 1.0;
	vec4 color = vec4(vec3(0.1), 1);
	vec3 n = normalize(texture(normalMap, gl_TexCoord[0].st).rgb * 2.0 - 1.0);
	float cosTheta, cosAlpha;

	vec3 E = normalize(eyeVec);

	if(bEnableBumpMapping == 0)  { n = normalize(normal); }

	if(bEnableShadowMapping == 1)
	{
		float bias = 0.005;
		vec4 black = vec4(vec3(0), 1.0);

		// If the light is disable then don't map shadows
		if(!(gl_LightSource[0].ambient == black && 
		   gl_LightSource[0].specular == black && 
		   gl_LightSource[0].diffuse == black))
		{
			if ( texture( shadowMap0, ShadowCoord[0].st ).z  <  ShadowCoord[0].z - bias)
			{
				visibility *= 0.5;
			}
		}	
		
		// If the light is disable then don't map shadows
		if(!(gl_LightSource[1].ambient == black && 
		   gl_LightSource[1].specular == black && 
		   gl_LightSource[1].diffuse == black))
		{
			if ( texture( shadowMap1, ShadowCoord[1].st ).z  <  ShadowCoord[1].z - bias)
			{
				visibility *= 0.5;
			}
		}
		

	} 
	else
	{
		visibility = 1.0;
	}

	for(i = 0; i < NUM_LIGHTS; i++)
	{
		float spot = 1.0;
		vec3 l = normalize(lightDir[i]);
		vec3 h = normalize(halfVector[i]);

		// Spot Light
		if(gl_LightSource[i].spotCutoff <= 90.0)
		{
			vec3 spotDir = vec3(gl_LightSource[i].spotDirection);

			if(bEnableBumpMapping == 1) { spotDir = lightSpotDir[i]; }

			vec3 D = normalize(spotDir);
			float cosAngle = dot(-l, D);
			
			if(cosAngle > gl_LightSource[i].spotCosCutoff)
			{
				spot = pow(gl_LightSource[i].spotCosCutoff, gl_LightSource[i].spotExponent);
			}
			else
			{
				spot = 0.0;
			}
		}
		vec3 R = reflect(-l,n);
		
		float nDotL = max(0.0, dot(n, l));
		float nDotH;

		cosAlpha = clamp( dot( E,R ), 0,1 );
		cosTheta = clamp( dot( n,l ), 0,1 );

		if(bEnableBumpMapping == 0)
		{
			vec3 halfDir = normalize(l + eyeVec);
			nDotH = max(0.0, dot(normal, halfDir));
		}
		else { nDotH = max(0.0, dot(n, h)); }

		float power = (nDotL == 0.0) ? 0.0 : pow(nDotH, gl_FrontMaterial.shininess);
		vec4 ambient = gl_FrontLightProduct[i].ambient;
		vec4 diffuse = gl_FrontLightProduct[i].diffuse * nDotL * visibility * cosTheta;
		vec4 specular = gl_FrontLightProduct[i].specular * power * visibility * pow(cosAlpha,5);
		color += ( ambient + diffuse + specular ) * att[i] * spot;
	}

	gl_FragColor = color * texture(colorMap, gl_TexCoord[0].st);
	gl_FragColor.a = materialAlpha;

	if(bEnableReflection == 1 || bEnableRefraction == 1) 
	{ 
		gl_FragColor += texture(CubeMap, Refl.xyz);
	}
}