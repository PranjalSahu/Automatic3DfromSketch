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

#define PI 3.14159265
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






//    std::vector<myline*> lt;
//    //lt.push_back(new myline(0, 0, 0, 5));
//    lt.push_back(new myline(0, 5, 6, 9));
//    lt.push_back(new myline(0, 5, 6, 10));
//    lt.push_back(new myline(0, 5, 6, 8));
//    std::sort(lt.begin(), lt.end(), sortbyangleasc(new myline(0, 0, 0, 5)));
//    printf("%d %d %d %d\n", lt[0]->x1, lt[0]->x2, lt[0]->y1, lt[0]->y2);
//    return 0;

//    std::vector<int> a;
//    a.push_back(5);
//    a.push_back(13);
//    a.push_back(17);
//
//    std::sort(a.begin(), a.end(), sorter_check(10));
//
//    printf("%d %d %d\n", a[0], a[1], a[2]);
//    //a.sort(sorter_check(10));
//
//    return 0;

//            std::vector<i2tuple> lp;
//            std::vector<i2tuple>::iterator iter = lp.begin();
//
//    lp.push_back(i2tuple(1, 1));
//    lp.push_back(i2tuple(1, 6));
//    lp.push_back(i2tuple(1, 8));
//    lp.push_back(i2tuple(1, 9));
//    iter = lp.begin();
//    ++iter;
//
//    lp[iter-lp.begin()] = i2tuple(100,100);
//    printf("pranal tsting");

//        std::vector<int> lp;
//        std::vector<int>::iterator iter = lp.begin();
//
//        lp.push_back(5);
//        lp.push_back( 6);
//        lp.push_back( 7);
//        lp.push_back( 8);
//        lp.push_back( 9);
//        lp.push_back( 10);
//
//        iter = lp.begin();
//
//        std::vector<int>::iterator itera = iter;
//        itera = itera+1;
//    printf("%d ", *itera);
//        iter = lp.erase(iter);
//
//    printf("%d\n", iter-lp.begin());
//    printf("%d\n", itera-lp.begin());
//    printf("%d ", *iter);
//    printf("%d ", *itera);

//        iter  = lp.begin();
//        iter++;
//
//        lp.erase(iter+3);
//        lp.erase(iter);
//
//        lp.insert(iter, 100);
//
//        iter++;
//        printf("%d\n", *iter);
//        iter++;
//        printf("%d\n", *iter);
//        iter++;
//        printf("%d\n", *iter);
//        iter++;
//        printf("%d\n", *iter);

//
//        printf("%d \n", *iter);
//        iter++;
//        printf("%d \n", *iter);
//        iter++;
//        printf("%d \n", *iter);
//        iter++;
//        printf("%d \n", *iter);
//        iter++;
//    printf("%d \n", *iter);
//    iter++;
//    if(iter == lp.end()){
//        printf("pranjal");
//    }
//    printf("%d \n", *iter);
//    iter++;
//    printf("%d \n", *iter);
//    iter++;
//    printf("%d \n", *iter);
//    iter++;

