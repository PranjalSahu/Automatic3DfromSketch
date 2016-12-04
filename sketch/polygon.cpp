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
#include "cost.h"

polygon::polygon(std::vector<myline*> tlines){
    this->lines = tlines;
    placed = false;
    axis_assigned = false;
}


// checks if two polygons are equal or not
bool polygon::is_equal_to(polygon *b){
    printf("TESTING BUG 9 start\n");
    std::vector<myline*> at = this->lines;
    std::vector<myline*> bt = b->lines;
    
    if(bt.size() != at.size()){
        printf("TESTING BUG 9 end\n");
        return false;
    }
    
    bool flag = false;
    std::vector<mypoint*> pl = b->get_points();
    //printf("TESTING BUG >> %f %f %f\n", pl[0]->x, pl[0]->y, pl[0]->z);
    for(std::vector<myline*>::iterator it = bt.begin(); it != bt.end(); ++it){
        myline *t = *it;
        flag = false;
        for(std::vector<myline*>::iterator ita = at.begin(); ita != at.end(); ++ita){
            myline *ta = *ita;
            //if(ta->x1 == 156  && ta->y1 == 170 && t->x1 == 188 && t->y1 == 31){
                //printf("TESTING BUG10 %d %d %d %d\n", ta->x1, ta->y1, t->x1, t->y1);
            //}
            printf("TESTING BUG 10 start\n");
            bool flaga = ta->is_equal_to(t);
            bool flagb = ta->is_reverse_of(t);
            
            printf("TESTING BUG 10 end\n");
            if(flaga || flagb){
                flag = true;
                break;
            }
        }
        
        if(flag == false){
            printf("TESTING BUG 9 end\n");
            return false;
        }
    }
    
    printf("TESTING BUG 9 end\n");
    return true;
}

// checks if the polygon is contained in the polygon vector pvector
bool polygon::is_part_of(std::vector<polygon*> pvector){
    printf("TESTING BUG 8 start\n");
    for(std::vector<polygon*>::iterator it = pvector.begin(); it != pvector.end(); ++it){
        polygon *a = *it;
        if(this->is_equal_to(a)){
            printf("TESTING BUG 8 end\n");
            return true;
        }
    }
    printf("TESTING BUG 8 end\n");
    return false;
}

// returns all the polygons by traversing taking each line segment as starting point
std::vector<polygon*> get_all_polygons(std::vector<myline*> all_lines){
    std::vector<polygon*> all_polygons;
    // loop for every line and take it as start and get the polygon by traversing over all valid lines
    for(std::vector<myline*>::iterator it = all_lines.begin(); it != all_lines.end(); ++it){
        myline *start = *it;
        std::vector<myline*> polygon_lines = start->get_polygon(all_lines);
        polygon* temp = new polygon(polygon_lines);
        
        // check if this polygon is already covered
        // if not then insert it into list of polygons
        if(!temp->is_part_of(all_polygons)){
            all_polygons.push_back(temp);
        }
    }
    
    return all_polygons;
}



// returns the points of a polygon by traversing over the lines
std::vector<mypoint*> polygon::get_points(){
    std::vector<mypoint*> all_points;
    for(std::vector<myline*>::iterator it = lines.begin(); it != lines.end(); ++it){
        myline *l = *it;
        all_points.push_back(new mypoint(l->x1, l->y1, 0));
    }
    return all_points;
}

// returns the points of a polygon by traversing over the lines
std::vector<glm::vec2> polygon::get_points_vec(){
    std::vector<glm::vec2> all_points;
    for(std::vector<myline*>::iterator it = lines.begin(); it != lines.end(); ++it){
        myline *l = *it;
        all_points.push_back(glm::vec2(l->x1, l->y1));
    }
    return all_points;
}

std::vector<myline*> polygon::get_green_lines(){
    std::vector<myline*> green_lines;
    for(std::vector<myline*>::iterator it = lines.begin(); it != lines.end(); ++it){
        myline *t = *it;
        if(t->label.compare("green") == 0){
            green_lines.push_back(t);
        }
    }
    return green_lines;
}

// returns the three points of the starting plane
std::vector<glm::vec3> polygon::get_plane_points(polygon *p, myline * line, std::vector<glm::vec2> corres_2d, std::vector<glm::vec3> corres_3d){
    std::vector<glm::vec3> po;
    std::vector<myline*> lines = this->lines;
    myline *line2;  // get the next line
    
    int i=0;
    for(i=0;i<lines.size();++i){
        if(lines[i]->is_equal_to(line) || lines[i]->is_reverse_of(line)){
            break;
        }
    }
    
    
    if(i ==lines.size()-1){
        line2 = lines[0];
    }
    else{
        line2 = lines[i+1];
    }
    
    // first two points will be rotation axis
    for(int i=0;i<corres_2d.size();++i){
        if(line->x1 == corres_2d[i][0] && line->y1 == corres_2d[i][1]){
            po.push_back(corres_3d[i]);
        }
        if(line->x2 == corres_2d[i][0] && line->y2 == corres_2d[i][1]){
            po.push_back(corres_3d[i]);
        }
    }
    for(int i=0;i<corres_2d.size();++i){
        if(line2->x2 == corres_2d[i][0] && line2->y2 == corres_2d[i][1]){
            po.push_back(corres_3d[i]);
        }
    }
    
    return po;
}


