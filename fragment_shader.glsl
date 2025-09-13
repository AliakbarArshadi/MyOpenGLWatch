#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords; // مختصات بافت

// متغیرهای ورودی از برنامه اصلی
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1; // بافت Diffuse
// uniform sampler2D texture_specular1; // بافت Specular (اگر مدل داشته باشد)

void main()
{
    // ۱. Ambient lighting (نور محیط)
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * lightColor;
  	
    // ۲. Diffuse lighting (پخش نور)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // ۳. Specular lighting (براقیت)
    float specularStrength = 0.5; // می‌توانید این را تغییر دهید
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // عدد بالاتر = براقیت کمتر و متمرکزتر
    vec3 specular = specularStrength * spec * lightColor;
        
    vec3 lighting = (ambient + diffuse + specular);
    
    // رنگ نهایی را از بافت diffuse بگیرید و با نور ترکیب کنید
    // اگر مدل بافت diffuse ندارد، objectColor (سفید) استفاده می‌شود
    FragColor = texture(texture_diffuse1, TexCoords) * vec4(lighting, 1.0); 
}   