//
//  cost.cpp
//  sketch
//
//  Created by Pranjal Sahu on 11/5/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//  Refer: Optimization-Based Reconstruction of a 3D Object From a Single Freehand Line Drawing

#include <stdio.h>
#include "myline.h"
#include "polygon.h"
#include "mypoint.h"
#include "cost.h"
#include <math.h>

float gaussian(float angle){
    float mean = 0;
    float num = angle-mean;
    float den =7;
    float v = num/den;
    float value = v*v;
    return exp(-1*value);
}

// returns the cost for the points
// points in 2d and in 3d have to be in same order
float cost::axis_alignment(std::vector<glm::vec2> points_2d, std::vector<glm::vec3> points_3d){
    std::vector<glm::vec2> line_2d;
    std::vector<glm::vec3> line_3d;
    
    // weights for each 2d line
    std::vector<float> weights;
    std::vector<int> axis_mapping; // best axis in 2d 0 -> x, 1 -> y, 2 -> z
    
    for(int i=0;i<points_2d.size()-1;++i){
        
        // get line vector in 3d and 2d
        line_2d[i] = points_2d[i+1]-points_2d[i];
        line_3d[i] = points_3d[i+1]-points_3d[i];
        
        float mod2 = sqrt(line_2d[i][0]*line_2d[i][0]+line_2d[i][1]*line_2d[i][1]);
        float mod3 = sqrt(line_3d[i][0]*line_3d[i][0]+line_3d[i][1]*line_3d[i][1]+line_3d[i][2]*line_3d[i][2]);
        
        // normalize
        line_2d[i] = line_2d[i]/mod2;
        line_3d[i] = line_3d[i]/mod3;
    }
    
   
    // getting weights for each 2d line
    for(int i=0;i<line_2d.size();++i){
   
        // take angle with each axis in 2d
        float maxv = -1;
        int index = -1;
        
        for(int j=0;j<3;++j){
            
            float a = glm::angle(line_2d[i], axis_2d[j]);
            float v = gaussian(a);
            if(v > maxv){
                maxv   = v;
                index  = j;
            }
        }
        
        weights[i] = maxv;
        axis_mapping[i] = index;
    }

    
    
    // refer axis alignment cost functions
    float total_cost = 0;
    for(int i=0;i<line_3d.size();++i){
        total_cost = total_cost + weights[i]*angle(line_3d[i], axis_3d[axis_mapping[i]]);
    }
    
    return total_cost;
}

cost::cost(glm::vec2 point_2d_axis[]){
    
    axis_3d[0][0] = 1;
    axis_3d[0][1] = 0;
    axis_3d[0][2] = 0;
    
    axis_3d[1][0] = 0;
    axis_3d[1][1] = 1;
    axis_3d[1][2] = 0;
    
    axis_3d[2][0] = 0;
    axis_3d[2][1] = 1;
    axis_3d[2][2] = 0;
    
    
    // tale 4 points as input from user
    glm::vec2 px  = point_2d_axis[1]-point_2d_axis[0];
    glm::vec2 py  = point_2d_axis[2]-point_2d_axis[0];
    glm::vec2 pz  = point_2d_axis[3]-point_2d_axis[0];
    
    axis_2d[0] = px;
    axis_2d[1] = py;
    axis_2d[2] = pz;
}