// returns the adjacent polygon of this polygon using the huffman labels
// i.e. return polygons attached by a green line
std::vector<polygon*> polygon::get_adjacent_polygons_using_huffman(std::vector<polygon*> all_polygons,
                                                                   std::vector<glm::vec2> corres_2d,
                                                                   std::vector<glm::vec3> corres_3d){
    std::vector<polygon*> adjacent_polygons;
    
    // get the convex lines
    std::vector<myline*> green_lines = this->get_green_lines();
    
    // get all polygons which share edge using green lines
    for(std::vector<polygon*>::iterator it = all_polygons.begin(); it != all_polygons.end(); ++it){
        polygon * pp = *it;
        std::vector<myline*> plines  = pp->lines;
        
        for(std::vector<myline*>::iterator itb = plines.begin(); itb != plines.end(); ++itb){
            myline * lp = *itb;
            
            bool flag = false;
            //if it shares any green line with the current polygon lines then push it
            for(std::vector<myline*>::iterator ita = green_lines.begin(); ita != green_lines.end(); ++ita){
                myline *lpp = *ita;
                if((lp == lpp || lp->reverse_line == lpp) && !this->is_equal_to(pp)){
                    
                    if(!pp->axis_assigned){
                        // calculate the rotation axis for this adjacent polygon
                        std::vector<glm::vec3> plane_points = get_plane_points(pp, lpp, corres_2d, corres_3d);
                        
                        // get normalized rotation axis
                        printf("TESTING BUG 50 start %f\n", plane_points[0][0]);
                        glm::vec3 rotation_axis = plane_points[0]-plane_points[1];
                        printf("TESTING BUG 50 end\n");
                        rotation_axis = rotation_axis/glm::length(rotation_axis);
                        
                        
                        plane * np = new plane(plane_points);
                        
                        pp->rotation_axis    = rotation_axis;
                        pp->plane_to_project = np;
                        pp->axis_assigned    = true;
                    }
                    
                    adjacent_polygons.push_back(pp);
                    flag = true;
                    break;
                }
            }
            
            if(flag){
                break;
            }
        }
    }
    
    
    return adjacent_polygons;
}

// returns the plane rotated by angle_p across rotation axis
plane * polygon::get_plane_with_angle(float angle_p, plane *global_plane_to_project){
    plane *temp;
    glm::vec3 axis;
    
    if(this->axis_assigned){
        axis = this->rotation_axis;
        temp = this->plane_to_project->rotate_it(-1*angle_p, axis[0], axis[1], axis[2]);
    }
    else{
        temp = global_plane_to_project->rotate_it(-1*angle_p, 0, 1, 0);
    }
    
    return temp;
}

// returns the points of polygon projected onto a plane
// with minimum cost angle by iterating over all angles
std::pair<std::vector<glm::vec3>, float> polygon::get_min_cost_angle_points(std::vector<glm::vec2> corres_2d,
                                  std::vector<glm::vec3> corres_3d,
                                  std::vector<std::pair<int, int>> edges_list,
                                  plane *global_plane_to_project, cost* cost_obj){
    
    polygon *current_polygon_p = this;
    
    std::vector<glm::vec4> points_in_camera;
    std::vector<glm::vec3> points_to_render_vec_global;
    std::vector<glm::vec3> points_to_render_vec;
    
    plane *temp;
    plane *final_plane = NULL;
    
    glm::vec3 axis;
    float angle_p = 0;
    
    // Camera matrix
    glm::mat4 View = glm::lookAt(
                                 glm::vec3(-5,5,5), // Camera is at (4,3,3), in World Space
                                 glm::vec3(0,0,0),  // and looks at the origin
                                 glm::vec3(0,1,0)   // Head is up (set to 0,-1,0 to look upside-down)
                                 );
    
    
    glm::mat4 ViewI = glm::inverse(View);
    
    float min_cost  = 1000000;
    float min_angle = 0;
    
    for(int i = 1;i<180; ++i){
        angle_p = i;
        
        plane *temp = get_plane_with_angle(angle_p, global_plane_to_project);
        
        // always clear points before pushing new one
        points_in_camera.clear();
        
        // points in the camera reference
        std::vector<mypoint*>polygon_points  = current_polygon_p->get_points();
        for(int i=0;i<polygon_points.size();++i){
            points_in_camera.push_back(glm::vec4(polygon_points[i]->x, polygon_points[i]->y, 0, 1));
        }
        
        // get world coordinates for these points
        for(int i=0;i<points_in_camera.size();++i){
            points_in_camera[i] = ViewI*points_in_camera[i];
        }
        
        points_to_render_vec       = temp->project_polygon(points_in_camera, -5, 5, 5);
        
        std::vector<glm::vec2> p2d = current_polygon_p->get_points_vec();
        
        float axis_alignment_cost = cost_obj->axis_alignment(p2d, points_to_render_vec);
        float parallelism_cost    = cost_obj->parallelism(p2d, points_to_render_vec, edges_list, corres_2d, corres_3d);
        float total_cost          = axis_alignment_cost + parallelism_cost;
        
        if(min_cost > total_cost){
            
            min_cost  = total_cost;
            min_angle = angle_p;
            points_to_render_vec_global = points_to_render_vec;
            final_plane = temp;
        }
        
        printf("angle is %f alignment, parallelism, total is %f, %f, %f\n",
               angle_p, axis_alignment_cost, parallelism_cost, total_cost);
    }
    
    
    
    
    this->plane_to_project = final_plane;
    return std::make_pair(points_to_render_vec_global, min_angle);
}
