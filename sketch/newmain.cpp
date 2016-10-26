/*
 * PRANJAL SAHU
 *
 *
 *
 * Refer
 * 1. http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/
 * 2. https://web.archive.org/web/20160322113207/http://opencv-code.com/quick-tips/implementation-of-thinning-algorithm-in-opencv/
 * 3. http://opencvexamples.blogspot.com/2013/10/harris-corner-detection.html
 * */

#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif


#ifndef NEWMAIN_H
#define NEWMAIN_H


#include <queue>
#include <list>
#include <vector>
#include <fstream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <queue>
#include <list>
#include <math.h>
#include "myline.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "myutilities.h"

using namespace cv;
using namespace std;


int godisp = 1;
// for right click menu
void createmymenua(void);
void menu(int value);
void disp(void);

int merge_iteration =0;

static int winmenu;
static int menuid;
static int val = 0;

int sa_width  = 800;
int sa_height = 800;

//flags to be used by keyboard for three different operations
int count     = 0;
int entertext = 0;
int drawline  = 0;
int cropimage = 0;
int drawpoly  = -1;		// get points of polygon
int donepoly  = 0;		// stop taking points for polygon
int polytype  = 0;		// 1 means y polygon and 2 means star polygon

char etext[100];

Mat imga, bw, img;
Mat dst_norm_scaled;

class mypointa{
public:
    int x;
    int y;
    mypointa(int a, int b);
    mypointa(mypointa *t);
    mypointa();
};
mypointa starmean  = new mypointa(0, 0);		// star polygon mean coordinate
// also for convex hull

mypointa::mypointa(int a,int b){
    x = a;
    y = b;
}
mypointa::mypointa(mypointa *t){
    x = t->x;
    y = t->y;
}
mypointa::mypointa(){
    x = 0;
    y = 0;
}



// 2d hash for storing lines at each pixel
std::map<i2tuple, myline*> all_lines;
std::map<i2tuple, int> map_slopes;
std::vector<myline*> all_lines_to_merge;
std::vector<myline*> all_lines_created;
vector<i2tuple> points_vector;
vector<i2tuple> corner_points;      // stores the corner points obtained from harris corner



#define PI 3.14159265


std::list<mypointa*> poly1;			// for storing the points of polygon
std::list<mypointa*> poly2;
std::list<mypointa*> convexpoly;		// for storing the points of convex polygon
std::list<mypointa*> maskpoly;		// final list of points which are within the polygon

int minpx = 100000;					// for polygon bounding box
int minpy = 100000;
int maxpx = -1;
int maxpy = -1;

std::queue<mypointa*> myq;
std::queue<mypointa*> myq1;

int win1;
int win2 = -1;

#define MAXSIZE 1000
unsigned int w, h, d;
FILE *fp;

int px[10], py[10];
int click = 0, drawme = 0, dragging  =0;
unsigned char *image2;
unsigned char myimage[MAXSIZE][MAXSIZE][3];
unsigned char original[MAXSIZE][MAXSIZE][3];
unsigned char formaskpoly[MAXSIZE][MAXSIZE][3];			// for storing the inside pixels of a polygon

int newwidth, newheight;

int polysize;						// for storing the size of the polygon
mypointa points[MAXSIZE];
std::queue<int> p1;					// for intersection points x coordinate

std::list<int> scanq[1000];			// one queue for each scan line

void myclear(){
    for(int i=0;i<1000;++i)
        scanq[i].clear();
    return;
}


void drawpoint(int x, int y, int color){
    y = h-y;
    glPointSize(10);
    if(color == 1)
        glColor3f(0, 1, 0);
    else
        glColor3f(0, 0, 1);
    
    glBegin(GL_POINTS);
    glVertex2f(-1+(x*2)/(w+0.0),-1+(2*y)/(h+0.0));
    glEnd();
    glutSwapBuffers();
}

