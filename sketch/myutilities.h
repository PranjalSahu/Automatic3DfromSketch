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

#include <glm/vec2.hpp> // glm::vec4
#include <glm/vec3.hpp> // glm::vec4
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>


#define PI 3.14159265
#define MY_GLM_EPS 0.001

typedef std::tuple<int,int> i2tuple;

int mymin(int a, int b);
float mymin(float a, float b);
int mymax(int a, int b);
float mymax(float a, float b);
float get_point_distance(int x1, int y1, int x2, int y2);
#endif

