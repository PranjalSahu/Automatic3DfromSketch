//
//  myline.h
//  sketch
//
//  Created by Pranjal Sahu on 10/23/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef mylinea_h
#define mylinea_h

#define infslope 10000000
#define infvalue 10000000
#define MY_EPS   0.001
#define SLOPE_DIFFERENCE 0.01
#define DISTANCE_DIFFERENCE 1
#define SLOPE_ITER_DIFF 1
#define HARRIS_THRESH 80
#define MERGE_POINT_THRESH 4.0        # threshold for merging points when
#define PERPENDICULAR_THRESH 3
#define POINT_PAIR_LYING_THRESH 0.9
#define SHOW_CORNER_POINTS 1                    # flag to display points using opengl



#include <opencv2/opencv.hpp>
#include "myutilities.h"

class myline{
public:
    // points through which it passes
    int x1, y1;
    int x2, y2;
    
    float m; // slope
    
    int checkpointlies(int a, int b);
    std::vector<i2tuple> pointliecount(std::vector<i2tuple> points_vector);
    void mergelines(myline *t);
    int mergeable(myline *t);
    int get_intersection_count(cv::Mat &im);
    float get_perpendicular_distance(i2tuple a);
    int check_within_line_segment(i2tuple a);
    float get_distance(const myline *ml) const;
    float get_line_length() const;
    
    myline(i2tuple a, i2tuple b);
    myline(int a, int b, float m);
    myline(myline *t);
    myline(int a1, int b1, int c1, int d1);
    myline();
};



float triangle_area(myline *first, myline *second);
int ccw(myline *first, myline *second);



#endif /* myline_h */
