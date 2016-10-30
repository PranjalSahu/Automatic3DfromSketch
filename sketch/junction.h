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
    junction(std::vector<myline*> lines, int xa, int ya);
};


bool check_if_junction_present(int x, int y, std::list<junction *> all_junctions);
std::vector<myline*> get_all_lines_for_this_junction(int x, int y, std::vector<myline*> all_lines);

#endif /* junction_h */
