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



// ascending
// sorts the lines by the angle which they make from the base line
// will be used for getting the next adjacent line
struct sortbyangleasc : std::binary_function<myline*,myline*,bool> {
    sortbyangleasc( myline* base ) : m_base( base ) {}
    
    float get_angle(const myline* c1, const myline* m_base){
        int x1 = c1->x2-c1->x1;
        int y1 = c1->y2-c1->y1;
        float mod_1 = sqrt(x1*x1+y1*y1);
        
        int x3 = m_base->x2-m_base->x1;
        int y3 = m_base->y2-m_base->y1;
        float mod_3 = sqrt(x3*x3+y3*y3);
        
        int   dot1 = x1*x3+y1*y3;
        float den1 = mod_1*mod_3;
        float a1   = acos (dot1/den1) * 180.0 / PI;
        
        return a1;
    }
    
    bool operator()( const myline* c1, const myline* c2 )
    {
        float a1 = get_angle(c1, m_base);
        float a2 = get_angle(c2, m_base);
        // check which angle is more
        return a1 < a2;
    }
private:
    myline* m_base;
};

//descending
// sorts the lines by the angle which they make from the base line
// will be used for getting the next adjacent line
struct sortbyangledesc : std::binary_function<myline*,myline*,bool> {
    sortbyangledesc( myline* base ) : m_base( base ) {}
    
    float get_angle(const myline* c1, const myline* m_base){
        int x1 = c1->x2-c1->x1;
        int y1 = c1->y2-c1->y1;
        float mod_1 = sqrt(x1*x1+y1*y1);
        
        int x3 = m_base->x2-m_base->x1;
        int y3 = m_base->y2-m_base->y1;
        float mod_3 = sqrt(x3*x3+y3*y3);
        
        int   dot1 = x1*x3+y1*y3;
        float den1 = mod_1*mod_3;
        float a1   = acos (dot1/den1) * 180.0 / PI;
        
        return a1;
    }
    
    bool operator()( const myline* c1, const myline* c2 )
    {
        float a1 = get_angle(c1, m_base);
        float a2 = get_angle(c2, m_base);
        // check which angle is more
        return a1 > a2;
    }
private:
    myline* m_base;
};



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
        
        if ( this->get_perpendicular_distance(pt) <= 6 && this->check_within_line_segment(pt) == 1){
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
        float dist = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
        return dist;
    }
}

// only call when line's both points are known
float myline::get_perpendicular_distance(i2tuple a){
    int x_0 = std::get<0>(a);
    int y_0 = std::get<1>(a);
    
    float dist = abs((y2-y1)*x_0-(x2-x1)*y_0+x2*y1-y2*x1)/sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
    return dist;
}

// only call when line's both points are known
// checks if the point's x and y are within this line segment
int myline::check_within_line_segment(i2tuple a){
    int x_0 = std::get<0>(a);
    int y_0 = std::get<1>(a);
    int mx1 = mymin(x1, x2);
    int my1 = mymin(y1, y2);
    int mx2 = mymax(x1, x2);
    int my2 = mymax(y1, y2);
    
    if(x_0 >= mx1-PERPENDICULAR_THRESH && x_0 <= mx2+PERPENDICULAR_THRESH && y_0 >= my1-PERPENDICULAR_THRESH && y_0 <= my2+PERPENDICULAR_THRESH){
        return 1;
    }
    else{
        return 0;
    }
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


// checks if both lines are same
bool myline::is_equal_to(myline *a){
    return (a->x1 == this->x1 && a->x2 == this->x2 && a->y1 == this->y1 && a->y2 == this->y2);
}


// checks if both lines are reverse of each other
bool myline::is_reverse_of(myline *a){
    return (a->x1 == this->x2 && a->x2 == this->x1 && a->y1 == this->y2 && a->y2 == this->y1);
}


// returns the reverse line out of all lines present for the current present
myline *myline::get_reverse_line(std::vector<myline*> all_lines){
    for(std::vector<myline*>::iterator iter = all_lines.begin(); iter != all_lines.end(); iter++){
        myline *a = *iter;
        if(this->is_reverse_of(a)){
            return a;
        }
    }
    return NULL;
}



// for a given line returns the next line to traverse
myline* get_next_line(myline *first, std::vector<myline*> valid_lines){
    // get all adjacent lines
    std::vector<myline*> adjacent_lines = get_all_adjacent_lines(first, valid_lines);
    
    std::vector<myline*> ccw_lines; // counter clockwise lines
    std::vector<myline*> cw_lines;  // clock wise lines
    
    // get list of all clockwise and counter clock wise lines
    for(std::vector<myline*>::iterator iter = adjacent_lines.begin(); iter != adjacent_lines.end(); iter++){
        myline *second = *iter;
        if(ccw(first, second)){
            ccw_lines.push_back(second);
        }else{
            cw_lines.push_back(second);
        }
    }
    
    // only one counter clock wise line
    if(ccw_lines.size() == 1){
        return ccw_lines[0];
    }
    else if(ccw_lines.size() > 1){
        std::sort(ccw_lines.begin(), ccw_lines.end(), sortbyangledesc(first));
        return ccw_lines[0];
    }
    else if (cw_lines.size() == 1){
        return cw_lines[0];
    }
    else{
        std::sort(cw_lines.begin(), cw_lines.end(), sortbyangleasc(first));
        return cw_lines[0];
    }
    
    return NULL;
}



// returns the area of triangle formed by two lines
// end of first line and start of second line should be same
float triangle_area(myline *first, myline *second){
    return (first->x2 - first->x1)*(second->y2 - first->y1) - (first->y2 - first->y1)*(second->x2 - first->x1);
}

// Returns 1 if the lines first and second are in a counterclockwise order
bool ccw(myline *first, myline *second){
    int area = triangle_area(first, second);
    return (area > 0);
}

// here start is directed
// and all_lines_to_check is direted as well
std::vector<myline*> get_all_adjacent_lines(myline *start, std::vector<myline*> all_lines_to_check){
    std::vector<myline*> adjacent_lines;
    for(std::vector<myline*>::iterator iter = all_lines_to_check.begin(); iter != all_lines_to_check.end(); iter++){
        myline* a = *iter;
        // check if both lines are not same and check if either end of a is same as end of line a
        if(!a->is_equal_to(start) && !a->is_reverse_of(start) && (a->x1 == start->x2 && a->y1 == start->y2)){
            adjacent_lines.push_back(a);
        }
    }
    return adjacent_lines;
}

// get polygon lines by traversing over the lines starting from this line
std::vector<myline*> myline::get_polygon(std::vector<myline*> valid_lines){
    std::vector<myline*> polygon_lines;
    
    myline *start = this;
    myline *sa = start;
    
    while(1){
        myline * nl = get_next_line(sa, valid_lines);
        if(nl->is_equal_to(start)){
            break;
        }
        else{
            polygon_lines.push_back(nl);
        }
        sa = nl;
    }
    
    return polygon_lines;
}

#endif

