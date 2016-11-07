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
#define HARRIS_THRESH 60              // threshold for choosing the corner points using harris
#define MERGE_POINT_THRESH 4.0        // threshold for merging points when
#define PERPENDICULAR_THRESH 10
#define POINT_PAIR_LYING_THRESH 0.95
#define SHOW_CORNER_POINTS 1                    // flag to display points using opengl



#include <opencv2/opencv.hpp>
#include "myutilities.h"
#include "junction.h"

class junction;

class myline{
public:
    // points through which it passes
    int x1, y1;
    int x2, y2;
    
    // label for this line
    std::string label;
    
    float m; // slope
    
    bool is_equal_to(myline *);
    bool is_reverse_of(myline *);
    int checkpointlies(int a, int b);
    std::vector<i2tuple> pointliecount(std::vector<i2tuple> points_vector);
    void mergelines(myline *t);
    int mergeable(myline *t);
    int get_intersection_count(cv::Mat &im);
    float get_perpendicular_distance(i2tuple a);
    bool check_within_line_segment(i2tuple a);
    float get_distance(const myline *ml) const;
    float get_line_length() const;
    myline *get_reverse_line(std::vector<myline*> all_lines);
    std::vector<myline*> get_polygon(std::vector<myline*> all_lines);
    bool check_if_occluding_edge(std::vector<i2tuple> corner_points);
    float get_angle(myline *a, int bx, int by);
    bool label_line(std::vector<myline*> junction_lines, junction *j);
    
    myline(i2tuple a, i2tuple b);
    myline(int a, int b, float m);
    myline(myline *t);
    myline(int a1, int b1, int c1, int d1);
    myline();
};



float triangle_area(myline *first, myline *second);
bool ccw(myline *first, myline *second);
myline* get_next_line(myline *first, std::vector<myline*> valid_lines);
std::vector<myline*> get_all_adjacent_lines(myline *start, std::vector<myline*> all_lines_to_check);
std::vector<int> get_line_labels(std::vector<myline*> all_lines);
std::vector<int> get_huffman_label(std::vector<myline*> valid_lines_directed, std::vector<myline*> valid_lines_undirected, std::vector<i2tuple> corner_points);

#endif /* myline_h */
