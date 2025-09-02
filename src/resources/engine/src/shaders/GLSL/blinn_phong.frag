// Blinn Phong Fragment Shader
#version 460 core
#define MAX_NUMBER_OF_DIRECTIONAL_LIGHTS 2
#define MAX_NUMBER_OF_SPOT_LIGHTS        48
#define MAX_NUMBER_OF_POINT_LIGHTS       50

out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

struct Material
{
	// Todo: combine color and alpha into a vec4
	vec3 diffuse_color;
	float alpha;
	sampler2D texture_diffuse;

	// Until I implement metallic, roughness, etc I'll just directly affect the specular
	float specular_strength;
	int specular_sharpness;
	sampler2D texture_specular;

	// These will eventually replace the specular variables, as they'll each affect the specular highlights in different ways
	// sampler2D texture_metallic;
	// float metallic_strength;
	// sampler2D texture_roughness;
	// float roughness_strength;
};

struct Light
{
	vec3 color;
	float energy;
	float specular_strength;
	float ambient_strength;

	float attenuation;
	float range;

	vec3 position;
	vec3 direction;

	float spot_cutoff;
	float spot_cutoff_fade;

	// Texture *projection_texture;
};

// uniform vec3 environment_ambient_light_color;
// uniform float environment_ambient_light_contribution;

uniform Material current_material;

uniform Light directional_lights[MAX_NUMBER_OF_DIRECTIONAL_LIGHTS];
uniform Light point_lights[MAX_NUMBER_OF_POINT_LIGHTS];
uniform Light spot_lights[MAX_NUMBER_OF_SPOT_LIGHTS];

uniform int directional_lights_count; // Honestly, should only be 1, but you do you, I guess
uniform int point_lights_count;
uniform int spot_lights_count;

uniform int enable_ambient;
uniform int enable_diffuse;
uniform int enable_specular;

uniform vec3 view_position;
uniform vec4 debug_highlight;

vec3 calculatePointLight(Light light);
vec3 calculateSpotLight(Light light);
vec3 calculateDirectionalLight(Light light);
float calculateAttenuation(float range, float constant, float distance);
mat3x3 calculateLuminosity(Light light, vec3 light_direction);

void main()
{
	vec3 output_color = vec3(0.0f);

	for(int i = 0 ; i < directional_lights_count ; i++)
		output_color += calculateDirectionalLight(directional_lights[i]);
	for(int i = 0 ; i < point_lights_count ; i++)
		output_color += calculatePointLight(point_lights[i]);
	for(int i = 0 ; i < spot_lights_count ; i++)
		output_color += calculateSpotLight(spot_lights[i]);

	// Todo: once more than one material is supported, make sure that the alpha isn't just affected by one
	if(current_material.alpha < 0.5f)
		discard; // THIS IS A HACK FIX AND WILL BE REPLACED ONCE I HAVE PROPER TRANSPARENCY WORKING!!!!
	FragColor = vec4(output_color, 1.0f) + debug_highlight;
}

vec3 calculatePointLight(Light light)
{
	mat3x3 light_components = calculateLuminosity(light, normalize(light.position - fragment_position));
	float attenuation = calculateAttenuation(light.range, light.attenuation, length(light.position - fragment_position));

	light_components[0] *= attenuation;
	light_components[1] *= attenuation;
	light_components[2] *= attenuation;

	return light_components[0] + light_components[1] + light_components[2];
}

vec3 calculateSpotLight(Light light)
{
	vec3 light_direction = normalize(light.position - fragment_position);
	mat3x3 light_components = calculateLuminosity(light, light_direction);
	float attenuation = calculateAttenuation(light.range, light.attenuation, length(light.position - fragment_position));

	float theta = dot(light_direction, normalize(-light.direction));
	float epsilon = light.spot_cutoff_fade - light.spot_cutoff;
	float spotlight_radius = clamp((theta - light.spot_cutoff) / epsilon, 0.0f, 1.0f);

	light_components[1] *= attenuation * spotlight_radius;
	light_components[2] *= attenuation * spotlight_radius;

	return (light_components[0] + light_components[1] + light_components[2]);
}

vec3 calculateDirectionalLight(Light light)
{
	mat3x3 light_components = calculateLuminosity(light, normalize(-light.direction));
	return (light_components[0] + light_components[1] + light_components[2]);
}

mat3x3 calculateLuminosity(Light light, vec3 light_direction)
{
	float light_distance = length(light.position - fragment_position);
	vec3 reflect_direction = reflect(-light_direction, normalize(vertex_normal));
	vec3 blinn_halfway_vector = normalize(light_direction + normalize(view_position - fragment_position));

	float diffuse = max(dot(normalize(vertex_normal), light_direction), 0.0f);
	float specular = pow(max(dot(normalize(vertex_normal), blinn_halfway_vector), 0.0f), current_material.specular_sharpness);

	specular = diffuse != 0 ? specular : 0.0f;

	vec3 this_ambient = light.ambient_strength * light.color * texture(current_material.texture_diffuse, vertex_uv).rgb * current_material.diffuse_color * vertex_color;
	vec3 this_diffuse = light.energy * light.color * texture(current_material.texture_diffuse, vertex_uv).rgb * current_material.diffuse_color * vertex_color * diffuse;
	vec3 this_specular = light.specular_strength * light.color * texture(current_material.texture_specular, vertex_uv).rgb * current_material.specular_strength * specular;

	return mat3x3(this_ambient * enable_ambient, this_diffuse * enable_diffuse, this_specular * enable_specular);
}

float calculateAttenuation(float range, float constant, float distance)
{
	// How I got the numbers in this attenuation calculation:
	// 		https://www.desmos.com/calculator/vtbqukgvgp
	// Where I got the starting data:
	//		https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
	float linear = (4.69051 * pow(range, -1.00971));
	float quadratic = 82.44478 * pow(range, -2.01921);
	return (1.0f / (constant + linear * distance + quadratic * pow(distance, 2)));
}
