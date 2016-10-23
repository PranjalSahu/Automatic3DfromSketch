//
//  myline.h
//  sketch
//
//  Created by Pranjal Sahu on 10/23/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef myline_h
#define myline_h

#define infslope 10000000
#define infvalue 10000000
#define MY_EPS      0.00001

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

        
    myline(int a, int b, float m);
    myline(myline *t);
    myline();
};

#endif /* myline_h */
