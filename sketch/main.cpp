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


static int win_menu;

// screen height and width
int s_width  = 400;
int s_height = 400;

void mylinefun(int x1, int y1,int x2,int y2, int flag);
void createmymenu(void);



using namespace std;


//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

float _angle = 0.0f;
float _zview = 10.0f;
float yangle = 0;



int allpoints = 0;

class mypoint{
public:
    int x, y, z;
    mypoint(int a, int b, int c);
    mypoint(mypoint *t);
    mypoint();
};


mypoint::mypoint(int a,int b, int c){
    x = a;
    y = b;
    z = c;
}
mypoint::mypoint(mypoint *t){
    x = t->x;
    y = t->y;
    z = t->z;
}
mypoint::mypoint(){
    x = 0;
    y = 0;
    z = 0;
}


class cuboid{
public:
    GLfloat length, breadth, height, shortest, largest;
};

class view{
public:
    int num_cuboid = 2;     // we are starting with only 2 cuboids as input
    int joint_type = 1;     // starting with rotation joint
    
    int cuboid_one[3];
    int cuboid_two[3];
    
    
    view();
};




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


int get_distance(int x1, int x2, int y1, int y2){
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

//              x        y
// returns the length breadth and height of the cuboid given the 5 points
void get_height_width_depth_of_cuboid(){
    int length  = get_distance(allpoints_x[0], allpoints_x[1], allpoints_y[0], allpoints_y[1]);
    int breadth = get_distance(allpoints_x[1], allpoints_x[2], allpoints_y[1], allpoints_y[2]);
    int height  = get_distance(allpoints_x[1], allpoints_x[4], allpoints_y[1], allpoints_y[4]);
    
    cuboid_dimensions[0] = length;
    cuboid_dimensions[1] = breadth;
    cuboid_dimensions[2] = height;
    
    // flag to denote that cuboid input has been taken
    cuboid_taken = 1;
    printf("Cuboid lbh %d %d %d\n", length, breadth, height);
}


void get_3d_from_2d(std::list<mypoint*> allpoints){
    
}


void debugmessage(){
    for (int i=0;i<8;++i){
        printf("%d %d %d\n", countfacepoint, allpoints_x[i], allpoints_y[i]);
    }
}


// stores the point in allpoints_x2 for drawing the back face of cuboid
void get_2nd_face_points(){
    allpoints_x2[0] = allpoints_x[1];
    allpoints_y2[0] = allpoints_y[1];
    
    if(countfacepoint >= 5){
        allpoints_x2[1] = allpoints_x[4];
        allpoints_y2[1] = allpoints_y[4];
    }
    return;
}

void mousemotion(int button, int state, int x, int y){
    printf("%d %d\n", x, y);
    if(state == GLUT_DOWN){
        allpoints_x[countfacepoint] = x;
        allpoints_y[countfacepoint] = y;
        ++countfacepoint;
        if(countfacepoint >= 5){
            get_height_width_depth_of_cuboid();
        }
        
        
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
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0.0f, 400.0, 400.0, 0.0, 0.0, 1.f);
//    
//    
//    glDepthMask(GL_FALSE);      // disable depth writes
//    glEnable( GL_TEXTURE_2D );  // draw background
//    glBindTexture( GL_TEXTURE_2D, texture );
//    
//    glBegin (GL_QUADS);
//    glTexCoord2d(0.0,0.0); glVertex2d(0.0, 0.0);
//    glTexCoord2d(1.0,0.0); glVertex2d(400.0, 0.0);
//    glTexCoord2d(1.0,1.0); glVertex2d(400.0, 400.0);
//    glTexCoord2d(0.0,1.0); glVertex2d(0.0, 400.0);
//    glEnd();
//    
//    // re-enable depth writes
//    glDepthMask(GL_TRUE);
//    
//    // perspective projection for foreground
//    glLoadIdentity();
    
    int back = 1;
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f } };
    glClearColor(colors[back][0], colors[back][1], colors[back][2], 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
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
    gluLookAt(10, 10, _zview, 0, 0, 0, 0, 1, 0);

    
    // Rotating The cuboid
    glPushMatrix();
    glRotatef(-1*_angle*0.5, 0.0f, 0.0f, 1.0f);
    //glTranslatef(2.0f, -4.0f, 0.0f);
    glColor3f(0.9f, 0.9f, 0.9f);
    
    //cuboid_dimensions
    if(cuboid_taken != 1){
        drawmycuboid(6, 4, 1);
    }
    else{
        drawmycuboid(cuboid_dimensions[0]/20.0, cuboid_dimensions[1]/20.0, cuboid_dimensions[2]/20.0);
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
    
    if(countfacepoint <= 4){
      drawinputlines(allpoints_x, allpoints_y, 0, countfacepoint);
    }
    else{
      drawinputlines(allpoints_x, allpoints_y, 0, 4);
      get_2nd_face_points();
      drawinputlines(allpoints_x2, allpoints_y2, 0, countfacepoint-4+1);
    }
    
    
    
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

//int main(int argc, char** argv) {
//    //Initialize GLUT
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//    glutInitWindowSize(400, 400);
//    
//    //Create the window
//    win_menu = glutCreateWindow("Sketch");
//    createmymenu();
//    
//    
//    initRendering();
//    
//    //Set handler functions
//    glutDisplayFunc(drawScene);
//    glutKeyboardFunc(handleKeypress);
//    glutReshapeFunc(handleResize);
//    glutMouseFunc(mousemotion);
//    
//    //Add a timer
//    glutTimerFunc(25, update, 0);
//    glutMainLoop();
//}





