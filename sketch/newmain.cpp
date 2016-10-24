/*
 * PRANJAL SAHU
 * 09CS1036
 * Graphics Assignment 2
 *
 * NOTE: I have used plain P3 format of .ppm images
 * Two sample images are also attached in the submission
 *
 *
 * Refer
 * 1. http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/
 * 2. https://web.archive.org/web/20160322113207/http://opencv-code.com/quick-tips/implementation-of-thinning-algorithm-in-opencv/
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
typedef std::tuple<int,int> i2tuple;
std::map<i2tuple, myline*> all_lines;
std::map<i2tuple, int> map_slopes;
std::vector<myline*> all_lines_to_merge;


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

void getminmaxforboundingbox(){
    int i;
    minpx = 100000;
    minpy = 100000;
    maxpx = -1;
    maxpy = -1;
    for(i=0;i<polysize;++i){			// getting bounding box;
        if(points[i].x < minpx)
            minpx = points[i].x;
        if(points[i].y < minpy)
            minpy = points[i].y;
        if(points[i].x > maxpx)
            maxpx = points[i].x;
        if(points[i].y > maxpy)
            maxpy = points[i].y;
    }
    return;
}

int findinvertexset(int x, int y){
    for(int i=0;i<polysize;++i){
        if(points[i].x == x && points[i].y == y){
            if(i==0){
                if((points[0].y-points[1].y)*(points[0].y-points[polysize-1].y) < 0)		// oppsite side
                    return 0;
                else
                    return 1;
            }
            else{
                if((points[i].y-points[i+1].y)*(points[i].y-points[i-1].y) < 0)		// oppsite side
                    return 0;
                else
                    return 1;
            }
        }
        
    }
    return 0;
}

void display1(void){
    glDrawPixels(newwidth, newheight, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)image2);
    glutSwapBuffers();
}

void drawrectangle(){
    newwidth  = px[1]-px[0];
    newheight = py[1]-py[0];
    
    printf("drawing rectangle\n");
    
    image2 = (unsigned char *)malloc(sizeof(unsigned char)*(newwidth+1)*(newheight+1)*3);
    glReadPixels(px[0], w-py[0]-newheight, newwidth, newheight, GL_RGB, GL_UNSIGNED_BYTE, image2);
    
    glutInitWindowSize(newwidth+1, newheight+1);
    win2 = glutCreateWindow("New Image");
    glutDisplayFunc(display1);
    cropimage = 0;
}

void display2(){
    int size = (int)myq.size();
    glRasterPos2f(-1,-1);
    float barwidth = 2/(size+0.0);
    int i, temp;
    float start = -1;
    float pre   = -1;
    
    glBegin(GL_LINE_STRIP);
    for(i=0;i<size;++i){
        temp = original[myq.front()->y][myq.front()->x][0]+original[myq.front()->y][myq.front()->x][1]+original[myq.front()->y][myq.front()->x][2];
        glVertex2f(start, pre);
        glVertex2f(start+barwidth, -1+ 2*temp/768.0);
        //glRectf(start, -1, start+barwidth, -1+ 2*temp/768.0);
        start = start+barwidth;
        pre   = -1+ 2*temp/768.0;
        myq.pop();
    }
    glEnd();
    glutSwapBuffers();
}

void showintensity(){
    if(win2 != -1)
        glutDestroyWindow(win2);
    glClearColor(0, 0, 0, 0);
    glutInitWindowSize(512, 256);					// make window of width = no. of pixels in the queue
    win2 = glutCreateWindow("Intensity profile");
    glutDisplayFunc(display2);
}

void putpixel(){
    int i, j, k;
    unsigned char tempimage[w][h][3];
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, tempimage);
    int size = (int)myq1.size();
    for(i=0;i<size;++i){
        tempimage[myq1.front()->y][myq1.front()->x][0] = (unsigned char)255;
        tempimage[myq1.front()->y][myq1.front()->x][1] = (unsigned char)0;
        tempimage[myq1.front()->y][myq1.front()->x][2] = (unsigned char)0;
        myq1.pop();
    }
    
    for(i=0;i<w;++i){
        for(j=0;j<h;++j){
            for(k=0;k<3;++k){
                myimage[i][j][k] = tempimage[i][j][k];
            }
        }
    }
    glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)tempimage);
    glutSwapBuffers();
    showintensity();
    drawline = 0;
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


int get_max_slope(int x, int y, std::vector<i2tuple> points_vector){
    int max_slope = 0;
    int max_count  = -1;
    
    for(int i=0; i<180; i = i+5){
        int theta = i;
        float m = tan(theta);
        
        // get a sweeping line
        myline *a = new myline(x, y, m);
        
        int count = 0;
        for(std::vector<i2tuple>::iterator it = points_vector.begin(); it != points_vector.end(); ++it){
            i2tuple pt = *it;
            if ( a->checkpointlies(get<0>(pt), get<1>(pt)) == 1){
                count = count+1;
            }
        }
        
        delete(a);
        
        if(count > max_count){
            max_count = count;
            max_slope = theta;
        }
    }
    printf("%d, %d\n", x, y);
    return max_slope;
}

// pass the matrix and perform the sweepline algorithm
std::map<i2tuple, int> sweepline(Mat& im){
    vector<i2tuple> points_vector;
    
    // get all the non zero pixels
    for(int i=1; i<im.rows; ++i){
        for(int j=1;j<im.cols;++j){
            if(im.at<uchar>(i, j) > 120){
                points_vector.push_back(i2tuple(i, j));
            }
        }
    }
    
    // stores the slopes for each pixel
    std::map<i2tuple, int> all_line_slopes;
    
    // for each positive pixel
    for(std::vector<i2tuple>::iterator it = points_vector.begin(); it != points_vector.end(); ++it) {
        i2tuple pt = *it;
        all_line_slopes[pt] = get_max_slope(get<0>(pt), get<1>(pt), points_vector);
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
    
    // Draw a Red 1x1 Square centered at origin
    int sizea = map_slopes.size();
    
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
            GLfloat px = (linet->x1+10)*IMG_SCALE/sa_width;
            GLfloat py = (linet->y1+10)*IMG_SCALE/sa_height;
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
        return (o1->get_line_length() - o2->get_line_length() < 0);
    }
};


int get_line_to_merge(myline *current_line){
    std::vector<myline*>::iterator iterator = all_lines_to_merge.begin();
    float slope_difference;
    float distance_difference;
    myline *min_prev;
    
    float diff_total = 100000;
    float min_slope = 100000;
    float min_distance = 1000000;
    int p1x, p1y, p2x, p2y;
    
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



// in iteration merges all the lines present in the all_lines_to_merge
int mergelines(){
    // In first iteration we only merge lines with same x or same y and adjacent to each other
    int checkiter = 1;
    int merged_count = 0;
    merge_iteration++;
    
    if(all_lines_to_merge.size() == 0){
        for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
            i2tuple key = iterator->first;
            int x = get<0>(key);
            int y = get<1>(key);
            int theta   = iterator->second;
            all_lines_to_merge.push_back(new myline(x, y, mytan(theta)));
        }
        checkiter = 0;
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
        
        // temporary delete current line
        iter  = all_lines_to_merge.erase(iter);
        
        // if the merge line is found then well and good else push it just behind current iterator
        int index  = get_line_to_merge(current_line);
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
        all_lines_to_merge.insert(iter, line_to_insert);
        ++merged_count;
        
    }
    
    return merged_count;
    
}

//Called when a key is pressed
void handleKeypressa(unsigned char key, int x, int y) {
    switch (key) {
        case 'i':
            mergelines();
            break;
        case 27: //Escape key
            exit(0);
    }
}


int main(int argc, char** argv){
    
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
//    
//        iter  = lp.begin();
//        iter++;
//    
//        lp.erase(iter+3);
//        lp.insert(iter, 100);
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
    
    
    
    
    
    Mat imga = imread("/Users/pranjal/Desktop/img3.png", CV_LOAD_IMAGE_GRAYSCALE);
    Mat bw = imga > 128;
    Mat img = bw > 120;
    
    
    bitwise_not(bw, img);
    thinning(img);
    map_slopes = sweepline(img);
//    for(int i=0;i<10;++i){
//        int mer  = mergelines();
//        if(mer < 5)
//            break;
//    }
//    mergelines();
//    
    
    int p;
    
    for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
        i2tuple key = iterator->first;
        int theta = iterator->second;
        printf("(%d, %d)  => %d\n", get<0>(key), get<1>(key), theta);
    }
    
    
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitWindowSize(sa_width, sa_height);

    
    glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
    //glutInitWindowSize(320, 320);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(displayone); // Register display callback handler for window re-paint
    glutKeyboardFunc(handleKeypressa);
    glutTimerFunc(25, updatea, 0);

    glutMainLoop();           // Enter the event-processing loop
    
    
    
    
    
    
    
    
    
    
    
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
    return 0;
}
#endif
