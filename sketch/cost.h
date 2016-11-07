//
//  cost.h
//  sketch
//
//  Created by Pranjal Sahu on 11/5/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef cost_h
#define cost_h

#include "myline.h"
#include "mypoint.h"

class cost{
public:
    glm::vec2 axis_2d[3];       // x, y, z
    glm::vec3 axis_3d[3];       // x, y, z
    glm::vec3 ap[3];
    
    float axis_alignment(std::vector<glm::vec2> points, std::vector<glm::vec3> points_3d);
    cost(std::vector<glm::vec2>);
};


#endif /* cost_h */
