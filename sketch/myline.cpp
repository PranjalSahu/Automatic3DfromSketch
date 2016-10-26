//
//  myline.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/23/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef MYLINE_H
#define MYLINE_H

#include <stdio.h>
#include "myline.h"
#include <math.h>
#include "myutilities.h"
#include <opencv2/opencv.hpp>
#include <vector>



myline::myline(i2tuple a, i2tuple b){
    x1 = std::get<0>(a);
    y1 = std::get<1>(a);
    x2 = std::get<0>(b);
    y2 = std::get<1>(b);
    
    if (x1 == x2){
        m = infslope;
    }
    else{
        printf("debug message  is %d %d %d %d\n", x1, y1,  x2, y2);
        m = (y2-y1)*1.0/(x2-x1);
    }
    printf("slope is a %f\n", m);
}

myline::myline(int a,int b, float ma){
    x1 = a;
    y1 = b;
    
    // when x2, y2 anre not known
    x2 = infvalue;
    y2 = infvalue;
    
    // special treatment to infinite slope
    if(ma > infslope){
        m = infslope;
    }
    else{
        m = ma;
    }
}
myline::myline(myline *t){
    x1 = t->x1;
    y1 = t->y1;
}
myline::myline(){
    x1 = 0;
    y1 = 0;
}
myline::myline(int a1, int b1, int c1, int d1){
    x1 = a1;
    x2 = b1;
    y1 = c1;
    y2 = d1;
    
    if (x1 == x2){
        m = infslope;
    }
    else{
        m = (y2-y1)*1.0/(x2-x1);
    }
}

int myline::checkpointlies(int x, int y){
    // special handling for infinite slope
    if (m == infslope){
        if (y == y1){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        // line equation in point slope form
        float lhs = y-y1;
        float rhs = m*(x-x1);
        
        if(abs(lhs-rhs) < MY_EPS)
            return 1;
        else
            return 0;
    }
    return 0;
}


// returns the count of point lying on this line segment
std::vector<i2tuple>  myline::pointliecount(std::vector<i2tuple> points_vector){
    int lying_count = 0;
    std::vector<i2tuple> lying_vector;
    
    for(std::vector<i2tuple>::iterator it = points_vector.begin(); it != points_vector.end(); ++it){
        i2tuple pt = *it;
        int x = std::get<0>(pt);
        int y = std::get<1>(pt);
        
        if ( this->get_perpendicular_distance(pt) <= 4){
            lying_count = lying_count+1;
            lying_vector.push_back(i2tuple(x, y));
        }
    }
    
    return lying_vector;
}


int myabs(int a){
    return (a > 0 ? a : -1*a);
}

// returns 1 if two lines are mergeable
int myline::mergeable(myline *t){
    // only slope and one point are known
    if(x2 == infvalue && y2 == infvalue){
        if(x1 == t->x1 || myabs(y1-t->y1) == 1){
            x2 = t->x2;
            y2 = t->y2;
        }else{
            
        }
    }
    // when both points are known
    else{
        
    }
    return 0;
}

void myline::mergelines(myline *t){
    // case of infinite slope
    if(t->x1 == this->x1){
        m = infslope;
    }
    // case of 0 slope
    else if(t->y1 == this->y1){
        m = 0;
    }
    else{
        // update the slope taking the 2nd point
        m = (t->y1-this->y1)/(t->x1-this->x1);
    }
}

float myline::get_line_length() const{
    if(x2 == infvalue)
        return 1;
    else{
        return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    }
}

// only call when line's both points are known
float myline::get_perpendicular_distance(i2tuple a){
    int x_0 = std::get<0>(a);
    int y_0 = std::get<1>(a);
    
    float dist = abs((y2-y1)*x_0-(x2-x1)*y_0+x2*y1-y2*x1)/sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
    return dist;
}


float myline::get_distance(const myline *ml) const {
    // when both are just points
    if(x2 == infvalue && ml->x2 == infvalue){
        // distance between two points
        return get_point_distance(x1, y1, ml->x1, ml->y1);
    }
    // when one is line and other is point
    else if(x2 == infvalue){
        float a = get_point_distance(x1, y1, ml->x1, ml->y1);
        float b = get_point_distance(x1, y1, ml->x2, ml->y2);
        return mymin(a, b);
    }
    else if(ml->x2== infvalue){
        float a = get_point_distance(x1, y1, ml->x1, ml->y1);
        float b = get_point_distance(x2, y2, ml->x1, ml->y1);
        return mymin(a, b);
    }
    // when both are line segments
    else{
        float a = get_point_distance(x1, y1, ml->x1, ml->y1);
        float b = get_point_distance(x1, y1, ml->x2, ml->y2);
        float c = get_point_distance(x2, y2, ml->x1, ml->y1);
        float d = get_point_distance(x2, y2, ml->x2, ml->y2);
        return mymin(mymin(a, b), mymin(c, d));
    }
    
    return 1000;
}

int myline::get_intersection_count(cv::Mat &im){
    
    return 0;
}
#endif

