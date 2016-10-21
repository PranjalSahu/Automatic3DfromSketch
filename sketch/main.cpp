//
//  main.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/4/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//


#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <queue>
#include <list>
#include <vector>
#include <fstream>

#include <IL/il.h>
#include <GLUT/glut.h>

#include "cuboid.h"
#include "view.h"

static int win_menu;

// screen height and width
int s_width  = 600;
int s_height = 600;
GLuint texid;
float cuboid_ratio = 107.0;

float _zview = 2.0f;
float _xview = 2.0f;
float _yview = 2.0f;
float angle_amount = 0.05;


void mylinefun(int x1, int y1,int x2,int y2, int flag);
void createmymenu(void);



using namespace std;



//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
        case 'z':
            _zview = _zview+angle_amount;
            printf("z angle is %f\n", _zview);
            break;
        case 'x':
            _xview = _xview+angle_amount;
            printf("x angle is %f\n", _xview);
            break;
        case 'y':
            _yview = _yview+angle_amount;
            printf("y angle is %f\n", _yview);
            break;
        case 'a':
            _zview = _zview-angle_amount;
            printf("z angle is %f\n", _zview);
            break;
        case 's':
            _xview = _xview-angle_amount;
            printf("x angle is %f\n", _xview);
            break;
        case 'd':
            _yview = _yview-angle_amount;
            printf("y angle is %f\n", _yview);
            break;
        case 27: //Escape key
            exit(0);
    }
}

//Initializes 3D rendering
void initRendering() {
    glEnable( GL_POINT_SMOOTH );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); //Enable lighting
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Automatically normalize normals
    
    //glShadeModel(GL_SMOOTH); //Enable smooth shading
}

//Called when the window is resized
void handleResize(int w, int h) {
    s_width  = w;
    s_height = h;
    
    glViewport(0, 0, w, h);
    glEnable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

float _angle = 0.0f;
float yangle = 0;



int allpoints = 0;


// the current view whose input is being taken
view* current_view;
int view_index = 0;



std::list<mypoint*> face1;


// for storing the input given by user
// These points will be used to get 3d points and a 3d object will be drawn on screen
int countfacepoint = 0;
int allpoints_x[8];
int allpoints_y[8];

// for taking points for width part
int allpoints_x2[8];
int allpoints_y2[8];

int cuboid_taken = 0;     // flag to denote if cuboid's input has been taken or not
int facetaken = 0;        // flag to denote if one face input has been taken or not



// stores the length, breadth and height of the cuboid obtained after taking input
int cuboid_dimensions[3];



void get_3d_from_2d(std::list<mypoint*> allpoints){
    
}


void debugmessage(){
    for (int i=0;i<8;++i){
        printf("%d %d %d\n", countfacepoint, allpoints_x[i], allpoints_y[i]);
    }
}


void mousemotion(int button, int state, int x, int y){
    printf("%d %d\n", x, y);
    if(state == GLUT_DOWN){
        // create a cuboid if currently no cuboid is present
        cuboid* c = NULL;
        if(current_view->num_cuboid == 0){
            c = current_view->create_cuboid(1);
        }
        else{
            c = current_view->get_current_cuboid();
            if(c->input_done == 1){                     // create a new cuboid after the 7th click
                c = current_view->create_cuboid(0);      // later we will provide GUI to take user option to create a new cuboid
            }
        }
        
        // insert the point for that cuboid
        c->insert_mouse_point(x, y);
        
        //debugmessage();
    }
}


// given the count it draws the points taken from array point_x and point_y
// takes the starting index from where to start taking points from array
void drawinputlines(int point_x[], int point_y[], int start, int count){
    glBegin(GL_LINE_LOOP);
    for(int i=start;i<count;++i){
        glVertex2f(point_x[i], point_y[i]);
    }
    glEnd();
}


//void drawmycuboid_coord(int cuboid_ratio){
//    int world_x[8];
//    int world_y[8];
//    int world_z[8];
//    
//    world_x[0] = 0; world_x[1] = 1;
//    glBegin(GL_QUADS);
//    glNormal3f(0.0f, 0.0f, 1.0f);
//    
//    // Front face 0, 1, 2, 3
//    glNormal3f(1.0f, 0.0f, 0.0f);
//    glVertex3f(world_x[0], world_x[0], world_x[0]);
//    glVertex3f(world_x[1], world_x[1], world_x[1]);
//    glVertex3f(world_x[2], world_x[2], world_x[2]);
//    glVertex3f(world_x[3], world_x[3], world_x[3]);
//    
//    // Back face 4, 5, 6, 7
//    glNormal3f(-1.0f, 0.0f, 0.0f);
//    glVertex3f(world_x[4], world_x[4], world_x[4]);
//    glVertex3f(world_x[5], world_x[5], world_x[5]);
//    glVertex3f(world_x[6], world_x[6], world_x[6]);
//    glVertex3f(world_x[7], world_x[7], world_x[7]);
//    
//    // Side 0, 4, 5, 1
//    glNormal3f(0.0f, 1.0f, 0.0f);
//    glVertex3f(world_x[0], world_x[0], world_x[0]);
//    glVertex3f(world_x[4], world_x[4], world_x[4]);
//    glVertex3f(world_x[5], world_x[5], world_x[5]);
//    glVertex3f(world_x[1], world_x[1], world_x[1]);
//    
//    // Side 3, 7, 6, 2
//    glNormal3f(0.0f, 1.0f, 0.0f);
//    glVertex3f(world_x[3], world_x[3], world_x[3]);
//    glVertex3f(world_x[7], world_x[7], world_x[7]);
//    glVertex3f(world_x[6], world_x[6], world_x[6]);
//    glVertex3f(world_x[2], world_x[2], world_x[2]);
//    
//    // Top 1, 5, 6, 2
//    glVertex3f(world_x[1], world_x[1], world_x[1]);
//    glVertex3f(world_x[5], world_x[5], world_x[5]);
//    glVertex3f(world_x[6], world_x[6], world_x[6]);
//    glVertex3f(world_x[2], world_x[2], world_x[2]);
//    
//    // HIDDEN
//    //    glVertex3f(0.0f, 0.0f, -1*b);
//    //    glVertex3f(0.0f, l, -1*b);
//    //    glVertex3f(-1*h, l, -1*b);
//    //    glVertex3f(-1*h, 0.0f, -1*b);
//    
//    glEnd();
//    return;
//}


void drawmycuboid(GLfloat l, GLfloat b, GLfloat h){
    glBegin(GL_QUADS);
    
    glNormal3f(0.0f, 0.0f, 1.0f);
    
    // THIS CUBOID IS OF DIMENSION 6 x 4 x 2
    // FRONT
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(0.0f, l, -1*b);
    glVertex3f(0.0f, 0.0f, -b);
    
    // SHIFT FRONT BY 2 IN -X TO GET BACK
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(-1*h, 0.0f, -1*b);
    
    // TOP
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(0.0f, l, -1*b);
    
    // SHIFT TOP BY -6 IN Y TO GET BOTTOM
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, -1*b);
    glVertex3f(0.0f, 0.0f, -1*b);
    
    // OPPOSITE HIDDEN
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    
    // HIDDEN
    glVertex3f(0.0f, 0.0f, -1*b);
    glVertex3f(0.0f, l, -1*b);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(-1*h, 0.0f, -1*b);
    
    glEnd();
    return;
}



