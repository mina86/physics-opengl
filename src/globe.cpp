//-----------------------------------------------------------------------------
// File: globe.cpp
// Desc: sample showing how to texture a sphere with OpenGL, glu quadric
//
// Autumn 2000 - Ge Wang - implementation
//        ( RGB file loading routine by David Blythe, SGI )
// Autumn 2008 - Michal Nazarewicz - minor modifications
//-----------------------------------------------------------------------------
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <stdio.h>
#include <string.h>




//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void tick(int timeout);
void displayFunc( );
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );

void initialize( );

struct GeImageData;
bool ge_read_image( const char * filename, GeImageData * pImgData );
unsigned * ge_read_rgb( const char * name, int * width,
                        int * height, int * comp );




//-----------------------------------------------------------------------------
// Name: struct GeImageData
// Desc: holds basic image data
//-----------------------------------------------------------------------------
struct GeImageData
{
    int width;
    int height;
    unsigned * bits;

    GeImageData( int w = 0, int h = 0, unsigned * p = 0 )
        : width( w ), height( h ), bits( p )
    { }
};




//-----------------------------------------------------------------------------
// global variables and #defines
//-----------------------------------------------------------------------------

// width and height of the window
GLsizei g_width = 320;
GLsizei g_height = 320;

// texture and glu data
GLUquadricObj * g_sphere = NULL;
GeImageData g_imgData;
GLuint g_img = 0;

GLfloat g_angle_x = 27.0f;
GLfloat g_inc = 0.0f;
GLfloat g_rotation = 0.0f;
char g_filename[128] = "data/earth.hq.sgi";

// light position
GLfloat g_light_position[4] = { 2.0f, 1.2f, 4.0f, 1.0f };



//-----------------------------------------------------------------------------
// Name: main( )
// Desc: starting point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    // initialize GLUT
    glutInit( &argc, argv );
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( g_width, g_height );
    // set the window postion
    glutInitWindowPosition( 100,100 );
    // create the window
    glutCreateWindow( argv[0] );

    // set the tick function - called each 20 ms
    glutTimerFunc(20, tick, 20);
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );

    if( argc > 1 )
        strcpy( g_filename, argv[1] );

    // do our own initialization
    initialize();

    // let GLUT handle the current thread from here
    glutMainLoop();

    return 0;
}




//-----------------------------------------------------------------------------
// Name: initialize( )
// Desc: sets initial OpenGL states
//       also initializes any application data
//-----------------------------------------------------------------------------
void initialize()
{
    // set the GL clear color - use when the color buffer is cleared
    glClearColor( 0.0f, 0.0f,0.0f, 0 );
    // set the shading model to 'smooth'
    glShadeModel( GL_SMOOTH );

    // enable depth
    glEnable( GL_DEPTH_TEST );
    // set the front faces of polygons
    glFrontFace( GL_CCW );
    // cull faces
    glEnable( GL_CULL_FACE );

    // set the initial line width
    glLineWidth( 1.0f );

    // enable lighting
    glEnable( GL_LIGHTING );
    // enable lighting for front
    glLightModeli( GL_FRONT, GL_TRUE );
    // material have diffuse and ambient lighting
    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    // enable color
    glEnable( GL_COLOR_MATERIAL );
    // enable light 0
    glEnable( GL_LIGHT0 );

    // initialize our sphere
    g_sphere = gluNewQuadric( );
    gluQuadricNormals( g_sphere, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( g_sphere, GL_TRUE );

    // set texture state
    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    // read the texture
    if( !ge_read_image( g_filename, &g_imgData ) )
    {
        printf( "couldn't open file '%s'...\n", g_filename );
        printf( "usage: globe <filename>.rgb\n" );
        exit(1);
    }

    // generate OpenGL texture
    glGenTextures( 1, &g_img );
    glBindTexture( GL_TEXTURE_2D, g_img );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        g_imgData.width,
        g_imgData.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        g_imgData.bits
        );

    printf( "----------------------------------------------------\n" );
    printf( "Texture mapping sample in OpenGL\n" );
    printf( "----------------------------------------------------\n" );
    printf( "[L/R] mouse button to rotate about x-axis\n" );
    printf( "----------------------------------------------------\n" );
    printf( "\n" );
}




//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void tick(int param)
{
    g_angle_x += g_inc;
    g_rotation += 0.4;
    // render the scene
    glutPostRedisplay( );
    glutTimerFunc(param, tick, param);
}




