//
//  polygon.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/29/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#include <stdio.h>
#include "myline.h"
#include "polygon.h"


polygon::polygon(std::vector<myline*> tlines){
    this->lines = tlines;
}


// checks if two polygons are equal or not
bool polygon::is_equal_to(polygon *b){
    std::vector<myline*> at = this->lines;
    std::vector<myline*> bt = b->lines;
    
    if(bt.size() != at.size()){
        return false;
    }
    
    bool flag = false;
    for(std::vector<myline*>::iterator it = bt.begin(); it != bt.end(); ++it){
        myline *t = *it;
        flag = false;
        for(std::vector<myline*>::iterator ita = at.begin(); ita != at.end(); ++ita){
            myline *ta = *ita;
            if(ta->is_equal_to(t) || ta->is_reverse_of(t)){
                flag = true;
                break;
            }
        }
        
        if(flag == false){
            return false;
        }
    }
    
    return true;
}

// checks if the polygon is contained in the polygon vector pvector
bool polygon::is_part_of(std::vector<polygon*> pvector){
    for(std::vector<polygon*>::iterator it = pvector.begin(); it != pvector.end(); ++it){
        polygon *a = *it;
        if(this->is_equal_to(a)){
            return true;
        }
    }
    return false;
}

// returns all the polygons by traversing taking each line segment as starting point
std::vector<polygon*> get_all_polygons(std::vector<myline*> all_lines){
    std::vector<polygon*> all_polygons;
    // loop for every line and take it as start and get the polygon by traversing over all valid lines
    for(std::vector<myline*>::iterator it = all_lines.begin(); it != all_lines.end(); ++it){
        myline *start = *it;
        std::vector<myline*> polygon_lines = start->get_polygon(all_lines);
        polygon* temp = new polygon(polygon_lines);
        
        // check if this polygon is already covered
        // if not then insert it into list of polygons
        if(!temp->is_part_of(all_polygons)){
            all_polygons.push_back(temp);
        }
    }
    
    return all_polygons;
}


// returns the points of a polygon by traversing over the lines
std::vector<mypoint*> polygon::get_points(){
    std::vector<mypoint*> all_points;
    for(std::vector<myline*>::iterator it = lines.begin(); it != lines.end(); ++it){
        myline *l = *it;
        all_points.push_back(new mypoint(l->x1, l->y1, 0));
    }
    return all_points;
}