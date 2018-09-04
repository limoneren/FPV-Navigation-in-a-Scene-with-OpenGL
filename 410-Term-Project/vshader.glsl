attribute  vec4 vPosition;
attribute  vec4 vColor;
attribute  vec2 vTexCoord;

varying vec4 color;
varying vec2 texCoord;

uniform mat4 ModelView;
uniform mat4 Projection;

void main() 
{
    color       = vColor;
    texCoord    = vTexCoord;
    gl_Position = Projection * ModelView * vPosition;
} 