//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{

    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if( g_img )
    {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, g_img );
    }

    // save the current matrix
    glPushMatrix( );

        // rotate the view point
        glRotatef( g_angle_x, 1.0f, 0.0f, 0.0f );

        // rotate the planet
        glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
        glRotatef( g_rotation, 0.0f, 0.0f, 1.0f );

        // draw the sphere
        glColor3f( 1.0f, 1.0f, 1.0f );
        gluSphere( g_sphere, 1.28f, 24, 24 );

    glPopMatrix( );

    if( g_img )
        glDisable( GL_TEXTURE_2D );

    glFlush( );
    glutSwapBuffers( );
}




//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( GLsizei w, GLsizei h )
{
    // save the new window size
    g_width = w; g_height = h;
    // map the view port to the client area
    glViewport(0, 0, w, h);
    // set the matrix mode to project
    glMatrixMode(GL_PROJECTION);
    // load the identity matrix
    glLoadIdentity();
    // create the viewing frustum
    gluPerspective(45.0, (GLfloat) w / (GLfloat) h, 1.0, 300.0);
    // set the matrix mode to modelview
    glMatrixMode(GL_MODELVIEW);
    // load the identity matrix
    glLoadIdentity();
    // translate so we can have a better view
    glTranslatef(0.0, 0.0, -5.0f );
    // set the position of the light
    glLightfv( GL_LIGHT0, GL_POSITION, g_light_position );
}




//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    switch( key )
    {
    case 'Q':
    case 'q':
        exit(1);
    break;
    case '-':
        g_angle_x -= 1.6f;
    break;
    case '+':
    case '=':
        g_angle_x += 1.6f;
    break;
    }
}




//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON )
    {
        // when left mouse button is down, move left
        if( state == GLUT_DOWN )
            g_inc -= 0.8f;
        else
            g_inc += 0.8f;
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        // when right mouse button down, move right
        if( state == GLUT_DOWN )
            g_inc += .8f;
        else
            g_inc -= .8f;
    }
    else
    {
        // reset the location of object
        g_angle_x = 27.0f;
        g_inc = 0.0f;
    }
}




//-----------------------------------------------------------------------------
// Name: ge_read_image( )
// Desc: reads an RGB file into pImgData
//-----------------------------------------------------------------------------
bool ge_read_image( const char * filename, GeImageData * pImgData )
{
    int c;
    if( !filename || !pImgData )
        return false;

    // zero out the memory
    memset( pImgData, 0, sizeof( GeImageData ) );

    pImgData->bits = ge_read_rgb( filename, &pImgData->width,
                                  &pImgData->height, &c );

    return ( pImgData->bits != 0 );
}




//-----------------------------------------------------------------------------
/* texture loading - by David Blythe, SGI */
/* a simplistic routine for reading an SGI .rgb image file. */
//-----------------------------------------------------------------------------




void
bwtorgba(unsigned char *b,unsigned char *l,int n) {
    while(n--) {
        l[0] = *b;
        l[1] = *b;
        l[2] = *b;
        l[3] = 0xff;
        l += 4; b++;
    }
}

void
rgbtorgba(unsigned char *r,unsigned char *g,unsigned char *b,
          unsigned char *l,int n) {
    while(n--) {
        l[0] = r[0];
        l[1] = g[0];
        l[2] = b[0];
        l[3] = 0xff;
        l += 4; r++; g++; b++;
    }
}

void
rgbatorgba(unsigned char *r,unsigned char *g,unsigned char *b,
           unsigned char *a,unsigned char *l,int n) {
    while(n--) {
        l[0] = r[0];
        l[1] = g[0];
        l[2] = b[0];
        l[3] = a[0];
        l += 4; r++; g++; b++; a++;
    }
}

typedef struct _ImageRec {
    unsigned short imagic;
    unsigned short type;
    unsigned short dim;
    unsigned short xsize, ysize, zsize;
    unsigned int min, max;
    unsigned int wasteBytes;
    char name[80];
    unsigned long colorMap;
    FILE *file;
    unsigned char *tmp, *tmpR, *tmpG, *tmpB;
    unsigned long rleEnd;
    unsigned int *rowStart;
    int *rowSize;
} ImageRec;

static void
ConvertShort(unsigned short *array, unsigned int length) {
    unsigned short b1, b2;
    unsigned char *ptr;

    ptr = (unsigned char *)array;
    while (length--) {
        b1 = *ptr++;
        b2 = *ptr++;
        *array++ = (b1 << 8) | (b2);
    }
}

static void
ConvertUint(unsigned *array, unsigned int length) {
    unsigned int b1, b2, b3, b4;
    unsigned char *ptr;

    ptr = (unsigned char *)array;
    while (length--) {
        b1 = *ptr++;
        b2 = *ptr++;
        b3 = *ptr++;
        b4 = *ptr++;
        *array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
    }
}

