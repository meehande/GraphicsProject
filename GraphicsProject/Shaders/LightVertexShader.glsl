#version 330
in vec3 vPosition;
in vec2 vTextureCoords;
in vec3 vNormal;

out vec3 vFragPosition;
out vec2 vFragTextureCoords;
out vec3 vFragNormal;
out float fDistance;

uniform mat4 mModelMatrix;
uniform mat4 mProjectionMatrix;
uniform mat4 mViewMatrix;
uniform vec3 vLightPosition;

void main()
{
	vec4 vWorldPosition = mModelMatrix * vec4(vPosition.xyz, 1.0);
	vec4 vWorldNormal = mModelMatrix * vec4(vNormal.xyz, 0.0);
	vec4 vRelativePosition = mViewMatrix * vWorldPosition;
	
	gl_Position = mProjectionMatrix * vRelativePosition;		
	
	fDistance = length(vRelativePosition.xyz);
	
	vFragTextureCoords = vTextureCoords;
	vFragNormal = vWorldNormal.xyz;
	vFragPosition = vWorldPosition.xyz;
}