void display1(void){
    glDrawPixels(newwidth, newheight, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)image2);
    glutSwapBuffers();
}

bool isleft(mypointa *a, mypointa *b, mypointa *c){
    return ((b->x - a->x)*(c->y - a->y) - (b->y - a->y)*(c->x - a->x)) > 0;
}

bool isequal(mypointa *a, mypointa *b){
    return (a->x==b->x)&&(a->y==b->y);
}

void copyit(mypointa *a, mypointa *b){
    a->x = b->x;
    a->y = b->y;
}

void reset(){
    entertext = 0;
    cropimage = 0;
    drawline  = 0;
    drawpoly  = 0;
    donepoly  = 0;
    unsigned char tempimage[w][h][3];    
    unsigned int a;
    int i, j;
    
    for(i=0;i<w;++i){
        for(j=0;j<h;++j){
            tempimage[i][j][0] = original[i][j][0];			
            tempimage[i][j][1] = original[i][j][1];
            tempimage[i][j][2] = original[i][j][2];
            myimage[i][j][0]   = original[i][j][0];
            myimage[i][j][1]   = original[i][j][1];
            myimage[i][j][2]   = original[i][j][2];
        }
    }	 
    glRasterPos2f(-1, -1);
    glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)tempimage);
    glutSwapBuffers();
}


void thinningIteration(cv::Mat& im, int iter)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);
    
    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);
            
            int A  = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
            (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
            (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
            (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
            int B  = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
            int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
            int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);
            
            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }
    
    im &= ~marker;
}

void thinning(cv::Mat& im)
{
    im /= 255;
    
    Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
    cv::Mat diff;
    
    do {
        thinningIteration(im, 0);
        thinningIteration(im, 1);
        cv::absdiff(im, prev, diff);
        im.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);
    
    im *= 255;
}

void get_transpose(Mat& im, Mat& im2){
    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            im2.at<uchar>(i, j) = im.at<uchar>(j, i);
        }
    }
}

void get_positive_y(Mat &im){
    Mat temp = cv::Mat::zeros(im.size(), CV_8UC1);
    
    for(int i=1;i < im.rows-1;++i){
        for(int j=1;j< im.cols-1;++j){
            temp.at<uchar>(i, im.cols-j) = im.at<uchar>(i, j);
        }
    }
    
    for(int i=1;i < im.rows-1;++i){
        for(int j=1;j< im.cols-1;++j){
            im.at<uchar>(i, j) = temp.at<uchar>(i, j);
        }
    }
    
    return;
}

void merge_horizontal_and_vertical_lines(){
    
}


int get_max_slope(int x, int y, std::vector<i2tuple> points_vector, Mat& im){
    int max_slope = 0;
    int max_count  = -1;
    
    for(int i=0; i<=180; i = i+SLOPE_ITER_DIFF){
        int theta = i;
        float m = tan(theta);
        
        // get a sweeping line
        myline *a = new myline(x, y, m);
        
        // instead of sloving for all intersection we will try only for a line segment
        // and therefore look for in neighbour hood of 10 pixels
        // and therefore only 400 pixel entries to check
        int count = 0;
        int startx = mymax(0, x-10);
        int endx   = mymin(im.rows, x+10);
        int starty = mymax(0, y-10);
        int endy   = mymin(im.cols, y+10);
        
        for(int i=startx;i<endx;++i){
            for(int j=starty;j<endy;++j){
                if(im.at<uchar>(i, j) > 120 && a->checkpointlies(i, j) == 1){
                    count = count +1;
                }
            }
        }
        
        delete(a);
        
        if(count >= max_count){
            max_count = count;
            max_slope = theta;
        }
    }
    printf("%d, %d\n", x, y);
    return max_slope;
}

