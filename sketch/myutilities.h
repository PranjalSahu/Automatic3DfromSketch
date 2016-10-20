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

class mypoint{
public:
    GLfloat x, y, z;
    mypoint(GLfloat a, GLfloat b, GLfloat c);
    mypoint(mypoint *t);
    mypoint();
};




#endif