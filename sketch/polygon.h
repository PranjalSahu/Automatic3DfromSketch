//
//  polygon.h
//  sketch
//
//  Created by Pranjal Sahu on 10/29/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef polygon_h
#define polygon_h

#include "myline.h"

class polygon{
public:
    std::vector<myline*> lines;
    bool is_equal_to(polygon *);
    bool is_part_of(std::vector<polygon*> pvector);

    polygon(std::vector<myline*> lines);
};




#endif /* polygon_h */
