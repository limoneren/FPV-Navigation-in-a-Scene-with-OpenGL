// rotating cube with texture objects
// two different textures (switch between via key '1' and '2'
// switch to color mode via key 't'
// rotate with keys special keys and aslo key 'y'
// can zoom in and out with 'z' and 'Z'

#include "Angel.h"

const int  NumTriangles = 12; // (6 faces)(2 triangles/face)
const int  NumVertices  = 3 * NumTriangles;
const int  TextureSize  = 128;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// Texture objects and storage for texture image
GLuint textures[2];

GLubyte image[TextureSize][TextureSize][3];
GLubyte image2[TextureSize][TextureSize][3];

GLubyte image3[320][320][3];
GLubyte image4[2000][2000][3];

// cube
point4  points_box[NumVertices];
color4  quad_colors[NumVertices];
vec2    tex_coords[NumVertices];
vec3 normalsForCube[NumVertices];

// cube2
point4  points_box2[NumVertices];

// floor
point4  points_floor[NumVertices];

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat scaleFactor = 1.0;
float mouseX;
float mouseY;

// Model-view and projection matrices uniform location
GLuint ModelView, Projection;

bool textureFlag = true; //enable texture mapping
GLuint  TextureFlag; // texture flag uniform location


vec3 model_positions[] = {
    vec3(1.0, 1.0,1.0),
    vec3(0.0, 0.0, -5.0)
};

// camera attributes
vec3 camera_position(0.0, 1.0, 5.0);
vec3 target_position(0.0, 0.0, 0.0);
vec3 camera_look;
vec3 camera_up(0.0, 1.0, 0.0);
vec3 camera_right(0.0, 0.0, 0.0);
vec3 world_up(0.0, 1.0, 0.0);
float yaw_angle = M_PI;
float pitch_angle = 0.0;
float fov_angle = 45.0;
float time_passed = 1;
float MOUSE_SENSITIVITY = 0.05f;

//----------------------------------------------------------------------------
void move_camera(vec3 offset){
    camera_position += offset;
    
    vec3 new_look;
    new_look.x = cosf(pitch_angle) * sinf(yaw_angle);
    new_look.y = sinf(pitch_angle);
    new_look.z = cosf(pitch_angle) * cosf(yaw_angle);
    
    camera_look = normalize(new_look);
    
    camera_right = normalize(cross(camera_look, world_up));
    camera_up = normalize(cross(camera_right, camera_look));
    
    target_position = camera_position + camera_look;
}
//----------------------------------------------------------------------------

void rotate_camera(float yaw, float pitch){
yaw_angle += DegreesToRadians * yaw;
pitch_angle += DegreesToRadians * pitch;
float min = -M_PI / 2.0f + 0.1f;
float max = M_PI / 2.0f - 0.1f;
if( pitch_angle < min){
    if(min > max){
        pitch_angle = max;
    } else {
        pitch_angle = min;
    }
} else {
    if(pitch_angle > max){
        pitch_angle = max;
    } else {
        //pitch_angle = pitch_angle;
    }
}

vec3 new_look;
new_look.x = cosf(pitch_angle) * sinf(yaw_angle);
new_look.y = sinf(pitch_angle);
new_look.z = cosf(pitch_angle) * cosf(yaw_angle);

camera_look = normalize(new_look);

camera_right = normalize(cross(camera_look, world_up));
camera_up = normalize(cross(camera_right, camera_look));

target_position = camera_position + camera_look;
}


//----------------------------------------------------------------------------

/*  readPPM */

