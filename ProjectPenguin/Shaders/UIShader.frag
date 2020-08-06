#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;

uniform vec3 color;

void main()
{
    vec4 texColor = texture(texture0, TexCoord);
    
    vec3 rgb = texColor.rgb;
    float alpha = texColor.a;

    rgb *= color;

    FragColor = vec4(rgb, alpha);
}