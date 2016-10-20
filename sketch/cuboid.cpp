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
    int length  = get_distance(this->mouse_xs[0], this->mouse_xs[1], this->mouse_ys[0], this->mouse_ys[1]);
    int breadth = get_distance(this->mouse_xs[1], this->mouse_xs[2], this->mouse_ys[1], this->mouse_ys[2]);
    int height  = get_distance(this->mouse_xs[1], this->mouse_xs[4], this->mouse_xs[1], this->mouse_xs[4]);
    
    
    // update the dimensions of this cuboid
    this->dimen[0] = length;
    this->dimen[1] = breadth;
    this->dimen[2] = height;
    
    
    // flag to denote that cuboid input has been taken
    this->input_done = 1;
    
    printf("Cuboid lbh %d %d %d\n", length, breadth, height);
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