bool
readPPM(char* name){
    
    std::string fileName(name);
    std::string path = "/Users/erenlimon/Desktop/MyApps/410-Term-Project/410-Term-Project/" + fileName;
    
    const char* pathAsPointer = path.c_str();
    
    
    int k, n, m, nm;
    char c;
    int i;
    char b[100];
    //float s;
    int red, green, blue;
    FILE * fd = fopen(pathAsPointer, "r"); // openning to read
    
    if( fd == NULL ){
        std::cout << "File cannot be opened!" << std::endl;
        return false;
    }
    
    fscanf(fd,"%[^\n] ",b);
    
    if(b[0]!='P' || b[1] != '3'){
        printf("%s is not a PPM file!\n", b);
        exit(0);
    }
    printf("%s is a PPM file\n",b);
    fscanf(fd, "%c",&c);
    while(c == '#') {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);
    printf("%d rows %d columns max value= %d\n",n,m,k);
    nm = n*m;
    //int *image = (int *)malloc(3*sizeof(GLuint)*nm);
    
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            fscanf(fd,"%d %d %d",&red, &green, &blue );
            // n-i-1 because ppm's origin is bottom left
            //std::cout << (GLubyte)red << " " << (GLubyte)green << " " << (GLubyte)blue << std::endl;
            if(n == 320){
                image3[n-i-1][j][0] = (GLubyte)red;
                image3[n-i-1][j][1] = (GLubyte)green;
                image3[n-i-1][j][2] = (GLubyte)blue;
            } else if(n == 2000){
                image4[n-i-1][j][0] = (GLubyte)red;
                image4[n-i-1][j][1] = (GLubyte)green;
                image4[n-i-1][j][2] = (GLubyte)blue;
            }
            
        }
    }
    /*
     for(i=nm;i>0;i--){
     // this does not work
     //printf("%d\n",i);
     fscanf(fd,"%d %d %d",&red, &green, &blue );
     image[3*nm-3*i]=red;
     //image[3*nm-3*i][][0] = red;
     image[3*nm-3*i+1]=green;
     //image[3*nm-3*i][][1] = green;
     image[3*nm-3*i+2]=blue;
     //image[3*nm-3*i][][2] = blue;
     }*/
    
    fclose(fd);
    /*
     FILE * fd2 = fopen(pathAsPointer2, "w+");
     
     //36 da 1 new line
     for(i=0;i<nm*3;i++){
     int x = image[i];
     
     if(i % 34 == 0 && i != 0){
     fprintf(fd2, "\n");
     }
     
     fprintf(fd2, "%d ",x);
     
     
     }
     fclose(fd2);
     */
    return true;
}

int Index = 0;

void
quad( int a, int b, int c, int d )
{
    point4 vertices[8] = {
    point4( -1.0, -1.0,  1.0, 1.0 ),
    point4( -1.0,  1.0,  1.0, 1.0 ),
    point4(  1.0,  1.0,  1.0, 1.0 ),
    point4(  1.0, -1.0,  1.0, 1.0 ),
    point4( -1.0, -1.0, -1.0, 1.0 ),
    point4( -1.0,  1.0, -1.0, 1.0 ),
    point4(  1.0,  1.0, -1.0, 1.0 ),
    point4(  1.0, -1.0, -1.0, 1.0 )
    };
    
    point4 vertices_floor[8] = {
        point4( -10.0, -10.0,  10.0, 10.0 ),
        point4( -10.0,  10.0,  10.0, 10.0 ),
        point4(  10.0,  10.0,  10.0, 10.0 ),
        point4(  10.0, -10.0,  10.0, 10.0 ),
        point4( -10.0, -10.0, -10.0, 10.0 ),
        point4( -10.0,  10.0, -10.0, 10.0 ),
        point4(  10.0,  10.0, -10.0, 10.0 ),
        point4(  10.0, -10.0, -10.0, 10.0 )
    };

    color4 colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 0.5, 0.5, 0.5, 1.0 ),  // white
    color4( 1.0, 1.0, 1.0, 1.0 )   // cyan
    };
    
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];
    
    vec3 normal = normalize( cross(u, v) );

    quad_colors[Index] = colors[a];
    points_box[Index] = vertices[a];
    points_floor[Index] = vertices_floor[a];
    points_box2[Index] = vertices[a];
    tex_coords[Index] = vec2( 0.0, 0.0 );
    normalsForCube[Index] = normal;
    Index++;
    
    quad_colors[Index] = colors[a];
    points_box[Index] = vertices[b];
    points_floor[Index] = vertices_floor[b];
    points_box2[Index] = vertices[b];
    tex_coords[Index] = vec2( 0.0, 1.0 );
    normalsForCube[Index] = normal;
    Index++;
    
    quad_colors[Index] = colors[a];
    points_box[Index] = vertices[c];
    points_floor[Index] = vertices_floor[c];
    points_box2[Index] = vertices[c];
    tex_coords[Index] = vec2( 1.0, 1.0 );
    normalsForCube[Index] = normal;
    Index++;
    
    quad_colors[Index] = colors[a];
    points_box[Index] = vertices[a];
    points_floor[Index] = vertices_floor[a];
    points_box2[Index] = vertices[a];
    tex_coords[Index] = vec2( 0.0, 0.0 );
    normalsForCube[Index] = normal;
    Index++;
    
    quad_colors[Index] = colors[a];
    points_box[Index] = vertices[c];
    points_floor[Index] = vertices_floor[c];
    points_box2[Index] = vertices[c];
    tex_coords[Index] = vec2( 1.0, 1.0 );
    normalsForCube[Index] = normal;
    Index++;
    
    quad_colors[Index] = colors[a];
    points_box[Index] = vertices[d];
    points_floor[Index] = vertices_floor[d];
    points_box2[Index] = vertices[d];
    tex_coords[Index] = vec2( 1.0, 0.0 );
    normalsForCube[Index] = normal;
    Index++;
}

