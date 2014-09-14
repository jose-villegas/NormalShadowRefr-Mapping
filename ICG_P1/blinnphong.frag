#define MAX_LIGHTS 8
#define NUM_LIGHTS 2

uniform sampler2D colorMap;
uniform float materialAlpha;

varying vec3 normal;
varying vec3 eyeVec;
varying vec3 lightDir[MAX_LIGHTS];
varying float att[MAX_LIGHTS];

void main()
{   
	int i;
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 N = normalize(normal);
	
	for(i = 0; i < NUM_LIGHTS; i++)
	{
		float spot = 1.0;
		vec3 L = normalize(lightDir[i]);

		// Spot Light
		if(gl_LightSource[i].spotCutoff <= 90.0)
		{
			vec3 D = normalize(gl_LightSource[i].spotDirection);

			float cosAngle = dot(-L, D);
			
			if(cosAngle > gl_LightSource[i].spotCosCutoff)
			{
				spot = pow(gl_LightSource[i].spotCosCutoff, gl_LightSource[i].spotExponent);
			}
			else
			{
				spot = 0.0;
			}
		}
	
		vec3 halfDir = normalize(L + eyeVec);
		float nDotL = max(0.0, dot(N, L));
		float nDotH = max(0.0, dot(normal, halfDir));
		float power = (nDotL == 0.0) ? 0.0 : pow(nDotH, gl_FrontMaterial.shininess);

		vec4 ambient = gl_FrontLightProduct[i].ambient;
		vec4 diffuse = gl_FrontLightProduct[i].diffuse * nDotL;
		vec4 specular = gl_FrontLightProduct[i].specular * power ;
		color += ( ambient + diffuse + specular ) * att[i] * spot;
	}
	gl_FragColor = color * texture2D(colorMap, gl_TexCoord[0].st);
	gl_FragColor.a = materialAlpha;
}