int get_max_slopea(int x, int y, std::vector<i2tuple> points_vector){
    int max_slope = 0;
    int max_count = -1;
    
    for(int i=0; i<=180; i = i+SLOPE_ITER_DIFF){
        int theta = i;
        float m = tan(theta);
        
        // get a sweeping line
        myline *a = new myline(x, y, m);
        
        int count = 0;
        for(std::vector<i2tuple>::iterator it = points_vector.begin(); it != points_vector.end(); ++it){
            i2tuple pt = *it;
            if ( a->checkpointlies(std::get<0>(pt), std::get<1>(pt)) == 1){
                count = count+1;
            }
        }
        
        delete(a);
        
        if(count >= max_count){
            max_count = count;
            max_slope = theta;
        }
    }
    printf("%d, %d\n", x, y);
    return max_slope;
}

// fills the points_vector
void fill_points_vector(Mat& im){
    // get all the non zero pixels
    if(points_vector.size() == 0){
        for(int i=1; i<im.rows; ++i){
            for(int j=1;j<im.cols;++j){
                if(im.at<uchar>(i, j) > 120){
                    points_vector.push_back(i2tuple(i, j));
                }
            }
        }
    }
    
    return;
}

// pass the matrix and perform the sweepline algorithm
std::map<i2tuple, int> sweepline(Mat& im){
    
    fill_points_vector(im);
    
    // stores the slopes for each pixel
    std::map<i2tuple, int> all_line_slopes;
    
    // for each positive pixel
    for(std::vector<i2tuple>::iterator it = points_vector.begin(); it != points_vector.end(); ++it) {
        i2tuple pt = *it;
        all_line_slopes[pt] = get_max_slope(get<0>(pt), get<1>(pt), points_vector, im);
    }
    
    return all_line_slopes;
}

float mytan(int degree){
    return tan(degree * PI/180.0);
}


//Initializes 3D rendering
void initRendering() {
    glEnable( GL_POINT_SMOOTH );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); //Enable lighting
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Automatically normalize normals
    
    //glShadeModel(GL_SMOOTH); //Enable smooth shading
}

void updatea(int value) {
    glutPostRedisplay();
    glutTimerFunc(25, updatea, 0);
}


void displayone() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)
    
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f } };
    
    glBegin( GL_POINTS );
    glColor3f(1.0f, 0.0f, 0.0f);
    
    for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
        i2tuple key = iterator->first;
        GLfloat px = get<0>(key)*1.0/sa_width;
        GLfloat py = get<1>(key)*1.0/sa_height;
        glVertex2f(px, py);
    }
    glEnd();
    
    
    for(std::vector<myline*>::iterator iterator = all_lines_to_merge.begin(); iterator != all_lines_to_merge.end(); iterator++) {
        myline * linet = *iterator;
        
        if (linet->x2 == infvalue || linet->y2 ==infvalue){
            glBegin( GL_POINTS );
            glColor3f(1.0f, 0.0f, 0.0f);
            GLfloat px = (linet->x1-100)*IMG_SCALE/sa_width;
            GLfloat py = (linet->y1-100)*IMG_SCALE/sa_height;
            glVertex2f(px, py);
            glEnd();
            continue;
        }

        glBegin(GL_LINE_LOOP);
        
        int r = 1;//rand()%4;
        glColor3f(colors[r][0], colors[r][1], colors[r][2]);
        
        GLfloat px = (linet->x1+10)*IMG_SCALE/sa_width;
        GLfloat py = (linet->y1+10)*IMG_SCALE/sa_height;
        
        GLfloat qx = (linet->x2+10)*IMG_SCALE/sa_width;
        GLfloat qy = (linet->y2+10)*IMG_SCALE/sa_height;
        
        glVertex2f(px, py);
        glVertex2f(qx, qy);
        
        glEnd();
    }

    
    
    
    for(std::vector<i2tuple>::iterator iterator = corner_points.begin(); iterator != corner_points.end(); iterator++) {
        i2tuple p = *iterator;
        
        glBegin( GL_POINTS );
        glColor3f(0.0f, 1.0f, 0.0f);
        GLfloat px = (get<0>(p)-100)*IMG_SCALE/sa_width;
        GLfloat py = (get<1>(p)-100)*IMG_SCALE/sa_height;
        glVertex2f(px, py);
        glEnd();
    }

    
    
    
    //Drawing all_lines_created
