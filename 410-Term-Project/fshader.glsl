varying  vec4 color;
varying  vec2 texCoord;
uniform int TextureFlag;

uniform sampler2D texture;

void main() 
{ 
    if (TextureFlag == 1)
        //sample a texture color from texture object
        //with newer versions of OpenGL you can  use texture() function instead of deprecated texture2D() 
        gl_FragColor = texture2D( texture, texCoord );

    else gl_FragColor = color;
    
} 