//----------------------------------------------------------------------------

void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------
GLuint vaoArray[3];
GLuint buffer, buffer2, buffer3, buffer4;
void
init()
{
    colorcube();
    char* obj1 = "cratenew2.ppm";
    readPPM(obj1);
    char* obj2 = "floor.ppm";
    readPPM(obj2);

    // Create a checkerboard pattern
    for ( int i = 0; i <128; i++ ) {
        for ( int j = 0; j <128; j++ ) {
            GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8)  == 0)) * 255;
            //std::cout << c << std::endl;
            image[i][j][0]  = c;
            image[i][j][1]  = c;
            image[i][j][2]  = c;
            image2[i][j][0] = c;
            image2[i][j][1] = 0;
            image2[i][j][2] = c;
        }
    }
    
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

    // Initialize texture objects
    glGenTextures( 3, textures );
    // ------------ texture 0 ----------------
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 320, 320, 0,
          GL_RGB, GL_UNSIGNED_BYTE, image3 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR); //try here different alternatives
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //try here different alternatives
    // ------------ texture 0 ----------------
    // ------------ texture 1 ----------------
    glBindTexture( GL_TEXTURE_2D, textures[1] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0,
          GL_RGB, GL_UNSIGNED_BYTE, image2 );
    //glGenerateMipmap(GL_TEXTURE_2D); // try also activating mipmaps for the second texture object
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    // ------------ texture 1 ----------------
    // ------------ texture 2 ----------------
    glBindTexture( GL_TEXTURE_2D, textures[2] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 2000, 2000, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image4 );
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR); //try here different alternatives
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //try here different alternatives
    // ------------ texture 2 ----------------
    // Initialize texture objects done
    glBindTexture( GL_TEXTURE_2D, textures[0] ); //set current texture
    
    
    // Create a vertex array object
    glGenVertexArrays( 1, vaoArray );
    
    
    glBindVertexArray( vaoArray[0] );
    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER,
          sizeof(points_box) + sizeof(normalsForCube) + sizeof(tex_coords),
          NULL, GL_STATIC_DRAW );
    // Specify an offset to keep track of where we're placing data in our
    //   vertex array buffer.  We'll use the same technique when we
    //   associate the offsets with vertex attribute pointers.
    GLintptr offset = 0;
    glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(points_box), points_box );
    offset += sizeof(points_box);
    glBufferSubData( GL_ARRAY_BUFFER, offset,
             sizeof(normalsForCube), normalsForCube );
    offset += sizeof(normalsForCube);
    glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(tex_coords), tex_coords );
    // set up vertex arrays
    offset = 0;
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(offset) );
    offset += sizeof(points_box);
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(offset) );
    offset += sizeof(normalsForCube);
    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(offset) );
    
    
    
    // floor object
    glBindVertexArray( vaoArray[1] );
    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer2 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer2 );
    glBufferData( GL_ARRAY_BUFFER,
                 sizeof(points_floor) + sizeof(normalsForCube) + sizeof(tex_coords),
                 NULL, GL_STATIC_DRAW );
    // Specify an offset to keep track of where we're placing data in our
    //   vertex array buffer.  We'll use the same technique when we
    //   associate the offsets with vertex attribute pointers.
    offset = 0;
    glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(points_floor), points_floor );
    offset += sizeof(points_floor);
    glBufferSubData( GL_ARRAY_BUFFER, offset,
                    sizeof(normalsForCube), normalsForCube );
    offset += sizeof(normalsForCube);
    glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(tex_coords), tex_coords );
    // set up vertex arrays
    offset = 0;
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(offset) );
    offset += sizeof(points_box);
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(offset) );
    offset += sizeof(normalsForCube);
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(offset) );
    // floor object
    
    
    // crate object
    glBindVertexArray( vaoArray[2] );
    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer3 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer3 );
    glBufferData( GL_ARRAY_BUFFER,
                 sizeof(points_box2) + sizeof(normalsForCube) + sizeof(tex_coords),
                 NULL, GL_STATIC_DRAW );
    // Specify an offset to keep track of where we're placing data in our
    //   vertex array buffer.  We'll use the same technique when we
    //   associate the offsets with vertex attribute pointers.
    offset = 0;
    glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(points_box2), points_box2 );
    offset += sizeof(points_box2);
    glBufferSubData( GL_ARRAY_BUFFER, offset,
                    sizeof(normalsForCube), normalsForCube );
    offset += sizeof(normalsForCube);
    glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(tex_coords), tex_coords );
    // set up vertex arrays
    offset = 0;
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(offset) );
    offset += sizeof(points_box2);
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(offset) );
    offset += sizeof(normalsForCube);
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(offset) );
    // crate object
    
    
    
    
    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");

    
    
    glUseProgram( program );
    
    TextureFlag = glGetUniformLocation(program, "TextureFlag");
    glUniform1i( TextureFlag, textureFlag );

    glEnable( GL_DEPTH_TEST );
    
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}
float w2;
float h2;
void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    vec3 initial_pos_box( 0.0, 0.0, 0.0 );
    vec3 initial_pos_box2( 3.0, 0.0, 6.0 );
    vec3 initial_pos_floor( 5.0, 1.0, -5.0 );
    vec3 cam_pos( 0.0, 0.0, 0.0 );
    vec3 target_pos( 0.0, 0.0, -1.0 );
    vec3 up( 0.0, 1.0, 0.0 );

    glBindTexture( GL_TEXTURE_2D, textures[1] );
    mat4 view;
    view = LookAt(camera_position, target_position, camera_up);
    
    //Translate( initial_pos );
    glBindVertexArray( vaoArray[0] );
    mat4 model_view = ( view * Translate( -initial_pos_box )
                      );
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    
    glBindVertexArray( vaoArray[1] );
    mat4 model_view2 = ( view * Translate( -initial_pos_floor ) * Scale(50.0, 0.01, 50.0)
                        );
    glBindTexture( GL_TEXTURE_2D, textures[2] );
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view2 );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    
    
    glBindVertexArray( vaoArray[2] );
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    mat4 model_view3 = ( view * Translate( -initial_pos_box2 )
                       );
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view3 );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
    
    
    
    glutSwapBuffers();
}