//    for(std::vector<myline*>::iterator iterator = all_lines_created.begin(); iterator != all_lines_created.end(); iterator++) {
//        myline * linet = *iterator;
//        glBegin(GL_LINE_LOOP);
//        
//        int r = 0;//rand()%4;
//        glColor3f(colors[r][0], colors[r][1], colors[r][2]);
//        
//        GLfloat px = (linet->x1+50)*IMG_SCALE/sa_width;
//        GLfloat py = (linet->y1+50)*IMG_SCALE/sa_height;
//        
//        GLfloat qx = (linet->x2+50)*IMG_SCALE/sa_width;
//        GLfloat qy = (linet->y2+50)*IMG_SCALE/sa_height;
//        
//        glVertex2f(px, py);
//        glVertex2f(qx, qy);
//        
//        glEnd();
//    }
    
    
    
 //       }
    glFlush();  // Render now
}

//Called when the window is resized
void handleResize(int w, int h) {
    //s_width  = w;
    //s_height = h;
    
    glViewport(0, 0, w, h);
    glEnable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}



struct sortbyxasc {
    bool operator()(const myline* o1, const myline* o2) const {
        //return 100*(std::abs(o1->m-o2->m))+0.5*(o1->get_distance(o2));
        return o1->get_distance(o2);
    }
};

// short length lines will be merged first
struct sortbylength {
    bool operator()(const myline* o1, const myline* o2) const {
        return (o1->get_line_length() - o2->get_line_length() > 0);
    }
};

// sort from left to right
struct sortfromlefttoright {
    bool operator()(const myline* o1, const myline* o2) const {
        if(o1->x1 == o2->x1){
            return o1->y1-o2->y1;
        }
        else
            return o1->x1 < o2->x1;
    }
};


// gives the line which is nearest to the point by doing exhaustive search
// over all points
int get_min_line_to_merge(myline *current_line){
    std::vector<myline*>::iterator iterator = all_lines_to_merge.begin();
    std::vector<myline*>::iterator iterator_ret;
    
    float distance_difference;
    myline *min_prev;
    
    float diff_total = 100000;
    
    for(std::vector<myline*>::iterator iter = iterator; iter != all_lines_to_merge.end(); iter++){
        myline *prev_line = *iter;
        distance_difference = current_line->get_distance(prev_line);
        
        if(distance_difference < diff_total){
            diff_total   = distance_difference;
            min_prev     = prev_line;
            iterator_ret = iter;
        }
    }
    
    
    return iterator_ret-all_lines_to_merge.begin();
}


int get_line_to_merge(myline *current_line){
    std::vector<myline*>::iterator iterator = all_lines_to_merge.begin();
    float slope_difference;
    float distance_difference;
    myline *min_prev;
    
    float diff_total = 100000;
    float min_slope = 100000;
    float min_distance = 1000000;
    
    for(std::vector<myline*>::iterator iter = iterator; iter != all_lines_to_merge.end(); iter++){
        myline *prev_line = *iter;
        
         slope_difference    = fabs(prev_line->m-current_line->m);
         distance_difference = current_line->get_distance(prev_line);
        
        if(slope_difference + distance_difference < diff_total){
            min_slope = slope_difference;
            min_distance = distance_difference;
            diff_total = slope_difference + distance_difference;
            min_prev = prev_line;
        }
        
        if(slope_difference <= SLOPE_DIFFERENCE &&  distance_difference < DISTANCE_DIFFERENCE*(5+merge_iteration)/5.0){
            return iter-all_lines_to_merge.begin();
        }
    }
    

    return -1;
}