//Draws the 3D scene
void drawScene() {
    
    // clear color/depth buffer
    
    // orthographic projection for background
//        glMatrixMode(GL_PROJECTION);
//        glLoadIdentity();
//        glOrtho(0.0f, 400.0, 400.0, 0.0, 0.0, 1.f);
//    
//    
//        glDepthMask(GL_FALSE);      // disable depth writes
//        glEnable( GL_TEXTURE_2D );  // draw background
//        glBindTexture( GL_TEXTURE_2D, texid );
//    
//        glBegin (GL_QUADS);
//        glTexCoord2d(0.0,0.0); glVertex2d(0.0, 0.0);
//        glTexCoord2d(1.0,0.0); glVertex2d(400.0, 0.0);
//        glTexCoord2d(1.0,1.0); glVertex2d(400.0, 400.0);
//        glTexCoord2d(0.0,1.0); glVertex2d(0.0, 400.0);
//        glEnd();
//    
//        // re-enable depth writes
//        glDepthMask(GL_TRUE);
//    
//        // perspective projection for foreground
//        glLoadIdentity();
    
    int back = 1;
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f } };
    glClearColor(colors[back][0], colors[back][1], colors[back][2], 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 400.0 / 400.0, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(0.0f, 0.0f, -2.0f);
    
    //Add ambient light
    //GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    
    //Add positioned light
    //GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Color (0.5, 0.5, 0.5)
    //GLfloat lightPos0[] = {4.0f, 0.0f, 2.0f, 1.0f};   // Positioned at (4, 0, 8)
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    
    //Add directed light
    //GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    //GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    //glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    
    //gluLookAt(10, 10, 10, 3, 0, -3, 0, 1, 0);
    //_zview = _zview-0.05;
    gluLookAt(_xview, _yview, _zview, 0, 0, 0, 0, 1, 0);
    
    
    // Rotating The cuboid
    glPushMatrix();
    glRotatef(-1*_angle*0.5, 0.0f, 0.0f, 1.0f);
    //glTranslatef(2.0f, -4.0f, 0.0f);
    glColor3f(0.9f, 0.9f, 0.9f);
    
    //cuboid_dimensions
    //
    cuboid *current_cuboid = current_view->get_current_cuboid();
    
    if(current_cuboid == NULL || current_view->get_done_cuboids() <= 0){
        drawmycuboid(100/cuboid_ratio, 80/cuboid_ratio, 20/cuboid_ratio);
    }
    else{
//        if(current_view->num_cuboid > 1){
//            cuboid* cs = current_view->cuboids[1];
//            if(cs->input_done == 1){
//                current_cuboid = cs;
//                current_cuboid->drawmycuboid_coord(cuboid_ratio);
//            }
//        }
//        else{
            for (int i=0;i<current_view->num_cuboid;++i){
                cuboid* cs = current_view->cuboids[i];
                if(cs->input_done == 1){
                    current_cuboid = cs;
                    current_cuboid->drawmycuboid_coord(cuboid_ratio);
                }
            }
        //}
        //cuboid *current_cuboid = current_view->get_current_cuboid();
        //drawmycuboid(6, 4, 2);
    }
    
    //drawcuboid();
    glPopMatrix();
    
    // Drawing coordinate axes
    glLineWidth(2.5);
    glColor3f(1.0, 1.0, 0.0);
    
    glBegin(GL_LINES);
    //    glColor3f(1, 0, 0);
    //    glVertex3f(-10, -2, -20);
    //    glVertex3f(10, -2, -20);
    //
    //    glColor3f(0, 1, 0);
    //    glVertex3f(0, -10, -20);
    //    glVertex3f(0, 10, -20);
    
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, -10);
    glVertex3f(0, 0, 10);
    glEnd();
    
    
    
    //Setup for 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, s_width, s_height, 0.0, -1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    glLoadIdentity();
    glDisable(GL_CULL_FACE);
    
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(colors[back][0], colors[back][1], colors[back][2], 1.0f);
    
    
    /* Draw a quad */
