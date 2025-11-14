#version 450 core

in vec3 vNormal;
in vec3 FragPos;
in vec2 texCoord;

//TODO: Make a struct out of this for improved readability

// uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float specExponent;

uniform sampler2D tex;
uniform bool useTexture;

out vec4 fragmentColor;

void main() {
    vec3 ambient = ambientColor * 0.2;
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09f * distance + 0.032f * (distance * distance));
    
    vec3 norm = normalize(vNormal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    vec3 viewDirection = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);

    vec3 diffuse = max(dot(lightDirection, norm), 0.0) * diffuseColor;
    vec3 specular = pow(max(dot(viewDirection, reflectDir), 0.0), specExponent) * specularColor;
    
    vec3 result = ambient + diffuse + specular;
    // result *= attenuation;
    
    if(useTexture) {
        vec4 texColor = texture(tex, texCoord);
        result *= texColor.rgb;  // Multiply by texture color
        fragmentColor = vec4(result, texColor.a);  // Use texture alpha if available
    } else {
        fragmentColor = vec4(result, 1.0);
    }
}