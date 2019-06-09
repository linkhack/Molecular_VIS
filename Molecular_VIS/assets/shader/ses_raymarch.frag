#version 430 core

#define EPSILON 0.001
#define EPSILON_GRAD 0.000001
#define BLACK vec4(0.0f,0.0f,0.0f,1.0f);
#define WHITE vec4(1.0f,1.0f,1.0f,1.0f);
#define MAX_STEPS 256
#define MAX_DISTANCE 250.0f

#define _DIRECTIONAL_LIGHTS_COUNT 1
#define _POINT_LIGHTS_COUNT 1
#define _SPOT_LIGHT_COUNT 1

// Light properties structs
struct PointLight {
	vec3 color;
	vec3 position;
	vec3 attenuation;
};

struct DirectionalLight {
	vec3 color;
	vec3 direction;
};

struct SpotLight {
	vec3 color;
	vec3 position;
	vec3 direction;
	float innerOpeningAngle;
	float outerOpeningAngle;
	vec3 attenuation;
};

//Ray and Hit structs
struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Hit {
	int steps;
	float distance;
	vec3 viewDirection;
	vec3 position;
	vec3 normal;
};

//Material struct
struct Material {
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specularCoefficient;
};

out vec4 FragColor;
out vec4 fcout;
out vec4 offset;

in vec2 TexCoords;
in vec3 viewDirection;
in Ray ray;
in float distance;

//Lights
uniform PointLight pointLights[_POINT_LIGHTS_COUNT];
uniform DirectionalLight directionalLights[_DIRECTIONAL_LIGHTS_COUNT];
uniform SpotLight spotLights[_SPOT_LIGHT_COUNT];
uniform int nrPointLight;
uniform int nrDirLight;
uniform int nrSpotLight;

uniform vec3 cameraPosition;

uniform sampler3D SESTexture;
uniform sampler2D AtomTexture;
uniform sampler2D AtomDepth;
uniform vec3 grid_min;
uniform vec3 grid_max;
uniform float subsurfaceDepth;
uniform float dmax;
uniform float refraction;
uniform bool reflectionOn;
uniform bool refractionOn;
vec2 texOffset;


//Matrial constant
const Material mat = {vec3(0.0f,0.0f,1.f),vec3(0.1f),vec3(0.8f),vec3(0.1f),15.0f};

vec3 scale_unit(vec3 pos)
{
	return (pos - grid_min) / (grid_max - grid_min);
}

float sceneSDF(vec3 pos)
{
	return texture(SESTexture,scale_unit(pos)).r;
}

vec3 calculateNormal(vec3 pos, float value){
	const float eps=0.3;
	const vec2 k = vec2(1,-1);

    return normalize( k.xyy * (sceneSDF( pos + k.xyy*eps ).x -value)+
                    k.yyx * (sceneSDF( pos + k.yyx*eps ).x - value)+
                    k.yxy * (sceneSDF( pos + k.yxy*eps ).x - value)+
                    k.xxx * (sceneSDF( pos + k.xxx*eps ).x - value));
}


//Calculates distance of first hit
Hit rayMarching(vec3 start, vec3 direction){
	Hit result;
	float depth = EPSILON;
	for(int i = 0; i<MAX_STEPS; ++i){
		float dist = sceneSDF(start+depth*direction);
		if(dist < EPSILON){
			result.distance = depth;
			result.position = start+depth*direction;
			result.normal = calculateNormal(result.position, dist);
			result.viewDirection = direction;
			result.steps = i;
			return result;
		}
		depth += dist;
		if(depth > MAX_DISTANCE){
			result.distance = MAX_DISTANCE + EPSILON;
			result.steps = i;
			return result;
		}
	}
	result.distance = MAX_DISTANCE + EPSILON;
	result.steps = MAX_STEPS;
	return result;
}

//Calculates Shading of Hit w.r.t all lights
vec4 calculateShading(Hit hit){
	//
	vec3 color;
	float ao = 0.0;
	const int nbIte = 4;
	const float maxDist = 16.f;
	float distance = 1.0f;
	float falloff=1.0;
    for( int i=0; i<nbIte; i++ )
    {
        
        vec3 rd = hit.normal*distance;
		distance *= 2.1f;
        ao += (distance - max(sceneSDF( hit.position + rd ),0.f))/maxDist*falloff;
		falloff*=0.95;
    }

	float light =1.0f-0.15f*ao;
	//float light = 0.5f;
	//Subsurface
	vec3 innerPosition = hit.position + subsurfaceDepth*hit.viewDirection;
	float innerSDF = sceneSDF(innerPosition);
	float scalingFactor = (subsurfaceDepth - innerSDF)/(2*subsurfaceDepth);
	//scalingFactor = 1.0f;
	color = mat.color*mat.diffuse*light + .25f*vec3(1-scalingFactor);
	return vec4(vec3(color),1.f);

}

void main() {
	texOffset = 1.0/textureSize(SESTexture,0);
	vec3 rayDirection = normalize(ray.direction);
	Hit closestHit = rayMarching(ray.origin, rayDirection);
	if(closestHit.distance > MAX_DISTANCE){
		FragColor = BLACK;
		return;
	}
	else{
		float sigma = min(((texture(AtomDepth,TexCoords).x)-(closestHit.distance/MAX_DISTANCE))/dmax,1);
		float fc = exp(-5*sigma);
		fcout = vec4(fc);
		if(reflectionOn)
		{
			Ray reflection;
			reflection.origin = closestHit.position;
			reflection.direction = rayDirection-2*(dot(rayDirection,normalize(closestHit.normal)))*normalize(closestHit.normal);
			Hit reflectionHit = rayMarching(reflection.origin, reflection.direction);
			//FragColor = calculateShading(reflectionHit)*(fc)+vec4(texture(AtomTexture,TexCoords))*(1-fc);
			if(reflectionHit.distance < MAX_DISTANCE)
			{
				FragColor = calculateShading(reflectionHit)*(1-fc)+vec4(texture(AtomTexture,TexCoords))*(fc);
			}else{
				FragColor = calculateShading(closestHit)*(1-fc)+vec4(texture(AtomTexture,TexCoords))*(fc);
			}
			return;
		}
		if(refractionOn)
		{
			vec3 R = normalize(refract(closestHit.viewDirection, closestHit.normal, 1/refraction));
			vec3 t = normalize(closestHit.viewDirection) - R;

		}
		offset = vec4(0.0f);
		FragColor = calculateShading(closestHit)*(1-fc)+vec4(texture(AtomTexture,TexCoords))*(fc);
		return;
	}
}