//----------------------------------------------------------------------------

float acceleration = 0.015; // 0.05 max - 0.005 min

void
idle( void )
{
    
    /*
     scaleFactor[0] *= (1.00 + acceleration);
     scaleFactor[1] *= (1.00 + acceleration);
     scaleFactor[2] *= (1.00 + acceleration);
     if(scaleFactor[0] >= 1.95 || scaleFactor[1] >= 1.95 || scaleFactor[0] >= 1.95){
     scaleFactor[0] = 1.95; // sometimes scalefactor becomes slightly higher than 1.95, which creates a bug
     scaleFactor[1] = 1.95; // so we need to set it back to 1.95
     scaleFactor[2] = 1.95;
     acceleration = -acceleration;
     } else if (scaleFactor[0] <= 1.0 || scaleFactor[1] <= 1.0 || scaleFactor[0] <= 1.0) {
     scaleFactor[0] = 1.0;
     scaleFactor[1] = 1.0;
     scaleFactor[2] = 1.0;
     acceleration = -acceleration;
     }
     */
    
    /*
     Theta[Axis] += 2.0;
     
     if ( Theta[Axis] > 360.0 ) {
     Theta[Axis] -= 360.0;
     }
     */
    /*
     Theta[Axis] += 1.0;
     
     if ( Theta[Axis] > 360.0 ) {
     Theta[Axis] -= 360.0;
     }
     */
    glutPostRedisplay();
    
}
int flag = 0;
void
mouse( int button, int state, int x, int y )
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        flag = 1;
                }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        flag = 0;
    }
   
}

