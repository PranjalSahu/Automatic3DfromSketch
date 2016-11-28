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
#include "mypoint.h"
//#include "plane.h"
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>


class polygon{
public:
    std::vector<myline*> lines;
    bool is_equal_to(polygon *);
    bool is_part_of(std::vector<polygon*> pvector);
    bool placed;
    
    //plane* plane_placed;
    std::vector<glm::vec3> points_to_render_vec;
    std::vector<myline*> get_green_lines();
    std::vector<polygon*> get_adjacent_polygons_using_huffman(std::vector<polygon*> all_polygons);

        
    std::vector<mypoint*> get_points();
    std::vector<glm::vec2> get_points_vec();
    polygon(std::vector<myline*> lines);
};

std::vector<polygon*> get_all_polygons(std::vector<myline*> all_lines);


#endif /* polygon_h */
