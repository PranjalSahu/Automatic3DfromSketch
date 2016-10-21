//
//  view.h
//  sketch
//
//  Created by Pranjal Sahu on 10/19/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "myutilities.h"

class mypoint;
class cuboid;

class view{
public:
    int num_cuboid = 0;    // we are starting with only 2 cuboids as input
    int joint_type = 1;     // starting with rotation joint
    
    // two vanishing points per view
    mypoint *vanishing_point[2];
    
    // current cuboid whose input is being taken
    int current_cuboid = -1;
    
    cuboid* get_current_cuboid();
    
    // currently having only two cuboids
    cuboid* cuboids[2];
    
    cuboid *create_cuboid(int parentflag);
    void get_height_width_depth_of_cuboid();
    
    // draws the input lines for a view
    void drawinputlinesforaview();

    // draw input lines for a cuboid
    void drawinputlinesforacuboid(int point_x[], int point_y[], int start, int count);
    
    //drawinputlinesforacuboid_b
    void drawinputlinesforacuboid_b(int point_x[], int point_y[], int start, int count);
    
    // draw a line between two points
    void drawline(int x1, int y1, int x2, int y2);
    
    // takes the input of the first cuboid and saves the vanishing point
    // using the two parallel lines
    void get_vanishing_points();
    
    // get count of cuboids whose input has been taken
    int get_done_cuboids();
    
    //contructor
    view();
};
