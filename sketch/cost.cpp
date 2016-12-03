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
#include <utility>

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
        line_2d.push_back(points_2d[i+1]-points_2d[i]);
        line_3d.push_back(points_3d[i+1]-points_3d[i]);
        line_3d[i] = line_3d[i]/glm::length(line_3d[i]);
    }
    
    // get line vector in 3d and 2d
    line_2d.push_back(points_2d[0]-points_2d[points_2d.size()-1]);
    line_3d.push_back(points_3d[0]-points_3d[points_2d.size()-1]);
    line_3d[points_2d.size()-1] = line_3d[points_2d.size()-1]/glm::length(line_3d[points_2d.size()-1]);
    
    
    
    // getting weights for each 2d line
    for(int i=0;i<line_2d.size();++i){
        
        // take angle with each axis in 2d
        float maxv = -1;
        int index = -1;
        
        for(int j=0;j<3;++j){
            float a = 180*acos(glm::dot(line_2d[i]/glm::length(line_2d[i]), -1*axis_2d[j]))/PI;
            float b = 180*acos(glm::dot(line_2d[i]/glm::length(line_2d[i]), axis_2d[j]))/PI;
            
            float angle = a < b ? a : b;
            float v = gaussian(angle);
            if(v > maxv){
                maxv   = v;
                index  = j;
            }
        }
        
        weights.push_back(maxv);
        axis_mapping.push_back(index);
    }
    
    
    // refer axis alignment cost functions
    float total_cost = 0;
    float cost_v[100];
    float angle_v[100];
    float dot_values[100];
    for(int i=0;i<line_3d.size();++i){
        float dot_value = 0;
        for(int q = 0;q<3;++q){
            dot_value = dot_value+axis_3d[axis_mapping[i]][q]*line_3d[i][q];
        }
        
        dot_values[i] = dot_value;
        float angle = acos(dot_value);
        angle_v[i] = angle;
        float cross = fabs(sin(angle));
        float edge_weight = weights[i]*cross;
        cost_v[i] = edge_weight;
        total_cost  = total_cost + edge_weight;
    }
    
    //for(int i=0;i<4;++i){
    //std::cout<<glm::to_string(line_3d[i]/glm::length(line_3d[i]))<<std::endl;
//    printf("Dot values  are : %f %f %f %f \n", dot_values[0], dot_values[1], dot_values[2], dot_values[3]);
//    printf("angles  are : %f %f %f %f \n", angle_v[0], angle_v[1], angle_v[2], angle_v[3]);
//    printf("Edge costs are : %f %f %f %f \n", cost_v[0], cost_v[1], cost_v[2], cost_v[3]);
    //}
    
    return total_cost;
}


// returns the parallelism cost for the points
// uses edges_list to get the edges which are already rendered

float cost::parallelism(std::vector<glm::vec2> points_2d, std::vector<glm::vec3> points_3d,
                        std::vector<std::pair<int, int>> edges_list,
                        std::vector<glm::vec2> corres_2d,
                        std::vector<glm::vec3> corres_3d){
    
    // if no edge is already rendered then return 0
    // since then no cost can be calculated
    if(edges_list.size() == 0){
        return 0;
    }
    
    std::vector<glm::vec2> line_2d;
    std::vector<glm::vec3> line_3d;
    
    // to store weights for each 2d line
    std::vector<float> weights;
    // to store the most parallel edge in 3d
    std::vector<int>   most_parallel_3d_edge;
    
    
    for(int i=0;i<points_2d.size()-1;++i){
        // get line vector in 3d and 2d
        line_2d.push_back(points_2d[i+1]-points_2d[i]);
        line_3d.push_back(points_3d[i+1]-points_3d[i]);
        line_3d[i] = line_3d[i]/glm::length(line_3d[i]);
    }
    
    // get line vector in 3d and 2d
    line_2d.push_back(points_2d[0]-points_2d[points_2d.size()-1]);
    line_3d.push_back(points_3d[0]-points_3d[points_2d.size()-1]);
    line_3d[points_2d.size()-1] = line_3d[points_2d.size()-1]/glm::length(line_3d[points_2d.size()-1]);
    
    
    
    // getting weights for each 2d line
    // using the edges_list which is the list of already rendered 2d line
    for(int i=0;i< line_2d.size();++i){
        
        // take angle with each axis in 2d
        float maxv = -1;
        int  index = -1;
        
        // iterate over all 2d rendered lines
        for(int j=0;j< edges_list.size();++j){
            
            glm::vec2 edge_temp = corres_3d[edges_list[j].first] - corres_3d[edges_list[j].second];
            edge_temp = edge_temp/glm::length(edge_temp);
            
            float a = 180*acos(glm::dot(line_2d[i]/glm::length(line_2d[i]), -1*edge_temp))/PI;
            float b = 180*acos(glm::dot(line_2d[i]/glm::length(line_2d[i]), edge_temp))/PI;
            
            float angle = a < b ? a : b;
            float v = gaussian(angle);
            if(v > maxv){
                maxv   = v;
                index  = j;
            }
        }
        
        weights.push_back(maxv);
        most_parallel_3d_edge.push_back(index);
        
    }
    
    
    // refer axis alignment cost functions in the paper
    float total_cost = 0;
    float cost_v[10];
    float angle_v[10];
    float dot_values[10];
    
    for(int i=0;i< line_3d.size();++i){
        
        glm::vec3 most_weight_edge = corres_3d[edges_list[most_parallel_3d_edge[i]].first] - corres_3d[edges_list[most_parallel_3d_edge[i]].second];
        most_weight_edge = most_weight_edge/glm::length(most_weight_edge);
                                               
        float dot_value = glm::dot(most_weight_edge, line_3d[i]);
        
        if(dot_value > 1){
            dot_value = 1.0;
        }
        else if(dot_value < -1){
            dot_value = -1.0;
        }
        
        //dot_values[i] = dot_value;
        float angle = acos(dot_value);
        //angle_v[i] = angle;
        float cross = fabs(sin(angle));
        float edge_weight = weights[i]*cross;
        //cost_v[i] = edge_weight;
        total_cost  = total_cost + edge_weight;
    }
    
//    printf("Dot values  are : %f %f %f %f \n", dot_values[0], dot_values[1], dot_values[2], dot_values[3]);
//    printf("angles  are : %f %f %f %f \n", angle_v[0], angle_v[1], angle_v[2], angle_v[3]);
//    printf("Edge costs are : %f %f %f %f \n", cost_v[0], cost_v[1], cost_v[2], cost_v[3]);
    
    return total_cost;
}





// takes as input the point in 2d axis
// 4 points must be provided
cost::cost(std::vector<glm::vec2> point_2d_axis){
    
    axis_3d[0][0] = 1;
    axis_3d[0][1] = 0;
    axis_3d[0][2] = 0;
    
    axis_3d[1][0] = 0;
    axis_3d[1][1] = 1;
    axis_3d[1][2] = 0;
    
    axis_3d[2][0] = 0;
    axis_3d[2][1] = 0;
    axis_3d[2][2] = 1;
    
    
    // take 4 points as input from user
    glm::vec2 px  = point_2d_axis[1]-point_2d_axis[0];
    glm::vec2 py  = point_2d_axis[2]-point_2d_axis[0];
    glm::vec2 pz  = point_2d_axis[3]-point_2d_axis[0];
    
    axis_2d[0] = px/glm::length(px);
    axis_2d[1] = py/glm::length(py);
    axis_2d[2] = pz/glm::length(pz);
}