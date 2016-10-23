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

using namespace cv;
using namespace std;


int godisp = 1;
// for right click menu
void createmymenua(void);
void menu(int value);
void disp(void);

static int winmenu;
static int menuid;
static int val = 0;

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

#include "myline.h"


#define PI 3.14159265



struct sortbyxasc {
    bool operator()(const mypointa* o1, const mypointa* o2) const {
        return o1->x < o2->x;
    }
};
struct sortbyxdes {
    bool operator()(const mypointa* o1, const mypointa* o2) const {
        return o2->x < o1->x;
    }
};
struct sortbyyasc {
    bool operator()(const mypointa* o1, const mypointa* o2) const {
        return o1->y < o2->y;
    }
};
struct sortbyydes {
    bool operator()(const mypointa* o1, const mypointa* o2) const {
        return o2->y < o1->y;
    }
};
struct sortbyangle {
    bool operator()(const mypointa* o1, const mypointa* o2) const{
        return atan2(o1->y-starmean.y, o1->x-starmean.x) < atan2(o2->y-starmean.y, o2->x-starmean.x);
    }
};

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

// function prototypes
void mylinefun(int x1, int y1,int x2,int y2, int flag);

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

// flag = 1 means normal operation
// flag = 2 means only store points for intersection
void mylinefun(int x1, int y1,int x2,int y2, int flag){
    int oy1 = y1;
    
    int swap    = 0;
    int acrossx = 0;
    
    int t1;
    int xt, yt;
    
    if(y2 < y1){				// negative slope
        acrossx = 1;
        y2 = 2*(y1-y2)+y2;
    }
    
    if(flag == 1){
        myq.push(new mypointa(x1, y1));
        myq1.push(new mypointa(x1, y1));
    }
    else
        scanq[y1].push_back(x1);			// directly access by y
    
    if(y2-y1>x2-x1){
        swap = 1;			// swapping the coordinates
        t1 = x1;
        x1 = y1;			// getting image across y = x line
        y1 = t1;
        t1 = x2;
        x2 = y2;
        y2 = t1;
    }
    
    int i, j;
    unsigned char image1[w][h][3];
    int dx  = x2 - x1;
    int dy  = y2 - y1;
    int y   = y1;
    int x   = x1;
    int p = 2*dy-dx;
    
    int dy2 = 2*dy;
    int dy2minusdx2 = 2*(dy-dx);
    
    while(x<x2){
        x++;
        if(p<0)
            p = p + dy2;
        else{
            p = p + dy2minusdx2;
            y = y + 1;
        }
        if(swap){
            xt = y;			// getting the original line by swapping again
            yt = x;			// swapping across  y = x line
        }
        else{
            xt = x;
            yt = y;
        }
        
        if(acrossx){
            yt = yt -2*(yt-oy1);
        }
        if(flag == 1){
            myq.push(new mypointa(xt, yt));
            myq1.push(new mypointa(xt, yt));
        }
        else
            scanq[yt].push_back(xt);			// directly access by y
    }
    if(flag == 1)
        putpixel();
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

//int main(int argc, char** argv){	
//    
//    fp = fopen("/Users/pranjal/Downloads/Vision/assi3/hw3data/test.ppm","r");
//    
//    char buff[16];
//    char *t;
//    
//    fgets(buff, sizeof(buff), fp);
//    
//    if(buff[0] != 'P'||buff[1] != '3') {
//        printf("Not correct image file");
//        exit(0);
//    }	
//    
//    do{ 
//        t = fgets(buff, 256, fp);
//        if ( t == NULL ) 
//            exit(0);
//    }while( strncmp(buff, "#", 1) == 0 );
//    
//    int r = sscanf(buff, "%u %u", &w, &h);
//    if ( r < 2 ) 
//        exit(0);
//    
//    r = fscanf(fp, "%u", &d);
//    if((r < 1)||( d != 255 )) 
//        exit(0);
//    
//    fseek(fp, 1, SEEK_CUR);
//    
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
//    glutInitWindowSize(w, h);
//    winmenu = glutCreateWindow("Draw pixels test");
//    createmymenua(); 
//    
//    glutDisplayFunc(display);
//    glutMouseFunc(mousemotiona);
//    glutKeyboardFunc(keyboard);
//    glutMainLoop();
//    return 0;	
//}




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
    
    for(int i=0; i<180; i = i+2){
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
    for(int i=10; i<im.rows; ++i){
        for(int j=10;j<im.cols;++j){
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


void drawScenea() {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0.0f, 400.0, 400.0, 0.0, 0.0, 1.f);
    
    
            glDepthMask(GL_FALSE);      // disable depth writes
    
            glBegin (GL_QUADS);
            glTexCoord2d(0.0,0.0); glVertex2d(0.0, 0.0);
            glTexCoord2d(1.0,0.0); glVertex2d(400.0, 0.0);
            glTexCoord2d(1.0,1.0); glVertex2d(400.0, 400.0);
            glTexCoord2d(0.0,1.0); glVertex2d(0.0, 400.0);
            glEnd();
    
    int sizea = map_slopes.size();
    
    if(map_slopes.size() > 0){
        glBegin( GL_POINTS );
        glColor3f( 0.95f, 0.207, 0.031f );
        
        for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
            i2tuple key = iterator->first;
            int px = get<0>(key);
            int py = get<1>(key);
            
            glVertex2f(px, py);
        }
    }
    
            // re-enable depth writes
            glDepthMask(GL_TRUE);
    
            // perspective projection for foreground
            glLoadIdentity();
    
//    
//    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f } };
//    int back = 1;
//
//    
//    //Setup for 2D
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0.0, 500, 500, 0.0, -1.0, 10.0);
//    glMatrixMode(GL_MODELVIEW);
//    //glPushMatrix();
//    glLoadIdentity();
//    glDisable(GL_CULL_FACE);
//    
//    glClear(GL_DEPTH_BUFFER_BIT);
//    glClearColor(colors[back][0], colors[back][1], colors[back][2], 1.0f);
    
//    int sizea = map_slopes.size();
//    
//    if(map_slopes.size() > 0){
//        glBegin( GL_POINTS );
//        glColor3f( 0.95f, 0.207, 0.031f );
//        
//        for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
//            i2tuple key = iterator->first;
//            int px = get<0>(key);
//            int py = get<1>(key);
//            
//            glVertex2f(px, py);
//        }
//    }
    
    glutSwapBuffers();
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


int main(int argc, char** argv){
    
    
    
    
    
        //Set handler functions
       //        glutKeyboardFunc(handleKeypress);
//        glutReshapeFunc(handleResize);
//        glutMouseFunc(mousemotion);

    
    // i2tuple p1 = i2tuple(1, 1);
    
    //i2tuple p2 = i2tuple(1, 3);
    //all_lines[p1] = new myline(1, 6, 5);
    //all_lines[p2] = new myline(1, 9, 53);
    
    
    Mat imga = imread("/Users/pranjal/Desktop/img3.png", CV_LOAD_IMAGE_GRAYSCALE);
    Mat bw = imga > 128;
    Mat img = bw > 120;
    
    bitwise_not(bw, img);
    thinning(img);
    
    //get_positive_y(img);
    
     map_slopes = sweepline(img);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    winmenu = glutCreateWindow("Sketch");
    initRendering();
    glutDisplayFunc(drawScenea);
    glutReshapeFunc(handleResize);
    glutTimerFunc(25, updatea, 0);
    //glutMainLoop();
    
    
    printf("rows = %d cols = %d\n", img.rows, img.cols);
    //IplImage* img = cvLoadImage( "/Users/pranjal/Desktop/img2_2.png" , CV_LOAD_IMAGE_GRAYSCALE);
    cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE );
    namedWindow( "sketch", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "sketch", img );                   // Show our image inside it.

    
    
    cvWaitKey(0);
    cvDestroyWindow("Example1");
    return 0;
}
