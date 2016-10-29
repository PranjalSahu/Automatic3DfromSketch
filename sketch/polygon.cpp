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
    
    for(std::vector<myline*>::iterator it = all_lines.begin(); it != all_lines.end(); ++it){
        myline *a = *it;
        
        
    }
    return NULL;
}