//    glBegin(GL_QUADS);
//    glTexCoord2i(0, 0); glVertex2i(0,   0);
//    glTexCoord2i(0, 1); glVertex2i(0,   200);
//    glTexCoord2i(1, 1); glVertex2i(200, 200);
//    glTexCoord2i(1, 0); glVertex2i(200, 0);
//    glEnd();
    
    
    current_view->drawinputlinesforaview();
    
    
    // Making sure we can render 3d again
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    
    //glutSolidCube(5);
    
    // Drawing points at corners of the cuboid
    //    glPointSize( 6.0 );
    //    glBegin( GL_POINTS );
    //    glColor3f( 0.95f, 0.207, 0.031f );
    //    GLfloat mycolors[5][3] = { { 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f} };
    //    GLfloat mypoints[5][3] = { { -2.0f, -1.0f, 2.0f}, {2.0f, -1.0f, 2.0f}, {2.0f, 1.0f, 2.0f}, {-2.0f, 1.0f, 2.0f}, {2.0f, -1.0f, -2.0f} };
    //
    //    for ( int i = 0; i < 5; ++i ){
    //        glColor3f( mycolors[i][0], mycolors[i][1], mycolors[i][2]);
    //        glVertex3f(mypoints[i][0], mypoints[i][1], mypoints[i][2]);
    //    }
    //    glEnd();
    
    
    
    
    
    
    
    
    
    glutSwapBuffers();
}

void reset_view(){
    
}

void menufun(int value){
    int count = 0;
    
    if(value == 0){
        glutDestroyWindow(win_menu);
        exit(0);
    }
    else if(value == 1){		// take new view from user
        reset_view();
    }
}


void createmymenu(void){
    int polygonmenu = glutCreateMenu(menufun);
    glutAddMenuEntry("Add New View", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}



void update(int value) {
    //_angle += 2.0f;
    //    if (_angle > 360) {
    //        _angle -= 360;
    //    }
    
    yangle += 0.1;
    //if (yangle > 20){
    //    yangle -=20;
    //}
    
    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}


/* Load an image using DevIL and return the devIL handle (-1 if failure) */
int LoadImagea(char *filename)
{
    ILboolean success;
    ILuint image;
    
    ilGenImages(1, &image); /* Generation of one image name */
    ilBindImage(image); /* Binding of image name */
    success = ilLoadImage(filename); /* Loading of the image filename by DevIL */
    
    if (success) /* If no error occured: */
    {
        /* Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
        success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        
        if (!success)
        {
            return -1;
        }
    }
    else
        return -1;
    
    return image;
}


void setup_view(){
    // instantiate a new view
    current_view = new view();
}

int main(int argc, char** argv) {
    
    
    
    setup_view();
    
        
    
    
    //Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    
    //Create the window
    win_menu = glutCreateWindow("Sketch");
    createmymenu();
    
    
    initRendering();
    
    //Set handler functions
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mousemotion);
    
    //Add a timer
    glutTimerFunc(25, update, 0);
    
    
    
    
    int    image;
    
    /* Initialization of DevIL */
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION){
        printf("wrong DevIL version \n");
        return -1;
    }
    ilInit();
    /* load the file picture with DevIL */
    image = LoadImagea("/Users/pranjal/Downloads/oven.png");
    if ( image == -1 )
    {
        printf("Can't load picture file %s by DevIL \n", argv[1]);
        return -1;
    }
    
    /* OpenGL texture binding of the image loaded by DevIL  */
    glGenTextures(1, &texid); /* Texture name generation */
    glBindTexture(GL_TEXTURE_2D, texid); /* Binding of texture name */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData()); /* Texture specification */
    
    

    
    glutMainLoop();
    
    
    
    
    glDeleteTextures(1, &texid);
    return 0;
}





