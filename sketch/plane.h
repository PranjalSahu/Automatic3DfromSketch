//
//  plane.h
//  sketch
//
//  Created by Pranjal Sahu on 11/4/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef plane_h
#define plane_h
#include "myutilities.h"
#include "mypoint.h"
#include "polygon.h"

class plane{
public:
    float a, b, c;
    mypoint* p;
    
    plane* rotate_it(float angle, float a_, float b_, float c_);
    mypoint *project_point(float x, float y, float z);
    std::vector<mypoint*> project_polygon(std::vector<mypoint*> all_points);
    polygon *project_polygon(polygon *p);
    plane(float a, float b, float c, mypoint* p);
    
};

#endif /* plane_h */