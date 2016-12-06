//
//  junction.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/29/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "junction.h"


// taking 10 degree buffer
bool check_if_parallel(float angle){
    if(abs(angle) < 10 || (abs(angle) < 190 && abs(angle) > 170))
       return true;
    return false;
}

junction::junction(std::vector<myline*> linesa, int a,int b){
    x = a;
    y = b;
    lines = linesa;
    
    
    if(lines.size() == 2){
        junction_type = 0; // L type junction
    }
    // can't have more than 3 lines
    // trihedral assumption
    // A and Y junction
    else if(lines.size()== 3){
        myline *l1 = linesa[0];
        myline *l2 = linesa[1];
        myline *l3 = linesa[2];
        
        float ang[3];
        ang[0] = l1->get_angle(l2, x, y);
        ang[1] = l1->get_angle(l3, x, y);
        ang[2] = l2->get_angle(l3, x, y);
        
        // if two angles are more than 90 than it is Y junction
        // else it is A junction
        int count = 0;
        for(int i=0;i<3;++i){
            if(ang[i] > 90){
                count = count+1;
            }
        }
        
        // Ignoring T junction at present
        // REMEBER TO COMPLETE IT
        //if(check_if_parallel(ang[0]) || )
        if(count >=2){
            junction_type = 2; // Y type junction
        }
        else{
            junction_type = 1; // A type junction
        }
        
        
        // in case of A junction label the middle line
        if(junction_type == 1){
            if(ang[0] > ang[1] && ang[0] > ang[2]){
                if(l3->label.compare("nl") == 0){
                    l3->label =  "green";
                    l3->reverse_line->label = "green";
                }
            }else if(ang[1] > ang[0] && ang[1] > ang[2]){
                if(l2->label.compare("nl") == 0){
                    l2->label =  "green";
                    l2->reverse_line->label =  "green";
                }
            }else if(ang[2] > ang[1] && ang[2] > ang[0]){
                if(l1->label.compare("nl") == 0){
                    l1->label =  "green";
                    l1->reverse_line->label =  "green";
                }
            }
            
            printf("labelling junction A middle line");
        }
    }
}

// returns the lines for this junction
std::vector<myline*> get_all_lines_for_this_junction(int x, int y, std::vector<myline*> all_lines){
    std::vector<myline*> temp;
    for(std::vector<myline*>::iterator iterator = all_lines.begin(); iterator != all_lines.end(); ++iterator) {
        myline *t = *iterator;
        // check both ends
        if((t->x1 == x && t->y1 == y) || (t->x2 == x && t->y2 == y)){
            temp.push_back(t);
        }
    }
    return temp;
}

// given a junction and a line returns the rest of lines at that junction
std::vector<myline*> get_rest_of_junction_lines(myline* l, std::vector<myline*> junction_lines){
    std::vector<myline*> temp;
    for(std::vector<myline*>::iterator iterator = junction_lines.begin(); iterator != junction_lines.end(); ++iterator) {
        myline *t = *iterator;
        // ignore same line and insert the rest
        if(!(t->is_equal_to(l) || t->is_reverse_of(l))){
            temp.push_back(t);
        }
    }
    
    return temp;
}


// checks if a junction is alredy present the list
bool check_if_junction_present(int x, int y, std::list<junction *> all_junctions){
    for(std::list<junction*>::iterator iterator = all_junctions.begin(); iterator != all_junctions.end(); ++iterator) {
        junction *j = *iterator;
        if(j->x == x && j->y == y)
            return true;
    }
    return false;
}