void fill_merge_lines_data(){
    for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
        i2tuple key = iterator->first;
        int x = get<0>(key);
        int y = get<1>(key);
        int theta   = iterator->second;
        all_lines_to_merge.push_back(new myline(x, y, mytan(theta)));
    }
}

int check_if_already_covered_point(int x, int y){
    for(std::vector<myline*>::iterator iterator = all_lines_created.begin(); iterator != all_lines_created.end(); iterator++) {
        myline *l = *iterator;
        if((x >= l->x1 && x <= l->x2) || (x >= l->x2 && x <= l->x1)){
            if((y >= l->y1 && y <= l->y2) || (y >= l->y2 && x <= l->y1)){
                int flag = l->checkpointlies(x, y);
                if(flag == 1){
                    return 1;
                }
            }
        }
    }
    return 0;
}

// merges if the sweep angles are 180 apart
// we will check if the sum of slope of at two points is approx 180
void mergelinesa(Mat &im){
    std::vector<i2tuple>::iterator iterator = points_vector.begin();
    
    for(std::vector<i2tuple>::iterator iter = iterator; iter != points_vector.end(); ++iter){
        i2tuple current_tuple = *iter;
        int x = get<0>(current_tuple);
        int y = get<1>(current_tuple);
        
        if(check_if_already_covered_point(x, y) == 1){
            // remove this point and continue
            iter = points_vector.erase(iter);
        }
        
        int startx = mymax(0, x-10);
        int endx   = mymin(im.rows, x+10);
        int starty = mymax(0, y-10);
        int endy   = mymin(im.cols, y+10);
        
        for(int i=startx;i<endx;++i){
            for(int j=starty;j<endy;++j){
                float sum_slope = map_slopes[i2tuple(i, j)]+map_slopes[i2tuple(x, y)];
                //if((sum_slope < 190 && sum_slope > 170 ) || (sum_slope < 10 && sum_slope > 0)){
                if(((sum_slope  == 0 ) || (sum_slope == 180)) && get_point_distance(x, y, i, j) > 8){
                    myline *a = new myline(i, x, j, y);
                    all_lines_created.push_back(a);
                    //remove_all_lying_points(a, startx, starty, endx, endy);
                }
            }
        }
    }
    return;
}


