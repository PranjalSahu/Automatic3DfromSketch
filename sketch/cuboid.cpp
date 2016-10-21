//
//  cuboid.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/19/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#include <stdio.h>
#include "view.h"
#include "cuboid.h"


// insert a point in the cuboid
void cuboid::insert_mouse_point(int x, int y){
    if(this->input_done != 1){
        this->mouse_xs[this->current_index] = x;
        this->mouse_ys[this->current_index] = y;
        this->current_index = this->current_index+1;
        
        if(current_index >= 7){
            input_done = 1;
            this->get_height_width_depth_of_cuboid();
        }
    }
    return;
}


int get_distance(int x1, int x2, int y1, int y2){
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}



//              x        y
// returns the length breadth and height of the cuboid given the 5 points
void cuboid::get_height_width_depth_of_cuboid(){
    int height  = get_distance(this->mouse_xs[0], this->mouse_xs[1], this->mouse_ys[0], this->mouse_ys[1]);
    int length = get_distance(this->mouse_xs[1], this->mouse_xs[2], this->mouse_ys[1], this->mouse_ys[2]);
    int breadth  = get_distance(this->mouse_xs[1], this->mouse_xs[4], this->mouse_xs[1], this->mouse_xs[4]);
    
    
    // update the dimensions of this cuboid
    this->dimen[0] = length;
    this->dimen[1] = breadth;
    this->dimen[2] = height;
    
    
    // flag to denote that cuboid input has been taken
    this->input_done = 1;
    
    printf("Cuboid lbh %d %d %d\n", length, breadth, height);
}

// calculates the 3d coordinates of the cuboid and stores it in world_x, world_y, world_z
// if it has no parent then just scaling
// else use the coordinates of the parent cuboid
// to estimate the coordinates of the neighbouring cuboid
// should be called after getting length, breadth and height of the cuboid

void cuboid::get_3d_coordinates(){
    int height  = this->dimen[2];
    int length  = this->dimen[0];
    int breadth = this->dimen[1];
    
    
    // front face
    world_x[0] = 0; world_y[0] = 0; world_z[0] = 0;
    world_x[1] = 0; world_y[1] = height; world_z[0] = 0;
    world_x[2] = 0; world_y[2] = height; world_z[2] = -1*length;
    world_x[3] = 0; world_y[3] = 0; world_z[3] = -1*length;
    
    // back face
    world_x[4] = -1*breadth; world_y[4] = 0; world_z[4] = 0;
    world_x[5] = -1*breadth; world_y[5] = height; world_z[5] = 0;
    world_x[6] = -1*breadth; world_y[6] = height; world_z[6] = -1*length;
    world_x[7] = -1*breadth; world_y[7] = 0; world_z[7] = -1*length;
    
    
    return;
}


void cuboid::drawmycuboid(int cuboid_ratio){
    
    GLfloat l = dimen[2]/cuboid_ratio;
    GLfloat b = dimen[0]/cuboid_ratio;
    GLfloat h = dimen[1]/cuboid_ratio;

    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    
    // THIS CUBOID IS OF DIMENSION 6 x 4 x 2
    // FRONT
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(0.0f, l, -1*b);
    glVertex3f(0.0f, 0.0f, -b);
    
    // SHIFT FRONT BY 2 IN -X TO GET BACK
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(-1*h, 0.0f, -1*b);
    
    // TOP
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(0.0f, l, -1*b);
    
    // SHIFT TOP BY -6 IN Y TO GET BOTTOM
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, -1*b);
    glVertex3f(0.0f, 0.0f, -1*b);
    
    // OPPOSITE HIDDEN
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    
    // HIDDEN
    glVertex3f(0.0f, 0.0f, -1*b);
    glVertex3f(0.0f, l, -1*b);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(-1*h, 0.0f, -1*b);
    
    glEnd();
    return;
}


// helper function to create array for plotting cuboids 2nd face
void cuboid::get_2nd_face_points(int *xs, int *ys){
    xs[0] = mouse_xs[1];
    ys[0] = mouse_ys[1];
    
    if( current_index >= 5){
        xs[1] = mouse_xs[4];
        ys[1] = mouse_ys[4];
    }
    return;
}

