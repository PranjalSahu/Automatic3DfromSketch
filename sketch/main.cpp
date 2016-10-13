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


// Use it for loading texture to show in background for taking user input
//void display (void) {
//    
//    glClearColor (0.0,0.0,0.0,1.0);
//    glClear (GL_COLOR_BUFFER_BIT);
//    
//    // background render
//    
//    glOrtho(0.0f, 1024.0, 512.0, 0.0, 0.0, 1.f);
//    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    
//    glEnable( GL_TEXTURE_2D );
//    
//    glBindTexture( GL_TEXTURE_2D, texture );
//    
//    glBegin (GL_QUADS);
//    glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
//    glTexCoord2d(1.0,0.0); glVertex2d(1024.0,0.0);
//    glTexCoord2d(1.0,1.0); glVertex2d(1024.0,512.0);
//    glTexCoord2d(0.0,1.0); glVertex2d(0.0,512.0);
//    glEnd();
//    
//    // foreground render - added code, not working
//    
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    
//    glColor3f(0.0f, 0.0f, 1.0f);
//    
//    glBegin (GL_QUADS);
//    glVertex2d(500.0,400.0);
//    glVertex2d(500.0,500.0);
//    glVertex2d(600.0,400.0);
//    glVertex2d(600.0,500.0);
//    glEnd();
//    
//    glutSwapBuffers();
//}
//


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
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

float _angle = 0.0f;
float yangle = 0;

// TODO: function to add an arbitrary cuboid to the scene
void addcuboid(GLfloat dimensions[][3] , GLfloat corner[]){
    
}

//Draws the 3D scene
void drawScene() {
    int back = 1;
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f } };
    glClearColor(colors[back][0], colors[back][1], colors[back][2], 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(0.0f, 0.0f, -8.0f);
    
    //Add ambient light
    //GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    
    //Add positioned light
    //GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Color (0.5, 0.5, 0.5)
    //GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f};   // Positioned at (4, 0, 8)
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    
    //Add directed light
    //GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    //GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    //glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    
    gluLookAt(10, 10, 10, 3, 0, -3, 0, 1, 0);

    
    // Rotating The cuboid
    glPushMatrix();
    //glTranslatef(2.0f,-2.0f, 0.0f);
    glRotatef(-1*_angle*0.5, 0.0f, 0.0f, 1.0f);
    glTranslatef(-2.0f, 2.0f, 0.0f);
    //Colour of the object
    glColor3f(0.9f, 0.9f, 0.9f);
    glutSolidCube(4);
    glPopMatrix();
    
    
    
    
    
    //glutSolidCube(5);
    
    // Drawing points at corners of the cuboid
    glPointSize( 6.0 );
    glBegin( GL_POINTS );
    glColor3f( 0.95f, 0.207, 0.031f );
    GLfloat mycolors[5][3] = { { 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f} };
    GLfloat mypoints[5][3] = { { -2.0f, -1.0f, 2.0f}, {2.0f, -1.0f, 2.0f}, {2.0f, 1.0f, 2.0f}, {-2.0f, 1.0f, 2.0f}, {2.0f, -1.0f, -2.0f} };
    
    for ( int i = 0; i < 5; ++i ){
        glColor3f( mycolors[i][0], mycolors[i][1], mycolors[i][2]);
        glVertex3f(mypoints[i][0], mypoints[i][1], mypoints[i][2]);
    }
    glEnd();
    
    
    
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
    
    
    
    
    
    
    glutSwapBuffers();
}

void update(int value) {
    _angle += 2.0f;
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












//bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)
//
//bool movingUp = false; // Whether or not we are moving up or down
//float yLocation = 0.0f; // Keep track of our position on the y axis.
//
//float yRotationAngle = 0.0f; // The angle of rotation for our object
//
//void keyOperations (void) {
//    if (keyStates[GLUT_KEY_LEFT]) { // If the left arrow key has been pressed
//        // Perform left arrow key operations
//    }
//}
//
//void display (void) {
//    keyOperations();
//    
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear the background of our window to red
//    glClear(GL_COLOR_BUFFER_BIT); //Clear the colour buffer (more buffers later on)
//    glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations
//    
//    glTranslatef(0.0f, 0.0f, -5.0f); // Push eveything 5 units back into the scene, otherwise we won't see the primitive
//    
//    glTranslatef(0.0f, yLocation, 0.0f); // Translate our object along the y axis
//    
//    glRotatef(yRotationAngle, 0.0f, 1.0f, 0.0f); // Rotate our object around the y axis
//    
//    glutWireCube(2.0f); // Render the primitive
//    
//    glFlush(); // Flush the OpenGL buffers to the window
//    
//    if (movingUp) // If we are moving up
//        yLocation -= 0.005f; // Move up along our yLocation
//    else  // Otherwise
//        yLocation += 0.005f; // Move down along our yLocation
//    
//    if (yLocation < -3.0f) // If we have gone up too far
//        movingUp = false; // Reverse our direction so we are moving down
//    else if (yLocation > 3.0f) // Else if we have gone down too far
//        movingUp = true; // Reverse our direction so we are moving up
//    
//    yRotationAngle += 0.005f; // Increment our rotation value
//    
//    if (yRotationAngle > 360.0f) // If we have rotated beyond 360 degrees (a full rotation)
//        yRotationAngle -= 360.0f; // Subtract 360 degrees off of our rotation
//}
//
//void reshape (int width, int height) {
//    glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
//    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
//    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
//    
//    gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
//    
//    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
//}
//
//void keyPressed (unsigned char key, int x, int y) {
//    keyStates[key] = true; // Set the state of the current key to pressed
//}
//
//void keyUp (unsigned char key, int x, int y) {
//    keyStates[key] = false; // Set the state of the current key to not pressed
//}
//
//int main (int argc, char **argv) {
//    glutInit(&argc, argv); // Initialize GLUT
//    glutInitDisplayMode (GLUT_SINGLE); // Set up a basic display buffer (only single buffered for now)
//    glutInitWindowSize (500, 500); // Set the width and height of the window
//    glutInitWindowPosition (100, 100); // Set the position of the window
//    glutCreateWindow ("Your first OpenGL Window"); // Set the title for the window
//    
//    glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering
//    
//    glutIdleFunc(display); // Tell GLUT to use the method "display" as our idle method as well
//    
//    glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping
//    
//    glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses
//    glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events  
//    
//    glutMainLoop(); // Enter GLUT's main loop  
//}