// in iteration merges all the lines present in the all_lines_to_merge
int mergelines(int force){
    // In first iteration we only merge lines with same x or same y and adjacent to each other
    int checkiter = 1;
    int merged_count = 0;
    merge_iteration++;
    
    //printf("LINES TO MERGE %d\n", all_lines_to_merge.size());
    
    if(all_lines_to_merge.size() == 0){
        checkiter = 0;
        fill_merge_lines_data();
    }
    
    if(merge_iteration > 1){
        std::list<myline*> temp{ std::begin(all_lines_to_merge), std::end(all_lines_to_merge) };
        temp.sort(sortbylength());
        std::vector<myline*> tempa{ std::begin(temp), std::end(temp) };
        
        // sort lines to merge by their length
        all_lines_to_merge = tempa;
    }
    
    std::vector<myline*>::iterator iterator = all_lines_to_merge.begin();
    
    for(std::vector<myline*>::iterator iter = iterator; iter != all_lines_to_merge.end();){
        myline* current_line = *iter;
        // only consider small lines when force merge
        if(force == 1){
            if(current_line->get_line_length() >= 2){
                iter++;
                continue;
            }
        }
        
        
        //printf("%d %d %d %d \n", current_line->x1, current_line->y1, current_line->x2, current_line->y2);
        
        // temporary delete current line
        iter  = all_lines_to_merge.erase(iter);
        
        // if the merge line is found then well and good else push it just behind current iterator
        int index;
        if(force == 0)
            index = get_line_to_merge(current_line);
        else
            index = get_min_line_to_merge(current_line);
        
        if(index == -1){
            iter = all_lines_to_merge.insert(iter, current_line);
            iter++;
            continue;
        }
        
        std::vector<myline*>::iterator iterator_prev = all_lines_to_merge.begin();
        iterator_prev = iterator_prev+index;
        
        // other line to be deleted
        myline *prev_line = *iterator_prev;
        
        // create a new line
        myline* line_to_insert;
        
        if (current_line->x2 == infvalue && prev_line->y2 == infvalue){
            
            int x1 = mymin(prev_line->x1, current_line->x1);
            int x2 = mymax(prev_line->x1, current_line->x1);
            int y1 = mymin(prev_line->y1, current_line->y1);
            int y2 = mymax(prev_line->y1, current_line->y1);
            
            line_to_insert = new myline(x1, x2, y1, y2);
        }
        else{
            // now 2 cases arrive
            // one line is a point and other is line segment
            //      OR
            // both are line segment
            int x1, y1, x2, y2;
            
            // one is a point
            if(current_line->x2 == infvalue){
                x1 = mymin(mymin(current_line->x1, prev_line->x1), prev_line->x2);
                x2 = mymax(mymax(current_line->x1, prev_line->x1), prev_line->x2);
                y1 = mymin(mymin(current_line->y1, prev_line->y1), prev_line->y2);
                y2 = mymax(mymax(current_line->y1, prev_line->y1), prev_line->y2);
            }
            // two is a point
            else if(prev_line->x2 == infvalue){
                x1 = mymin(mymin(current_line->x1, prev_line->x1), current_line->x2);
                x2 = mymax(mymax(current_line->x1, prev_line->x1), current_line->x2);
                y1 = mymin(mymin(current_line->y1, prev_line->y1), current_line->y2);
                y2 = mymax(mymax(current_line->y1, prev_line->y1), current_line->y2);
            }
            // both are line segment
            else{
                // create a new line
                x1 = mymin(mymin(current_line->x1, prev_line->x1), mymin(current_line->x2, prev_line->x2));
                x2 = mymax(mymax(current_line->x1, prev_line->x1), mymax(current_line->x2, prev_line->x2));
                y1 = mymin(mymin(current_line->y1, prev_line->y1), mymin(current_line->y2, prev_line->y2));
                y2 = mymax(mymax(current_line->y1, prev_line->y1), mymax(current_line->y2, prev_line->y2));
            }
            
            // create a new line
            line_to_insert = new myline(x1, x2, y1, y2);
        }
        
        
        // delete the two lines and insert a new one
        all_lines_to_merge.erase(iterator_prev);
        all_lines_to_merge.push_back(line_to_insert);
        ++merged_count;
        
    }
    
    return merged_count;
    
}




// merges the points until no change is observed in the point set
int merge_points(){
    int count  = 0;
    printf("size of corner_points START %d\n", corner_points.size());
    
    vector<i2tuple> corner_points_temp;
    vector<tuple<i2tuple, i2tuple>> corner_points_to_merge;
    
    for(std::vector<i2tuple>::iterator it = corner_points.begin(); it != corner_points.end(); ++it){
        i2tuple pt = *it;
        int px1 = get<0>(pt);
        int py1 = get<1>(pt);
        int px2 = -1;
        int py2 = -1;
        
        float mind = 10000;
        std::vector<i2tuple>::iterator minp = it+1;
        
        for(std::vector<i2tuple>::iterator ita = it+1; ita != corner_points.end(); ++ita){
            i2tuple pta = *ita;
            int pxa = get<0>(pta);
            int pya = get<1>(pta);
            
            if(px1 == pxa && py1 == pya){
                continue;
            }
            
            int flag = 0;
            float d = get_point_distance(px1, py1, pxa, pya);
            if (d < mind){
                mind = d;
                minp = ita;
                px2 = pxa;
                py2 = pya;
            }
        }
        
        if(mind < 12.0){
            // replace the merged point with the new formed point
            corner_points[minp-corner_points.begin()] = i2tuple((px1+px2)/2, (py1+py2)/2);
            // erase the current point
            it = corner_points.erase(it);
            count  =count+1;
        }
        
    }
    
    printf("size of corner_points END %d\n", corner_points.size());
    return count;
}


