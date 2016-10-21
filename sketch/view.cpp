//
//  view.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/19/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#include "cuboid.h"
#include "view.h"
#include "myutilities.h"

#include <stdio.h>

view::view(){
    vanishing_point[0] = NULL;
    vanishing_point[1] = NULL;
}


mypoint* get_intersection(GLfloat x1, GLfloat x2, GLfloat x3, GLfloat x4, GLfloat y1, GLfloat y2, GLfloat y3, GLfloat y4){
    
    GLfloat ua = ((x4-x3)*(y1-y3)-(y4-y3)*(x1-x3))/((y4-y3)*(x2-x1)-(x4-x3)*(y2-y1));
    GLfloat ub = ((x2-x1)*(y1-y3)-(y2-y1)*(x1-x3))/((y4-y3)*(x2-x1)-(x4-x3)*(y2-y1));
    
    GLfloat xf = x1+ua*(x2-x1);
    GLfloat yf = y1+ub*(y2-y1);
    
    mypoint* mp = new mypoint();
    mp->x = xf;
    mp->y = yf;
    
    return mp;
}

cuboid* view::get_current_cuboid(){
    if(current_cuboid == -1){
        return NULL;
    }
    else{
        return this->cuboids[this->current_cuboid];
    }
}

// parent_flag = 1 means the current cuboid is the parent cuboid and is used for estimating
// the vanishing points
cuboid* view::create_cuboid(int parent_flag){
    cuboid *c = new cuboid();
    if(parent_flag == 1){
        c->parent_cuboid = NULL;            // parent cuboid is null if parent_flag is 1
    }
    else{
        c->parent_cuboid = this->cuboids[0];    // hardcoding at present
                                                // will change it later to incorporate multiple cuboids
    }
    this->cuboids[num_cuboid] = c;
    this->current_cuboid = this->num_cuboid;
    this->num_cuboid = this->num_cuboid+1;
    return c;
}

void view::drawinputlinesforaview(){
    for(int i=0;i<this->num_cuboid;++i){
        cuboid* c = this->cuboids[i];
        
        if(c->current_index <= 4){
            drawinputlinesforacuboid(c->mouse_xs, c->mouse_ys, 0, c->current_index);
        }
        //else if(c->current_index == 7){
            
        //}
        else{
            drawinputlinesforacuboid(c->mouse_xs, c->mouse_ys, 0, 4);
            drawline(c->mouse_xs[1], c->mouse_ys[1], c->mouse_xs[4], c->mouse_ys[4]);
            drawinputlinesforacuboid_b(c->mouse_xs, c->mouse_ys, 4, c->current_index);
        }
    }
}

// given the count it draws the points taken from array point_x and point_y
// takes the starting index from where to start taking points from array
void view::drawinputlinesforacuboid(int point_x[], int point_y[], int start, int count){
    glBegin(GL_LINE_LOOP);
    for(int i=start;i<count;++i){
        glVertex2f(point_x[i], point_y[i]);
    }
    glEnd();
}

void view::drawline(int pointx1, int pointy1, int pointx2, int pointy2){
    glBegin(GL_LINES);
    glVertex2f(pointx1,pointy1);
    glVertex2f(pointx2,pointy2);
    glEnd();
}

void view::drawinputlinesforacuboid_b(int point_x[], int point_y[], int start, int count){
    glBegin(GL_LINES);
    for(int i=start;i<count-1;++i){
        glVertex2f(point_x[i], point_y[i]);
        glVertex2f(point_x[i+1], point_y[i+1]);
    }
    glEnd();
    
    get_vanishing_points();
}

void view::get_vanishing_points(){
    // at present take the first cuboid as the parent
    // use it to get vanishing point and store it somewhere
    
    cuboid *parent_cuboid = this->cuboids[0];
    
    
    mypoint* mp = get_intersection(parent_cuboid->mouse_xs[1], parent_cuboid->mouse_xs[2], parent_cuboid->mouse_xs[0], parent_cuboid->mouse_xs[3],
                     parent_cuboid->mouse_ys[1], parent_cuboid->mouse_ys[2], parent_cuboid->mouse_ys[0], parent_cuboid->mouse_ys[3]);
    this->vanishing_point[0] = mp;
    
    drawline(parent_cuboid->mouse_xs[1], parent_cuboid->mouse_ys[1], mp->x, mp->y);
    drawline(parent_cuboid->mouse_xs[0], parent_cuboid->mouse_ys[0], mp->x, mp->y);

    
    if(parent_cuboid->input_done){
        mypoint* mpa = get_intersection(parent_cuboid->mouse_xs[3], parent_cuboid->mouse_xs[6], parent_cuboid->mouse_xs[2], parent_cuboid->mouse_xs[5],
                                       parent_cuboid->mouse_ys[3], parent_cuboid->mouse_ys[6], parent_cuboid->mouse_ys[2], parent_cuboid->mouse_ys[5]);
        
        this->vanishing_point[1] = mpa;
        
        drawline(parent_cuboid->mouse_xs[2], parent_cuboid->mouse_ys[2], mpa->x, mpa->y);
        drawline(parent_cuboid->mouse_xs[3], parent_cuboid->mouse_ys[3], mpa->x, mpa->y);
    }
    
    return;
}

int view::get_done_cuboids(){
    int count = 0;
    
    for (int i=0;i<this->num_cuboid;++i){
        cuboid* cs = this->cuboids[i];
        if(cs->input_done == 1){
            count = count+1;
        }
    }
    
    return count;
}
