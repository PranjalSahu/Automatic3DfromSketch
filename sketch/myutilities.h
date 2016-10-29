//
//  utilities.h
//  sketch
//
//  Created by Pranjal Sahu on 10/19/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//
#ifndef MYUTILITIES_H
#define MYUTILITIES_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>


#define IMG_SCALE 1.0

typedef std::tuple<int,int> i2tuple;

class mypoint{
public:
    GLfloat x, y, z;
    mypoint(GLfloat a, GLfloat b, GLfloat c);
    mypoint(mypoint *t);
    mypoint();
};



int mymin(int a, int b);
float mymin(float a, float b);
int mymax(int a, int b);
float mymax(float a, float b);
float get_point_distance(int x1, int y1, int x2, int y2);


#endif