//Called when a key is pressed
void handleKeypressa(unsigned char key, int x, int y) {
    switch (key) {
        case 'i':
            mergelines(0);
            break;
        case 'f':
            mergelines(1);
            break;
        case 'a':
            mergelinesa(img);
            break;
        case 'm':
            merge_points();
            break;
        case 27: //Escape key
            exit(0);
    }
}

// gets the corner points using the harris detector
// and clubs the points which are nearby into one
// These points will be used to
void get_corner_points(Mat &imga){
    Mat dst, dst_norm;
    dst = Mat::zeros( imga.size(), CV_32FC1 );
    
    cornerHarris( imga, dst, 5, 5, 0.05, BORDER_DEFAULT );
    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( dst_norm, dst_norm_scaled );
    
    
    // store it in corner points
    for( int j = 0; j < dst_norm.rows ; j++ ){
        for( int i = 0; i < dst_norm.cols; i++ ){
            if( (int) dst_norm.at<float>(j,i) > HARRIS_THRESH ){
                corner_points.push_back(i2tuple(j, i));
            }
        }
    }
    return;
}


void plotpoint(i2tuple pt){
    int i = get<0>(pt);
    int j = get<1>(pt);
    circle( dst_norm_scaled, Point( j, i ), 5,  Scalar(0, 255, 0), 2, 8, 0 );
    return;
}

