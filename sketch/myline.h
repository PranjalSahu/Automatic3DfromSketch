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
#define MY_EPS   0.00001
#define SLOPE_DIFFERENCE 0.01
#define DISTANCE_DIFFERENCE 1.5

#include <opencv2/opencv.hpp>


class myline{
public:
    // points through which it passes
    int x1, y1;
    int x2, y2;
    
    float m; // slope
    
    int checkpointlies(int a, int b);
    void mergelines(myline *t);
    int mergeable(myline *t);
    int get_intersection_count(cv::Mat &im);
    float get_distance(const myline *ml) const;

        
    myline(int a, int b, float m);
    myline(myline *t);
    myline(int a1, int b1, int c1, int d1);
    myline();
};





#endif /* myline_h */
