//
//  cuboid.h
//  sketch
//
//  Created by Pranjal Sahu on 10/19/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef cuboid_h
#define cuboid_h
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#endif /* cuboid_h */

class view;


class cuboid{
public:
    GLfloat dimen[3];   //length, breadth, height
    
    // count of points currently taken
    int current_index = 0;
    
    //
    view* parentview;
    
    // parent cuboid of this cuboid
    cuboid* parent_cuboid;
    
    // flag to denote that input has been taken
    int input_done = 0;
    
    // flag to denote if the cuboid is parent cuboid or a child cuboid
    // for parent cuboid we need 7 points while for child cuboid we only need 4 points
    int cuboid_type;
    
    // x and y coordinate of point of touch
    int mouse_xs[8];
    int mouse_ys[8];
    
    // world x, y and z coordinate for this cuboid
    GLfloat world_x[8];
    GLfloat world_y[8];
    GLfloat world_z[8];
    
    // world x, y, z scaled
    GLfloat world_xs[8];
    GLfloat world_ys[8];
    GLfloat world_zs[8];
    
    // Remember the order of drawing the lenghts for a cuboid
    // current_cuboid(current_cuboid->dimen[2]/cuboid_ratio, current_cuboid->dimen[0]/cuboid_ratio, current_cuboid->dimen[1]/cuboid_ratio);
    // draws the cuboid at origin given its length, breadth and height
    void drawmycuboid(int cuboid_ratio);
    
    // Draws the cuboid using its 3d coordinates and the given cuboid ratio
    void drawmycuboid_coord(int cuboid_ratio);
    
    // gets the height depth and width of cuboid based on the 8 clicked input points
    void get_height_width_depth_of_cuboid(int cuboid_ratio);
    
    // get the 3d coordinates of the cuboid after the input has been taken
    void get_3d_coordinates(int cuboid_ratio);
    
    // called when a new point is clicked for insertion
    void insert_mouse_point(int x, int y);
    
    
    // stores the point in allpoints_x2 for drawing the back face of cuboid
    void get_2nd_face_points(int *mouse_xs2, int *mouse_ys2);
    
};
