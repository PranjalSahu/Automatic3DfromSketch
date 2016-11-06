//
//  plane.cpp
//  sketch
//
//  Created by Pranjal Sahu on 11/4/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//
// Refer:: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
// Refer:: http://math.stackexchange.com/questions/100761/how-do-i-find-the-projection-of-a-point-onto-a-plane

#include <stdio.h>
#include <math.h>
#include "plane.h"

// point normal form equation of a plane
plane::plane(float _a, float _b, float _c, mypoint* _p){
    float mod = sqrt(_a*_a+_b*_b+_c*_c);
    a = _a/mod;
    b = _b/mod;
    c = _c/mod;
    p = _p;
}

//rotates the current plane by angle around the axis given by ka_, kb_, kc_ using Rodrigues's rotation formula
// give angle in degree
plane* plane::rotate_it(float angle, float ka_, float kb_, float kc_){
    
    // convert to radian
    angle = angle * PI/180.0;
    
    float mod = sqrt(ka_*ka_+kb_*kb_+kc_*kc_);
    ka_ = ka_/mod;
    kb_ = kb_/mod;
    kc_ = kc_/mod;
    
    float v_a = a*cos(angle);
    float v_b = b*cos(angle);
    float v_c = c*cos(angle);
    
    float kdotv = ka_*a + kb_*b + kc_*c;
    kdotv = kdotv*(1-cos(angle));
    
    float kcrossv_a, kcrossv_b, kcrossv_c;
    kcrossv_a = (kb_*c-kc_*b)*sin(angle);
    kcrossv_b = (kc_*a-ka_*b)*sin(angle);
    kcrossv_c = (ka_*b-kb_*a)*sin(angle);
    
    float vrota, vrotb, vrotc;
    vrota = v_a + kcrossv_a + kdotv*ka_;
    vrotb = v_b + kcrossv_b + kdotv*kb_;
    vrotc = v_c + kcrossv_c + kdotv*kc_;
    
    mod = sqrt(vrota*vrota+vrotb*vrotb+vrotc*vrotc);
    
    // change the normal vector for the plane
    //a = vrota/mod;
    //b = vrotb/mod;
    //c = vrotc/mod;
    
    return new plane(vrota/mod, vrotb/mod, vrotc/mod, p);
}

// returns the points obtained after projecting x, y, z onto this plane along vector n1, n2, n3
mypoint* plane::project_point(float x, float y, float z, float n1, float n2, float n3){
    float t;
    float mod = sqrt(n1*n1+n2*n2+n3*n3);
    
    n1 = n1/mod;
    n2 = n2/mod;
    n3 = n3/mod;
    
    float num = a*p->x-a*x+b*p->y-b*y+c*p->z-c*z;
    float den = a*n1+b*n2+c*n3;
    t = num/den;
    return new mypoint(x+t*a, y+t*b, z+t*c);
}



// returns the point obtained after projecting x,y,z onto this plane
mypoint* plane::project_point(float x, float y, float z){
    float t;
    float num = a*p->x-a*x+b*p->y-b*y+c*p->z-c*z;
    float den = a*a+b*b+c*c;
    t = num/den;
    return new mypoint(x+t*a, y+t*b, z+t*c);
}

// returns the projected points onto this plane along the vector n1, n2, n3
std::vector<mypoint*> plane::project_polygon(std::vector<mypoint*> all_points, float n1, float n2, float n3){
    std::vector<mypoint*> projected_points;
    
    for(std::vector<mypoint*>::iterator it = all_points.begin(); it != all_points.end(); ++it){
        mypoint *p = *it;
        projected_points.push_back(project_point(p->x, p->y, p->z, n1, n2, n3));
    }
    return projected_points;
}

// returns the projected points onto this plane
std::vector<mypoint*> plane::project_polygon(std::vector<mypoint*> all_points){
    std::vector<mypoint*> projected_points;
    
    for(std::vector<mypoint*>::iterator it = all_points.begin(); it != all_points.end(); ++it){
        mypoint *p = *it;
        projected_points.push_back(project_point(p->x, p->y, p->z));
    }
    return projected_points;
}


float plane::get_z(float x, float y){
    return -1*(a*x+b*y)/(c+0.000001);
}


// returns the projection of the polygon onto this plane
//polygon *project_polygon(polygon *p){
//    std::vector<myline*> all_lines = p->lines;
//    
//    
//    for(std::vector<myline*>::iterator it = bt.begin(); it != bt.end(); ++it){
//
//    mypoint
//}



