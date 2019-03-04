#version 410

in vec2 uv;

out vec4 FragmentColour;

uniform sampler2D SpriteTexture; 

void main(void)
{

    //FragmentColour = vec4(0.5,0.0,0.5,1.0f);
	vec4 textureColour = texture(SpriteTexture, uv);

	if(textureColour.a < 0.1)
        discard;

	FragmentColour = textureColour;

}
