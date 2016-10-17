/*
 * file: pngDrvr.c
 * author: mnorton
 *
 * description:
 * This program is the example driver code for the pngLoad.c
 * library. The program uses a modified version of Example 9-1
 * source code, from Chapter 9, Texture Mapping, from
 * The OpenGL Programming Guide v1.2 (aka the Red Book).
 * This source code also implements components of code found
 * Nate Miller's OpenGL Texture Mapping: An Example.
 *
 * This example loads a sample 256 x 256 pixel PNG file and
 * displays it in an OpenGL application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>

#include "pngLoad.h"

int setupGLTexture(unsigned char *image, int width,  int height, int id);
void display(void);

static GLuint texName;

int kWindowWidth = 256; /* window width */
int kWindowHeight = 256; /* window height */

/*
 * void init void
 * Load the PNG into an image buffer.
 * Set up OpenGL texture using image buffer.
 */
void init(void)
{
    
    int result;
    int *width, *height;
    unsigned char *image;
    char *filename = "texture.png";
    
    glEnable (GL_DEPTH_TEST);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    
    /* read the PNG file using pngLoad
     * raw data from PNG file is in image buffer
     */
    result = pngLoad(filename, &width, &height, &image);
    if (result == 0)
    {
        printf("(pngLoad) %s FAILED.\n", filename);
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    /* Namimg a texture object -see Chapter 9,
     * -Red Book
     */
    glGenTextures(1, &texName);
    
    /* take the raw image and put it into an
     * OpenGL 2D texture.
     */
    setupGLTexture(image, *width, *height, texName);
    
}

/* int setupGLTexture char* int int int
 * Function loads image from buffer into
 * OpenGL texture.
 */
int setupGLTexture(unsigned char *image, int width, int height, int texName)
{
    
    if (image == NULL)
    {
        return 0;
    }
    
    printf("(loadTexture) width: %d height: %d\n", width, height);
    
    /* create a new texture object
     * and bind it to texname (unsigned integer > 0)
     */
    glBindTexture(GL_TEXTURE_2D, texName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image);
    
    return 1;
}

/*
 * void display void
 * Callback function registered to glutDisplayFunc
 */
void display(void)
{
    
    if (!kWindowHeight)
        return;
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef (0, 0, -20); /* eye position */
    
    glEnable (GL_TEXTURE_2D); /* enable texture mapping */
    glBindTexture (GL_TEXTURE_2D, texName); /* bind to our texture*/
    
    /* setup texture map using vertices from OpenGL Texture Mapping,
     * an Introduction, by Nate Miller.This image buffer has a 256 x 256
     * pixel image.
     * Our PNG image is 256 x 256.
     */
    glBegin (GL_QUADS);
    glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
    glVertex3f (-10.0f, -10.0f, 0.0f);
    glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
    glVertex3f (10.0f, -10.0f, 0.0f);
    glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
    glVertex3f (10.0f, 10.0f, 0.0f);
    glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
    glVertex3f (-10.0f, 10.0f, 0.0f);
    glEnd ();
    
    glDisable(GL_TEXTURE_2D); /* disable texture mapping */
    glutSwapBuffers();
}

/*
 * void resize int int
 * Callback function registered to glutReshapeFunc
 */
void resize (int w, int h)
{
    /* window has been resized */
    kWindowWidth = w;
    kWindowHeight = h;
    
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    /*
     * gluPerspective fovy aspect near far
     *
     * GLdouble fovy   - angle of the field of view
     *                   in the yz plane [range 0.0 - 180.0]
     * GLdouble aspect - aspect ratio of frustrum width/height
     * GLdouble near   - distance to the view plane along -z axis
     * GLdouble far    - distance to the clipping plane along -z axis
     *
     * see Chapter 3, Viewing, OpenGL Programming Guide -Red Book.
     * see Chapter 5, Viewing, Interactive Computer Graphics
     */
    gluPerspective(55, (GLfloat) w / (GLfloat) h, 1, 9999);
    glutPostRedisplay();
}

/*
 * void keyboard uchar int int
 * Callback function registered to glutKeyboardFunc
 */
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:    /* ESC key */
        case 'Q':   /* Q|q keys */
        case 'q':
            exit(0);
            break;
        default:
            break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow("PNG Texture Example");
    
    /* glut drivers from source Example 9-1, Texture Mapping 
     * -RedBook
     */
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    
    return 0;
}