int main(int argc, char** argv){
//            std::vector<i2tuple> lp;
//            std::vector<i2tuple>::iterator iter = lp.begin();
//    
//    lp.push_back(i2tuple(1, 1));
//    lp.push_back(i2tuple(1, 6));
//    lp.push_back(i2tuple(1, 8));
//    lp.push_back(i2tuple(1, 9));
//    iter = lp.begin();
//    ++iter;
//    
//    lp[iter-lp.begin()] = i2tuple(100,100);
//    printf("pranal tsting");
    
//        std::vector<int> lp;
//        std::vector<int>::iterator iter = lp.begin();
//    
//        lp.push_back(5);
//        lp.push_back( 6);
//        lp.push_back( 7);
//        lp.push_back( 8);
//        lp.push_back( 9);
//        lp.push_back( 10);
//    
//        iter = lp.begin();
//    
//        std::vector<int>::iterator itera = iter;
//        itera = itera+1;
//    printf("%d ", *itera);
//        iter = lp.erase(iter);
//    
//    printf("%d\n", iter-lp.begin());
//    printf("%d\n", itera-lp.begin());
//    printf("%d ", *iter);
//    printf("%d ", *itera);
    
//        iter  = lp.begin();
//        iter++;
//    
//        lp.erase(iter+3);
//        lp.erase(iter);
//    
//        lp.insert(iter, 100);
//
//        iter++;
//        printf("%d\n", *iter);
//        iter++;
//        printf("%d\n", *iter);
//        iter++;
//        printf("%d\n", *iter);
//        iter++;
//        printf("%d\n", *iter);

//
//        printf("%d \n", *iter);
//        iter++;
//        printf("%d \n", *iter);
//        iter++;
//        printf("%d \n", *iter);
//        iter++;
//        printf("%d \n", *iter);
//        iter++;
//    printf("%d \n", *iter);
//    iter++;
//    if(iter == lp.end()){
//        printf("pranjal");
//    }
//    printf("%d \n", *iter);
//    iter++;
//    printf("%d \n", *iter);
//    iter++;
//    printf("%d \n", *iter);
//    iter++;
    
    
    
    
    
    
    
    
    
    
    
    imga = imread("/Users/pranjal/Desktop/img9.jpeg", CV_LOAD_IMAGE_GRAYSCALE);
    bw   = imga > 128;
    img  = bw > 120;
    
    // get corner points using harris detector
    get_corner_points(imga);
    
    while(merge_points() != 0){
        merge_points();
    }
    
    bitwise_not(bw, img);
    thinning(img);
    //map_slopes = sweepline(img);

//    for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
//        i2tuple key = iterator->first;
//        int theta = iterator->second;
//        printf("(%d, %d)  => %d\n", get<0>(key), get<1>(key), theta);
//    }
//    
//    
//    glutInit(&argc, argv);                 // Initialize GLUT
//    glutInitWindowSize(sa_width, sa_height);
//
//    
//    glutCreateWindow("OpenGL Setup Test");  // Create a window with the given title
//    //glutInitWindowSize(320, 320);         // Set the window's initial width & height
//    glutInitWindowPosition(50, 50);         // Position the window's initial top-left corner
//    glutDisplayFunc(displayone);            // Register display callback handler for window re-paint
//    glutKeyboardFunc(handleKeypressa);
//    glutTimerFunc(25, updatea, 0);
//
//    glutMainLoop();           // Enter the event-processing loop
    

    
    
//
    
    
    
    
    
    
    // i2tuple p1 = i2tuple(1, 1);
    
    //i2tuple p2 = i2tuple(1, 3);
    //all_lines[p1] = new myline(1, 6, 5);
    //all_lines[p2] = new myline(1, 9, 53);
    
//    
//    
//    printf("rows = %d cols = %d\n", img.rows, img.cols);
//    //IplImage* img = cvLoadImage( "/Users/pranjal/Desktop/img2_2.png" , CV_LOAD_IMAGE_GRAYSCALE);
//    cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE );
//    namedWindow( "sketch", WINDOW_AUTOSIZE );// Create a window for display.
//    imshow( "sketch", img );                   // Show our image inside it.
//
//    
//    
//    cvWaitKey(0);
//    cvDestroyWindow("Example1");
    
    
    
    
    Mat dst, dst_norm;
    dst = Mat::zeros( imga.size(), CV_32FC1 );
    
    cornerHarris( imga, dst, 5, 5, 0.05, BORDER_DEFAULT );
    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( dst_norm, dst_norm_scaled );
    
    
    for(std::vector<i2tuple>::iterator it = corner_points.begin(); it != corner_points.end(); ++it){
        i2tuple pt = *it;
        int i = get<0>(pt);
        int j = get<1>(pt);
        //circle( dst_norm_scaled, Point( j, i ), 5,  Scalar(0, 255, 0), 2, 8, 0 );
        printf("(%d, %d)\n", i, j);
    }

    //        i2tuple pt = *it;
    //        int i = get<0>(pt);
    //        int j = get<1>(pt);

    
    myline *ml = new myline(corner_points[18], corner_points[19]);
    printf("slope is %f x1 = %d, y1 = %d x2 = %d y2 = %d\n", ml->m, ml->x1, ml->y1, ml->x2, ml->y2);
    plotpoint(corner_points[18]);
    plotpoint(corner_points[19]);
    
    fill_points_vector(img);
    
    //printf("COUNT OF POINT LYING %d\n", ml->pointliecount(points_vector));
    
    std::vector<i2tuple> pp = ml->pointliecount(points_vector);
    
    printf("SIZE OF POINT VECTOR IS %d LENGTH OF LINE IS %f \n", pp.size(), ml->get_line_length());
    
    for(std::vector<i2tuple>::iterator it = pp.begin(); it != pp.end(); ++it){
        i2tuple pt = *it;
        int i = get<0>(pt);
        int j = get<1>(pt);
        circle( dst_norm_scaled, Point( j, i ), 2,  Scalar(0, 255, 0), 2, 8, 0 );
        //printf("(%d, %d)\n", i, j);
    }
    
    // Showing the result
    namedWindow( "corners_window", CV_WINDOW_AUTOSIZE );
    imshow( "corners_window", dst_norm_scaled );
    waitKey(0);

    return 0;
}
#endif
