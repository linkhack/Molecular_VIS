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

in vec2 TexCoords;
in vec3 viewDirection;
in Ray ray;

//Lights
uniform PointLight pointLights[_POINT_LIGHTS_COUNT];
uniform DirectionalLight directionalLights[_DIRECTIONAL_LIGHTS_COUNT];
uniform SpotLight spotLights[_SPOT_LIGHT_COUNT];
uniform int nrPointLight;
uniform int nrDirLight;
uniform int nrSpotLight;

uniform vec3 cameraPosition;

uniform sampler3D SESTexture;
uniform vec3 grid_min;
uniform vec3 grid_max;
uniform float subsurfaceDepth;

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

vec3 calculateNormal(vec3 pos){
	const float eps=0.1;
    const vec3 v1 = vec3( 1.0,-1.0,-1.0);
    const vec3 v2 = vec3(-1.0,-1.0, 1.0);
    const vec3 v3 = vec3(-1.0, 1.0,-1.0);
    const vec3 v4 = vec3( 1.0, 1.0, 1.0);

    return normalize( v1 * sceneSDF( pos + v1*eps ).x +
                    v2 * sceneSDF( pos + v2*eps ).x +
                    v3 * sceneSDF( pos + v3*eps ).x +
                    v4 * sceneSDF( pos + v4*eps ).x );
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
	const int nbIte = 16;
	const float maxDist = 16.f;
	float falloff=1.0;
    for( int i=0; i<nbIte; i++ )
    {
        float l = (0.01+float(i)/float(nbIte))*maxDist;
        vec3 rd = hit.normal*l;
        ao += (l - max(sceneSDF( hit.position + rd ),0.f))/maxDist*falloff;
		falloff*=0.9;
    }

	float light =1.f - 2.f*ao/float(nbIte);
	//Subsurface
	vec3 innerPosition = hit.position + subsurfaceDepth*hit.viewDirection;
	float innerSDF = sceneSDF(innerPosition);
	float scalingFactor = (subsurfaceDepth - innerSDF)/(2*subsurfaceDepth);

	color = mat.color*mat.diffuse*light + .5f*vec3(1-scalingFactor);
	return vec4(vec3(color),1.f);

}

void main() {
	vec3 rayDirection = normalize(ray.direction);
	Hit closestHit = rayMarching(ray.origin, rayDirection);
	if(closestHit.distance > MAX_DISTANCE){
		FragColor = WHITE;
		return;
	}else{
		FragColor = calculateShading(closestHit);
		return;
	}
}