//----------------------------------------------------------------------------

void
passive_mouse( int x, int y )
{
//std::cout << (float)(glutGet(GLUT_WINDOW_WIDTH) / 2.0 - x) * MOUSE_SENSITIVITY << " " << (float)(glutGet(GLUT_WINDOW_HEIGHT) / 2.0 - y) * MOUSE_SENSITIVITY << std::endl;
mouseX = x;
mouseY = y;
rotate_camera((w2 - x) * MOUSE_SENSITIVITY, (h2 - y) * MOUSE_SENSITIVITY);
glutPostRedisplay();
//glutWarpPointer( w2, h2);
//glFinish();
}

// Reshape the application window, preserving the object aspect ratio
void reshape(int w, int h){
    
    glViewport( 0, 0, w, h );
    GLfloat aspect = GLfloat(w) / h;
    
    mat4  projection;
    if (w <= h)
        //projection = Ortho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
        projection = Perspective( 45.0, (GLfloat) h / (GLfloat) w, 0.1, 200 );
    else
        //projection = Ortho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0);
        projection = Perspective( 45.0, (GLfloat) w / (GLfloat) h, 0.1, 200 );
    
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}


//----------------------------------------------------------------------------
float speed = 0.5;
void
keyboard( unsigned char key, int mousex, int mousey )
{
    switch( key ) {
    case 033: // Escape Key
    case 'q': case 'Q':
        exit( EXIT_SUCCESS );
        break;
        case '1':
            glBindTexture( GL_TEXTURE_2D, textures[0] );
            break;

        case '2':
            glBindTexture( GL_TEXTURE_2D, textures[1] );
            break;
        case 't':
            if (textureFlag == true) textureFlag = false;
            else textureFlag = true;
            glUniform1i( TextureFlag, textureFlag );
            glutPostRedisplay();
            break;
            
        case 'y':
            Theta[Yaxis] -= 3.0;
            
            if (Theta[Yaxis] < -360.0){
                Theta[Yaxis] += 360.0;
            }
            break;
            // "Zoom-in" to the object
        case 'z':
            scaleFactor *= 1.1;
            break;
            
            // "Zoom-out" from the object
        case 'Z':
            scaleFactor *= 0.9;
            break;
        case 'w':
            move_camera(speed * camera_look);
            //time_passed += 0.000000001;
            //speed += 0.000005;
            break;
        case 's':
            move_camera(speed * -camera_look);
            //time_passed += 1000000;
            //speed += 0.00000000005;
            break;
        case 'a':
            move_camera(speed * -camera_right);
            //speed += 0.000005;
            break;
        case 'd':
            move_camera(speed * camera_right);
            //speed += 0.000005;
            break;


    }

    glutPostRedisplay();
}

void special(int key, int x, int y){
    switch (key) {
        case GLUT_KEY_UP:
            Theta[Xaxis] += 3.0;
            
            if (Theta[Xaxis] > 360.0){
                Theta[Xaxis] -= 360.0;
            }
            break;
            
        case GLUT_KEY_DOWN:
            Theta[Xaxis] -= 3.0;
            
            if (Theta[Xaxis] < -360.0){
                Theta[Xaxis] += 360.0;
            }
            break;
            
        case GLUT_KEY_LEFT:
            Theta[Zaxis] += 3.0;
            
            if (Theta[Zaxis] > 360.0){
                Theta[Zaxis] -= 360.0;
            }
            break;
            
        case GLUT_KEY_RIGHT:
            Theta[Zaxis] -= 3.0;
            
            if (Theta[Zaxis] < -360.0){
                Theta[Zaxis] += 360.0;
            }
            break;
            
        default:
            break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
void timer( int p )
{
//glutWarpPointer( w2, h2);
//glutPostRedisplay();

glutTimerFunc(5,timer,0);
}
//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Comp 410 - Term Project - 0054129" );
    w2 = glutGet(GLUT_WINDOW_WIDTH) / 2.0;
    h2 = glutGet(GLUT_WINDOW_HEIGHT) / 2.0 - 10;
    init();
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouse );
    glutWarpPointer( w2, h2);
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    glutPassiveMotionFunc(passive_mouse);
    glutReshapeFunc( reshape );
    glutSpecialFunc( special );
    //glutTimerFunc(5,timer,0);
    
    glutMainLoop();
    return 0;
}
