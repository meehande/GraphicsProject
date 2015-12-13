#version 330

in vec3 vFragPosition;
in vec2 vFragTextureCoords;
in vec3 vFragNormal;
in float fDistance;

out vec4 FragColor;

uniform sampler2D Texture1;

uniform vec3 vSkyColour;
uniform vec3 vLightColour;
uniform vec3 vLightDirection;
uniform vec3 vViewPosition;

const float fShineDamper = 16.0;
const float fReflectivity = 0.3;
const float fDensity = 0.001;
const float fGradient = 0.9;
const float fTiling = 32.0;
const float fAmbient = 0.3;

void main()
{	
	vec4 vTextureColour = texture(Texture1, (vFragTextureCoords * fTiling));
	vec3 vToViewVector = normalize(vViewPosition - vFragPosition);

	vec3 uLightDirection = normalize(-vLightDirection);
	float fBrightness = max( dot(vFragNormal, uLightDirection), fAmbient);
	vec3 vReflectedLight = reflect(-uLightDirection, vFragNormal);
	float fSpecularFactor = max( dot( vReflectedLight, vToViewVector ), fAmbient);

	vec4 vDiffuseLight = vec4(fBrightness * vLightColour, 1.0);
	vec4 vSpecularLight = vec4(pow(fSpecularFactor, fShineDamper) * fReflectivity * vLightColour, 0.0);	

	FragColor = (vDiffuseLight) * vTextureColour + vSpecularLight;

	float fVisibility = exp(-pow( ( fDistance * fDensity) , fGradient) );
	fVisibility = clamp(fVisibility, 0.0, 1.0);
	FragColor = mix(vec4(vSkyColour, 1.0), FragColor, fVisibility);
}