//
//  junction.h
//  sketch
//
//  Created by Pranjal Sahu on 10/29/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef junction_h
#define junction_h

#include <stdio.h>
#include "myutilities.h"
#include <queue>

class junction{
public:
    std::vector<myline*> lines;
    int x, y;
    // 0 for L, 1 for A, 2 for Y, 3 for T
    int junction_type;
    
    junction(std::vector<myline*> lines, int xa, int ya);
};


bool check_if_junction_present(int x, int y, std::list<junction *> all_junctions);
std::vector<myline*> get_all_lines_for_this_junction(int x, int y, std::vector<myline*> all_lines);
std::vector<myline*> get_rest_of_junction_lines(myline* l, std::vector<myline*> la);



#endif /* junction_h */
