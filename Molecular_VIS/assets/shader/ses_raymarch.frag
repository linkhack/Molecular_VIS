#version 460 core

#define EPSILON 0.0001
#define EPSILON_GRAD 0.000001
#define BLACK vec4(0.0f,0.0f,0.0f,1.0f);
#define WHITE vec4(1.0f,1.0f,1.0f,1.0f);
#define MAX_STEPS 300
#define KERNEL_SIZE 5

#define PI 3.1415926

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
	float depth;
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
	float reflectionCoefficient;
};

out vec4 FragColor;

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
uniform vec3 cameraDirection;
uniform float near;
uniform float far;

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
uniform bool translucencyOn;

uniform mat4 viewProjectionMatrix;


const vec2 texOffset = 1.0/textureSize(AtomTexture,0);

//Matrial constant
const Material mat = {vec3(0.0f,0.0f,1.f),vec3(0.1f),vec3(0.8f),vec3(0.1f),15.0f, 0.1f};

vec3 scale_unit(vec3 pos)
{
	return (pos - grid_min) / (grid_max - grid_min);
}

float sceneSDF(vec3 pos)
{
	return texture(SESTexture,scale_unit(pos)).r;
}

vec3 calculateNormal(vec3 pos){
	const float eps=1.07f;
	const vec2 k = vec2(1,-1);

    return normalize( k.xyy * (sceneSDF( pos + k.xyy*eps ))+
                    k.yyx * (sceneSDF( pos + k.yyx*eps ) )+
                    k.yxy * (sceneSDF( pos + k.yxy*eps ))+
                    k.xxx * (sceneSDF( pos + k.xxx*eps )));
}

float linearizeDepth(float depth)
{
	return near * far / (far + depth * (near - far));
}

float weight(int i,int j, float sig)
{
	return 1/(2*PI*sig*sig)*exp(-(i*i+j*j)/2*sig*sig);
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
			result.normal = calculateNormal(result.position);
			result.viewDirection = direction;
			result.steps = i;
			result.depth = dot(cameraDirection, depth*direction);
			return result;
		}
		depth += dist;
		if(depth > far){
			result.distance = far + EPSILON;
			result.depth = far+EPSILON;
			result.steps = i;
			return result;
		}
	}
	result.distance = far + EPSILON;
	result.depth = far + EPSILON;
	result.steps = MAX_STEPS;
	return result;
}

//Calculates Shading of Hit w.r.t all lights
vec4 calculateShading(Hit hit){
	//
	if(hit.distance > far) return WHITE;
	vec3 color;
	float ao = 0.0;
	const int nbIte = 6;
	const float maxDist = 3.f;
	const float start = 0.1f;
	const float step = (maxDist - start) / float(nbIte-1);
	float distance = start;
	float falloff=1.0;
    for( int i=0; i<nbIte; i++ )
    {
        vec3 rd = hit.normal*distance;
        ao += (distance - max(sceneSDF( hit.position + rd ),0.f));
		distance += step;
    }

	float light =1.0f-ao/(maxDist*float(nbIte));
	//Subsurface
	vec3 innerPosition = hit.position + subsurfaceDepth*hit.viewDirection;
	float innerSDF = sceneSDF(innerPosition);
	float scalingFactor = (subsurfaceDepth - innerSDF)/(2*subsurfaceDepth);
	color = light*(mat.color*mat.diffuse + 0.75f*vec3(1.f-scalingFactor));
	return vec4(vec3(color),1.f);
}

void main() {
	vec3 rayDirection = normalize(ray.direction);
	Hit closestHit = rayMarching(ray.origin, rayDirection);
	vec4 directColor = calculateShading(closestHit);
	if(closestHit.distance>far)
	{
		FragColor = directColor;
		return;
	}
	vec4 reflectionColor = vec4(0.0f);
	if(reflectionOn)
	{
		Ray reflection;
		reflection.origin = closestHit.position;
		reflection.direction = reflect(ray.direction, closestHit.normal);
		Hit reflectionHit = rayMarching(reflection.origin, reflection.direction);
		reflectionColor = calculateShading(reflectionHit);
	}
	vec4 surfaceColor = directColor + mat.reflectionCoefficient*reflectionColor;
	vec4 offset = vec4(0.f);
	// add atoms

	// Calculate refraction offset
	float depthDifference = linearizeDepth(texture(AtomDepth,TexCoords).x)-closestHit.depth;
	vec2 center;
	float maxDepth = dmax*subsurfaceDepth;
	if(refractionOn)
	{
		vec3 R = refract(closestHit.viewDirection, closestHit.normal, 1.f/refraction);
		vec3 refractedWordPos = closestHit.position + depthDifference*R;
		vec4 screenSpace = viewProjectionMatrix * vec4(refractedWordPos,1.f);
		screenSpace /= screenSpace.w;
		screenSpace = 0.5f*(screenSpace + vec4(1.0f)); 
		center = screenSpace.xy;
	}
	else
	{
		center = TexCoords;
	}

	//Calculate blending factors (on new position)
	
	float sigma = min((linearizeDepth(texture(AtomDepth,center).x)-closestHit.depth)/maxDepth,1); //oder by TexCoords
	float fc = clamp(exp(-5*sigma),0.f,1.f);

	vec4 atomsColor = vec4(0.f);
	if(translucencyOn)
	{
		vec3 result=vec3(0.0f);
		float totalWeight = 0.f;
		for(int i=-KERNEL_SIZE;i<KERNEL_SIZE;i++)
		{
			for(int j=-KERNEL_SIZE;j<KERNEL_SIZE;j++)
			{
				float w = weight(i,j,fc);
				result += texture(AtomTexture,center+vec2(texOffset.x*i,texOffset.y*j)).rgb*w;
				totalWeight += w;
			}
		}
		atomsColor = vec4(result/totalWeight,1.0f);
	}
	else
	{
		atomsColor = texture(AtomTexture,center);
	}
	FragColor = (1.0f-fc)*surfaceColor + fc*atomsColor;
	return;
	
}