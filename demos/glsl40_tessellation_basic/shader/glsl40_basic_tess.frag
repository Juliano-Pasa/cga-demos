// Fragment Shader
//
// Simples Fragment Shader que seta a cor de todos os fragmentos para vermelho.
// Nenhum cálculo de iluminação é feito.
//
// Abril 2016 - Alex Frasson - afrasson@inf.ufsm.br

#version 400

in vec3 normalVector;
in vec3 viewDirection;
in vec3 lightDirection0;
in vec3 lightDirection1;

uniform vec3 lightColor;
uniform vec3 oceanColor;

const float ambientIntensity = 0.3;
const float specularIntensity = 0.5;
const float specularBloom = 32;

// Essa funcao calcula os valores das luzes difusas
vec3 DiffusedLights()
{
	float diffusion0 = max(dot(normalVector, lightDirection0), 0.0);
	vec3 diffusedLight0 = diffusion0 * lightColor;
	
	float diffusion1 = max(dot(normalVector, lightDirection1), 0.0);
	vec3 diffusedLight1 = diffusion1 * lightColor;

	return diffusedLight0 + diffusedLight1;
}

// Essa funcao calcula os valores das luzes especulares
vec3 SpecularLights()
{
	vec3 halfWayVector0 = normalize(viewDirection + lightDirection0);
	float specular0 = pow(max(dot(normalVector, halfWayVector0), 0.0), specularBloom);
	vec3 specularLight0 = specularIntensity * specular0 * lightColor;

	vec3 halfWayVector1 = normalize(viewDirection + lightDirection1);
	float specular1 = pow(max(dot(normalVector, halfWayVector1), 0.0), specularBloom);
	vec3 specularLight1 = specularIntensity * specular1 * lightColor;

	return specularLight0 + specularLight1;
}

void main()
{
	// Calculo da luz de ambiente
	vec3 ambientLight = ambientIntensity * lightColor;

	// Soma de todas as luzes
	vec3 lighting = ambientLight + DiffusedLights() + SpecularLights();

	// Multiplicacao das luzes pela luz do objeto (oceano)
	vec3 color = lighting * oceanColor;
	gl_FragColor = vec4(color, 1.0);
}