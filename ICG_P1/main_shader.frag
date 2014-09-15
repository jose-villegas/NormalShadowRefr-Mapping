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

// Shadow Mapping
uniform sampler2D shadowMap;
varying vec4 ShadowCoord;

void main()
{
	int i;
	float visibility = 1.0;
	vec4 color = vec4(vec3(0.1), 1);
	vec3 n = normalize(texture2D(normalMap, gl_TexCoord[0].st).rgb * 2.0 - 1.0);
	
	if(bEnableBumpMapping == 0)  { n = normalize(normal); }

	if(bEnableShadowMapping == 1)
	{
		float bias = 0.005;

		if ( texture( shadowMap, ShadowCoord.st ).z  <  ShadowCoord.z - bias)
		{
			visibility = 0.5;
		}
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

		float nDotL = max(0.0, dot(n, l));
		float nDotH;

		if(bEnableBumpMapping == 0)
		{
			vec3 halfDir = normalize(l + eyeVec);
			nDotH = max(0.0, dot(normal, halfDir));
		}
		else { nDotH = max(0.0, dot(n, h)); }

		float power = (nDotL == 0.0) ? 0.0 : pow(nDotH, gl_FrontMaterial.shininess);
		vec4 ambient = gl_FrontLightProduct[i].ambient;
		vec4 diffuse = gl_FrontLightProduct[i].diffuse * nDotL;
		vec4 specular = gl_FrontLightProduct[i].specular * power;
		color += ( ambient + diffuse + specular ) * att[i] * spot;
	}

	gl_FragColor = color * visibility * texture2D(colorMap, gl_TexCoord[0].st);
	gl_FragColor.a = materialAlpha;
}