#version 450 core

in vec3 vNormal;
in vec3 FragPos;
in vec2 texCoord;

uniform vec3 lightColor;
uniform vec3 lightPos;

uniform sampler2D tex;
uniform bool useTexture;

out vec4 fragmentColor;

void main() {
    vec3 ambient = lightColor * 0.2;
    
    vec3 norm = normalize(vNormal);
    vec3 direction = normalize(lightPos - FragPos);
    vec3 diffuse = max(dot(direction, norm), 0.0) * lightColor;
    
    vec3 result = ambient + diffuse;
    
    if(useTexture) {
        vec4 texColor = texture(tex, texCoord);
        result *= texColor.rgb;  // Multiply by texture color
        fragmentColor = vec4(result, texColor.a);  // Use texture alpha if available
    } else {
        fragmentColor = vec4(result, 1.0);
    }
}