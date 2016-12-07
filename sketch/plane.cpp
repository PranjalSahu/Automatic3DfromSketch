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
#include "myutilities.h"
#include <glm/vec2.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>


// point normal form equation of a plane
plane::plane(float _a, float _b, float _c, mypoint* _p){
    float mod = sqrt(_a*_a+_b*_b+_c*_c);
    a = _a/mod;
    b = _b/mod;
    c = _c/mod;
    p = _p;
    
    normal_array[0] = a;
    normal_array[1] = b;
    normal_array[2] = c;
}

plane::plane(std::vector<glm::vec3> plane_points){
    glm::vec3 ta = plane_points[0]-plane_points[1];
    glm::vec3 tb = plane_points[1]-plane_points[2];
    glm::vec3 tc = glm::cross(ta, tb);
    
    // normalize
    tc = tc/glm::length(tc);
 
    a = tc[0];
    b = tc[1];
    c = tc[2];
    p = new mypoint(plane_points[0][0], plane_points[0][1], plane_points[0][2]);
    
    normal_array[0] = a;
    normal_array[1] = b;
    normal_array[2] = c;
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
    
    
    glm::vec3 n = glm::vec3(ka_, kb_, kc_);
    glm::vec3 x = glm::vec3(this->a, this->b, this->c);
    glm::vec3 temp = glm::cross(n, x);

    glm::vec3 vrot = x + sin(angle)*(glm::cross(n, x))+(1-cos(angle))*(glm::cross(n, temp));
    vrot = vrot/glm::length(vrot);
    return new plane(vrot[0], vrot[1], vrot[2], p);
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
    return new mypoint(x+t*n1, y+t*n2, z+t*n3);
}

// returns the point obtained after projecting x,y,z onto this plane
mypoint* plane::project_point(glm::vec4 vertex, float n1, float n2, float n3){
    float x = vertex[0];
    float y = vertex[1];
    float z = vertex[2];
    
    float mod = sqrt(n1*n1+n2*n2+n3*n3);
    
    n1 = n1/mod;
    n2 = n2/mod;
    n3 = n3/mod;
    
    float t;
    float num = a*p->x-a*x+b*p->y-b*y+c*p->z-c*z;
    float den = a*n1+b*n2+c*n3;
    t = num/den;
    float px = x+t*n1;
    float py = y+t*n2;
    float pz = z+t*n3;
    return new mypoint(px, py, pz);
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

// returns the projected points onto this plane along the vector n1, n2, n3
std::vector<glm::vec3> plane::project_polygon(std::vector<glm::vec4> all_points, float n1, float n2, float n3){
    std::vector<glm::vec3> projected_points;
    
    for(std::vector<glm::vec4>::iterator it = all_points.begin(); it != all_points.end(); ++it){
        glm::vec4 p = *it;
        mypoint *pt = project_point(p, n1, n2, n3);
        projected_points.push_back(glm::vec3(pt->x, pt->y, pt->z));
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



