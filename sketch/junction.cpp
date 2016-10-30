//
//  junction.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/29/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#include <stdio.h>
#include "myline.h"
#include <stdlib.h>
#include "junction.h"

junction::junction(std::vector<myline*> linesa, int a,int b){
    x = a;
    y = b;
    lines = linesa;
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

// checks if a junction is alredy present the list
bool check_if_junction_present(int x, int y, std::list<junction *> all_junctions){
    for(std::list<junction*>::iterator iterator = all_junctions.begin(); iterator != all_junctions.end(); ++iterator) {
        junction *j = *iterator;
        if(j->x == x && j->y == y)
            return true;
    }
    return false;
}