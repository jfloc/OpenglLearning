#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;
out vec4 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	
    // Transform the normal to view space
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
	//Normal =  normalize(cross(vec3(fwidth(FragPos.x), fwidth(FragPos.y), fwidth(FragPos.z)), FragPos));

	
	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	
}