static ImageRec *ImageOpen(const char *fileName)
{
    union {
        int testWord;
        char testByte[4];
    } endianTest;
    ImageRec *image;
    int swapFlag;
    int x;

    endianTest.testWord = 1;
    if (endianTest.testByte[0] == 1) {
        swapFlag = 1;
    } else {
        swapFlag = 0;
    }

    image = (ImageRec *)malloc(sizeof(ImageRec));
    if (image == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    if ((image->file = fopen(fileName, "rb")) == NULL) {
        perror(fileName);
        //exit(1);
        return NULL;
    }

    fread(image, 1, 12, image->file);

    if (swapFlag) {
        ConvertShort(&image->imagic, 6);
    }

    image->tmp = (unsigned char *)malloc(image->xsize*256);
    image->tmpR = (unsigned char *)malloc(image->xsize*256);
    image->tmpG = (unsigned char *)malloc(image->xsize*256);
    image->tmpB = (unsigned char *)malloc(image->xsize*256);
    if (image->tmp == NULL || image->tmpR == NULL || image->tmpG == NULL ||
        image->tmpB == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }

    if ((image->type & 0xFF00) == 0x0100) {
        x = image->ysize * image->zsize * (int) sizeof(unsigned);
        image->rowStart = (unsigned *)malloc(x);
        image->rowSize = (int *)malloc(x);
        if (image->rowStart == NULL || image->rowSize == NULL) {
            fprintf(stderr, "Out of memory!\n");
            exit(1);
        }
        image->rleEnd = 512 + (2 * x);
        fseek(image->file, 512, SEEK_SET);
        fread(image->rowStart, 1, x, image->file);
        fread(image->rowSize, 1, x, image->file);
        if (swapFlag) {
            ConvertUint(image->rowStart, x/(int) sizeof(unsigned));
            ConvertUint((unsigned *)image->rowSize, x/(int) sizeof(int));
        }
    }
    return image;
}

static void
ImageClose(ImageRec *image) {
    fclose(image->file);
    free(image->tmp);
    free(image->tmpR);
    free(image->tmpG);
    free(image->tmpB);
    free(image);
}

static void
ImageGetRow(ImageRec *image, unsigned char *buf, int y, int z) {
    unsigned char *iPtr, *oPtr, pixel;
    int count;

    if ((image->type & 0xFF00) == 0x0100) {
        fseek(image->file, (long) image->rowStart[y+z*image->ysize],
              SEEK_SET);
        fread(image->tmp, 1, (unsigned int)image->rowSize[y+z*image->ysize],
              image->file);

        iPtr = image->tmp;
        oPtr = buf;
        for (;;) {
            pixel = *iPtr++;
            count = (int)(pixel & 0x7F);
            if (!count) {
                return;
            }
            if (pixel & 0x80) {
                while (count--) {
                    *oPtr++ = *iPtr++;
                }
            } else {
                pixel = *iPtr++;
                while (count--) {
                    *oPtr++ = pixel;
                }
            }
        }
    } else {
        fseek(image->file,
              512+(y*image->xsize)+(z*image->xsize*image->ysize),
              SEEK_SET);
        fread(buf, 1, image->xsize, image->file);
    }
}

unsigned * ge_read_rgb( const char * name, int * width, int * height,
                       int * components) {
    unsigned *base, *lptr;
    unsigned char *rbuf, *gbuf, *bbuf, *abuf;
    ImageRec *image;
    int y;

    image = ImageOpen(name);

    if(!image)
        return NULL;
    (*width)=image->xsize;
    (*height)=image->ysize;
    (*components)=image->zsize;
    base = (unsigned *)malloc(image->xsize*image->ysize*sizeof(unsigned));
    rbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    gbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    bbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    abuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    if(!base || !rbuf || !gbuf || !bbuf)
        return NULL;
    lptr = base;
    for(y=0; y<image->ysize; y++) {
        if(image->zsize>=4) {
            ImageGetRow(image,rbuf,y,0);
            ImageGetRow(image,gbuf,y,1);
            ImageGetRow(image,bbuf,y,2);
            ImageGetRow(image,abuf,y,3);
            rgbatorgba(rbuf,gbuf,bbuf,abuf,
              (unsigned char *)lptr,image->xsize);
            lptr += image->xsize;
        } else if(image->zsize==3) {
            ImageGetRow(image,rbuf,y,0);
            ImageGetRow(image,gbuf,y,1);
            ImageGetRow(image,bbuf,y,2);
            rgbtorgba(rbuf,gbuf,bbuf,(unsigned char *)lptr,image->xsize);
            lptr += image->xsize;
        } else {
            ImageGetRow(image,rbuf,y,0);
            bwtorgba(rbuf,(unsigned char *)lptr,image->xsize);
            lptr += image->xsize;
        }
    }
    ImageClose(image);
    free(rbuf);
    free(gbuf);
    free(bbuf);
    free(abuf);

    return (unsigned *) base;
}
