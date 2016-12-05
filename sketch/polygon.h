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
#include "plane.h"
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <utility>
#include "cost.h"

class plane;

class polygon{
public:
    std::vector<myline*> lines;
    
    // 50 vertices for tesselation it is required to pass data as continuos
    double vertices[50][3];
    
    bool is_equal_to(polygon *);
    bool is_part_of(std::vector<polygon*> pvector);
    bool placed;
    bool axis_assigned; // flag to denote if its rotating axis has been assigned or not
    
    polygon *adjacent_polygon;  // the polygon which was adjacent to it and the axis of rotation was assigned using this
    plane *plane_to_project;    // starting plane which will be rotated
    glm::vec3 rotation_axis;    // axis along whichi plane will be rotated
    std::vector<glm::vec2> rotation_axis_2d_points; // axis of rotation in image
    
    //plane* plane_placed;
    std::vector<glm::vec3> points_to_render_vec;
    std::vector<glm::vec3> get_plane_points(polygon * p, myline *line, std::vector<glm::vec2> corres_2d, std::vector<glm::vec3> corres_3d);
    std::vector<myline*>   get_green_lines();
    std::vector<polygon*>  get_adjacent_polygons_using_huffman(std::vector<polygon*> all_polygons,
                                                              std::vector<glm::vec2> corres_2d,
                                                              std::vector<glm::vec3> corres_3d,
                                                              bool assign_axis);
    
    plane * get_plane_with_angle(float angle_p, plane *global_plane_to_project);
    std::pair<std::vector<glm::vec3>, std::pair<float, float>> get_min_cost_angle_points(std::vector<glm::vec2> corres_2d,
                                      std::vector<glm::vec3> corres_3d,
                                      std::vector<std::pair<int, int>> edges_list,
                                      plane *global_plane_to_project, cost* cost_obj);

        
    std::vector<mypoint*> get_points();
    std::vector<glm::vec2> get_points_vec();
    polygon(std::vector<myline*> lines);
};

std::vector<polygon*> get_all_polygons(std::vector<myline*> all_lines);


#endif /* polygon_h */
