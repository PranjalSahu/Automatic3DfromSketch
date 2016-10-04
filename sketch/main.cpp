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



//void renderScene(void) {
//    
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    
//    glBegin(GL_TRIANGLES);
//    glVertex3f(-0.5,-0.5,0.0);
//    glVertex3f(0.5,0.0,0.0);
//    glVertex3f(0.0,0.5,0.0);
//    glEnd();
//    
//    glutSwapBuffers();
//}
//
//int main(int argc, char **argv) {
//    
//    
//
//    // init GLUT and create Window
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
//    glutInitWindowPosition(100,100);
//    glutInitWindowSize(320,320);
//    glutCreateWindow("Lighthouse3D - GLUT Tutorial");
//    
//    // register callbacks
//    glutDisplayFunc(renderScene);
//    
//    
//    // get opengl version
//    // printf("%s", glGetString(GL_VERSION));
//
//    // enter GLUT event processing cycle
//    glutMainLoop();
//    
//    return 1;
//}


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
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

float _angle = -70.0f;

//Draws the 3D scene
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(0.0f, 0.0f, -8.0f);
    
    //Add ambient light
    GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    
    //Add positioned light
    GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    
    //Add directed light
    GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    
    glRotatef(_angle, 0.0f, 1.0f, 1.0f);
    //Set the colour here
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    
    //Front
    glNormal3f(0.0f, 0.0f, 1.0f);
    //glNormal3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(-1.5f, -1.0f, 1.5f);
    //glNormal3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.5f, -1.0f, 1.5f);
    //glNormal3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.5f, 1.0f, 1.5f);
    //glNormal3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(-1.5f, 1.0f, 1.5f);
    
    //Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    //glNormal3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.5f, -1.0f, -1.5f);
    //glNormal3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.5f, 1.0f, -1.5f);
    //glNormal3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.5f, 1.0f, 1.5f);
    //glNormal3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.5f, -1.0f, 1.5f);
    
    //Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    //glNormal3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.5f, -1.0f, -1.5f);
    //glNormal3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.5f, 1.0f, -1.5f);
    //glNormal3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.5f, 1.0f, -1.5f);
    //glNormal3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.5f, -1.0f, -1.5f);
    
    //Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    //glNormal3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.5f, -1.0f, -1.5f);
    //glNormal3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(-1.5f, -1.0f, 1.5f);
    //glNormal3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(-1.5f, 1.0f, 1.5f);
    //glNormal3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.5f, 1.0f, -1.5f);
    
    glEnd();
    
    glutSwapBuffers();
}

void update(int value) {
    _angle += 1.5f;
    if (_angle > 360) {
        _angle -= 360;
    }
    
    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
    //Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    
    //Create the window
    glutCreateWindow("Rotating 3D Cuboid");
    initRendering();
    
    //Set handler functions
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    
    //Add a timer
    glutTimerFunc(25, update, 0);
    
    glutMainLoop();
}



