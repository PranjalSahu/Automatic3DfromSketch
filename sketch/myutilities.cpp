//
//  myutilities.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/23/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include "myline.h"


int mymin(int a, int b){
    return (a < b ? a : b);
}

float mymin(float a, float b){
    return (a < b ? a : b);
}

int mymax(int a, int b){
    return (a > b ? a : b);
}

float mymax(float a, float b){
    return (a > b ? a : b);
}

// returns distance between two points
float get_point_distance(int x1, int y1, int x2, int y2){
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

