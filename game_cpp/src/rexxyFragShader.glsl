#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPosView;
uniform vec3 lightColor;

void main(){

    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength*lightColor;

    // normalizing
    vec3 norm = normalize(Normal);
    vec3 dir = normalize(lightPosView-FragPos);

    float diff = dot(dir,norm);
    vec3 diffuse = diff*lightColor;

    vec3 viewDir = normalize(vec3(0.0f)-FragPos);
    vec3 refDir = reflect(-dir,norm);

    float specularStrength = 0.5f;
    float spec = pow(max(dot(viewDir,refDir),0.0f),32);
    vec3 specular = specularStrength*spec*lightColor;

    FragColor = vec4((specular+diffuse+ambient)*ourColor,1.0f);
}