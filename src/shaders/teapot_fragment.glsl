#version 450 core

in vec3 vNormal;
in vec3 FragPos;
in vec2 texCoord;

uniform vec3 lightColor;
uniform vec3 lightPos;

uniform sampler2D tex;
// uniform bool useTexture;

out vec4 fragmentColor;
void main() {

    //TODO: Add support for specularity and transparency
    
    vec3 ambient = lightColor * 0.2;
    vec4 texColor = texture(tex, texCoord);

    vec3 norm = normalize(vNormal);
    vec3 direction = normalize(lightPos - FragPos);
    vec3 diffuse = max(dot(direction, norm), 0.0) * lightColor;

    fragmentColor = vec4(texColor.rgb * (ambient + diffuse), 1.0);
    //fragmentColor = vec4(ambient + diffuse, 1.0);
}