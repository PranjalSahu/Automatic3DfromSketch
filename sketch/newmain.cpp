/*
 * PRANJAL SAHU
 *
 *
 *
 * Refer
 * 1. http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/
 * 2. https://web.archive.org/web/20160322113207/http://opencv-code.com/quick-tips/implementation-of-thinning-algorithm-in-opencv/
 * 3. http://opencvexamples.blogspot.com/2013/10/harris-corner-detection.html
 * 4. Finding faces in a planar embedding of a graph
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
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

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
#include "polygon.h"
#include "plane.h"


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


// 2d hash for storing lines at each pixel
std::map<i2tuple, myline*> all_lines;
std::map<i2tuple, int> map_slopes;
std::vector<myline*> all_lines_to_merge;
std::vector<myline*> all_lines_created;
vector<i2tuple> points_vector;
vector<i2tuple> corner_points;      // stores the corner points obtained from harris corner
std::vector<myline*> valid_lines_undirected;  // all the valid lines finally obtained
std::vector<myline*> valid_lines_directed;   // all the valid lines finally obtained


#define PI 3.14159265

int minpx = 100000;					// for polygon bounding box
int minpy = 100000;
int maxpx = -1;
int maxpy = -1;


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
std::queue<int> p1;					// for intersection points x coordinate

// different display types for labelled lines, polygons, and projected polygon
int display_type = 2;

// plane to project for demo
plane* plane_to_project;

float angle_p = 0;
float tr_x = 0;
float tr_y = 0;
float tr_z = 0;

// all the polygons present in the given image
std::vector<polygon*> all_polygons;

//bool isleft(mypointa *a, mypointa *b, mypointa *c){
//    return ((b->x - a->x)*(c->y - a->y) - (b->y - a->y)*(c->x - a->x)) > 0;
//}


// glut look at z position
int zglut = 100;


ofstream myfile;

float xview = 0, yview = 0, zview = 0;

// Camera matrix
glm::mat4 View = glm::lookAt(
                             glm::vec3(-5,5,5), // Camera is at (4,3,3), in World Space
                             glm::vec3(0,0,0), // and looks at the origin
                             glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                             );


glm::mat4 ViewI = glm::inverse(View);




//Add positioned light
GLfloat lightColors[][4] = {{1.f, 1.f, 1.f, 1.0f}, {0.5f, 0.2f, 0.2f, 1.0f}};
GLfloat lightPos[][4] = {{-5.0f, 5.0f, 5.0f, 1.0f}, {5.0f, 5.f, 5.f, 0.0f}};   // Positioned at (4, 0, 8)




// points in the camera plane
// get the world space coordinates for these
// then project these coordinates onto hinging plane to get the best plane and the projected polygon will be the final
std::vector<glm::vec4> points_in_camera;

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
//void initRenderinga() {
//    glEnable( GL_POINT_SMOOTH );
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_COLOR_MATERIAL);
//    glEnable(GL_LIGHTING); //Enable lighting
//    glEnable(GL_LIGHT0); //Enable light #0
//    glEnable(GL_LIGHT1); //Enable light #1
//    glEnable(GL_NORMALIZE); //Automatically normalize normals
//    
//    //glShadeModel(GL_SMOOTH); //Enable smooth shading
//}

void updatea(int value) {
    glutPostRedisplay();
    glutTimerFunc(25, updatea, 0);
}

void plot_points(vector<i2tuple> points_to_plot){
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f } };
    
    for(std::vector<i2tuple>::iterator iterator = points_to_plot.begin(); iterator != points_to_plot.end(); iterator++) {
        i2tuple p = *iterator;
        
        glBegin( GL_POINTS );
        glColor3f(0.0f, 1.0f, 0.0f);
        GLfloat px = (get<0>(p)-100)*IMG_SCALE/sa_width;
        GLfloat py = (get<1>(p)-100)*IMG_SCALE/sa_height;
        glVertex2f(px, py);
        glEnd();
    }
    
    return;
}

// corrects the coordinate such that x denotes x in both display and array
// similary y denotes both y in display and array
// helps in understanding
void get_correct_coord(std::vector<myline*> original_lines){
    for(std::vector<myline*>::iterator iterator = original_lines.begin(); iterator != original_lines.end(); iterator++) {
        myline * linet = *iterator;
        int tp = linet->x1;
        linet->x1 = linet->y1;
        linet->y1 = tp;
        tp = linet->x2;
        linet->x2 = linet->y2;
        linet->y2 = tp;
        
        linet->y1 = sa_height-linet->y1;
        linet->y2 = sa_height-linet->y2;
    }
    
    return;
}


// corrects the coordinate such that x denotes x in both display and array
// similary y denotes both y in display and array
// helps in understanding
// also it subtracts the minimum y coordinate from each point
// therefore the bottom point is our origin

std::vector<i2tuple> get_correct_coord_point_and_line(std::vector<i2tuple> original_points, std::vector<myline*> valid_lines_undirected){
    std::vector<i2tuple> temp;
    
    int minx = 100000;
    int miny  = 100000;
    
    for(std::vector<i2tuple>::iterator iterator = original_points.begin(); iterator != original_points.end(); iterator++) {
        i2tuple mp = *iterator;
        int x = get<1>(mp);
        int y = sa_height-get<0>(mp);
        temp.push_back(i2tuple(x, y));
        printf(">> %d, %d\n", x, y);
        if(miny > y){
            miny  = y;
            minx = x;
        }
    }
    
    // shift the points so that the bottom most point is origin
    for(std::vector<i2tuple>::iterator iterator = temp.begin(); iterator != temp.end(); iterator++) {
        i2tuple mp = *iterator;
        *iterator = i2tuple(get<0>(mp)-minx, get<1>(mp)-miny);
    }
    
    get_correct_coord(valid_lines_undirected);
    for(int i=0;i< valid_lines_undirected.size();++i){
        valid_lines_undirected[i]->x1 = valid_lines_undirected[i]->x1-minx;
        valid_lines_undirected[i]->x2 = valid_lines_undirected[i]->x2-minx;
        valid_lines_undirected[i]->y1 = valid_lines_undirected[i]->y1-miny;
        valid_lines_undirected[i]->y2 = valid_lines_undirected[i]->y2-miny;
    }
    
    printf("MINX IS %d and MINY IS %d\n", minx, miny);
    return temp;
}



void plot_line(std::vector<mypoint*> all_points, int color){
    glBegin(GL_LINE_LOOP);
    glLineWidth(5);
    for(std::vector<mypoint*>::iterator iterator = all_points.begin(); iterator != all_points.end(); iterator++) {
        mypoint *m = *iterator;
        GLfloat px = (m->x)*IMG_SCALE/sa_width;
        GLfloat py = (m->y)*IMG_SCALE/sa_height;
        glVertex2f(px, py);
    }
    glEnd();
    
    return;
}

void plot_line(myline *linet, int color){
    glBegin(GL_LINE_LOOP);
    glLineWidth(5);
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f } };

    int r = color;
    glColor3f(colors[r][0], colors[r][1], colors[r][2]);
    
    GLfloat px = (linet->x1)*IMG_SCALE/sa_width;
    GLfloat py = (linet->y1)*IMG_SCALE/sa_height;
    
    GLfloat qx = (linet->x2)*IMG_SCALE/sa_width;
    GLfloat qy = (linet->y2)*IMG_SCALE/sa_height;
    
    glVertex2f(px, py);
    glVertex2f(qx, qy);
    
    glEnd();
}

void plot_lines(std::vector<myline*> lines_to_plot, std::vector<int> color){
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f } };

    int index = 0;
    for(std::vector<myline*>::iterator iterator = lines_to_plot.begin(); iterator != lines_to_plot.end(); iterator++) {
        myline * linet = *iterator;
        
        if (linet->x2 == infvalue || linet->y2 ==infvalue){
            glBegin( GL_POINTS );
            glColor3f(1.0f, 0.0f, 0.0f);
            GLfloat px = (linet->y1-100)*IMG_SCALE/sa_width;
            GLfloat py = (linet->x1-100)*IMG_SCALE/sa_height;
            glVertex2f(px, py);
            glEnd();
            continue;
        }
        
        plot_line(linet, color[index]);
        index = index+1;
    }
    
    return;
}

void plot_lines(std::vector<myline*> lines_to_plot, int color){
    std::vector<int> colors;
    for(int i=0;i<lines_to_plot.size();++i){
        colors.push_back(color);
    }
    plot_lines(lines_to_plot, colors);
    return;
}


void drawmycuboid(GLfloat l, GLfloat b, GLfloat h){
    glBegin(GL_QUADS);
    
    glNormal3f(0.0f, 0.0f, 1.0f);
    
    // THIS CUBOID IS OF DIMENSION 6 x 4 x 2
    // FRONT
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(0.0f, l, -1*b);
    glVertex3f(0.0f, 0.0f, -b);
    
    // SHIFT FRONT BY 2 IN -X TO GET BACK
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(-1*h, 0.0f, -1*b);
    
    // TOP
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(0.0f, l, -1*b);
    
    // SHIFT TOP BY -6 IN Y TO GET BOTTOM
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    glVertex3f(-1*h, 0.0f, -1*b);
    glVertex3f(0.0f, 0.0f, -1*b);
    
    // OPPOSITE HIDDEN
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, l, 0.0f);
    glVertex3f(-1*h, l, 0.0f);
    glVertex3f(-1*h, 0.0f, 0.0f);
    
    // HIDDEN
    glVertex3f(0.0f, 0.0f, -1*b);
    glVertex3f(0.0f, l, -1*b);
    glVertex3f(-1*h, l, -1*b);
    glVertex3f(-1*h, 0.0f, -1*b);
    
    glEnd();
    return;
}



void displayone() {
    
    
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f } };
    
//    glBegin( GL_POINTS );
//    glColor3f(1.0f, 0.0f, 0.0f);
//    for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
//        i2tuple key = iterator->first;
//        GLfloat px = get<0>(key)*1.0/sa_width;
//        GLfloat py = get<1>(key)*1.0/sa_height;
//        glVertex2f(px, py);
//    }
//    glEnd();
    
    //plot_lines(all_lines_to_merge);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    
    // Render a color-cube consisting of 6 quads with different colors
    glLoadIdentity();                 // Reset the model-view matrix
    glTranslatef(1.5f, 0.0f, -7.0f);  // Move right and into the screen

    
    if(display_type == 0){
        std::vector<int> line_colors = get_line_labels(valid_lines_undirected);
        plot_lines(valid_lines_undirected, line_colors);
    }
    else if(display_type == 1){
        int index  = 0;
        for(std::vector<polygon*>::iterator iter = all_polygons.begin(); iter != all_polygons.end(); iter++){
            polygon* pl = *iter;
            plot_lines(pl->lines, index%4);
            ++index;
        }
    }
    else if(display_type == 2){
        polygon* p = all_polygons[3];
        plot_lines(p->lines, 0);
    }
    else if(display_type == 3){
        
        
        // Camera position angle
        gluLookAt(-5+xview, 5+yview, 5+zview, 0, 0, 0, 0, 1, 0);
        //gluLookAt(0, 0, zglut, 0, 0, 0, 0, 1, 0);
        //gluLookAt(-5, 5, 5, 0, 0, 0, 0, 1, 0);
        //gluLookAt(-5, -5, 0, 0, 0, 0, 0, 0, 1);
        //gluLookAt(5, 5, 5, 0, 0, 0, 0, 0, 1);

        
        
        
        //Add ambient light
        GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
        
        // Lighting from different position
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColors[0]);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos[0]);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColors[1]);
        glLightfv(GL_LIGHT1, GL_POSITION, lightPos[1]);

        
        
        
        polygon* p = all_polygons[3];
        std::vector<mypoint *> po = p->get_points();
//        po[0]->y = 0; po[0]->z = 0; po[0]->x = 0;
//        po[1]->y = 0; po[1]->z = 300; po[1]->x = 0;
//        po[2]->y = 300; po[2]->z = 400; po[2]->x = 0;
//        po[3]->y = 300; po[3]->z = 100; po[3]->x = 0;

                po[0]->y = 0; po[0]->z = 0; po[0]->x = 0;
                po[1]->y = 1; po[1]->z = 0; po[1]->x = 5;
                po[2]->y = 2; po[2]->z = 0; po[2]->x = 5;
                po[3]->y = 1; po[3]->z = 0; po[3]->x = 0;

        
        int i = 2;
        std::vector<mypoint *> axis_line;
        //std::vector<mypoint *> pp = temp->project_polygon(po);
//        std::vector<mypoint *> pp = temp->project_polygon(po, -5, -5, 5);
//        printf("1> %f %f %f\n", pp[0]->x, pp[0]->y, pp[0]->z);
//        printf("2> %f %f %f\n", pp[1]->x, pp[1]->y, pp[1]->z);
//        printf("3> %f %f %f\n", pp[2]->x, pp[2]->y, pp[2]->z);
//        printf("4> %f %f %f\n", pp[3]->x, pp[3]->y, pp[3]->z);
        
//        glBegin(GL_QUADS);
//        glColor3f(1,0,0);
//        glVertex3f(0, 0, 0);
//        glVertex3f(10000, 0, 0);
//        glVertex3f(10000, 10, 0);
//        glVertex3f(0, 10, 0);
//        glEnd();
        
        
        // plot the hingin plane's normal line
//        axis_line.push_back(new mypoint(-10000*temp->a, -10000*temp->b, -10000*temp->c));
//        axis_line.push_back(new mypoint(10000*temp->a, 10000*temp->b, 10000*temp->c));
//        plot_line(axis_line, 0);
//        
        
//        glBegin(GL_QUADS);
//        for(int i=0;i<4;++i){
//            glVertex3f( po[i]->x, po[i]->y, po[i]->z);
//            //glVertex3f( pp[i]->x/200.0, pp[i]->y/200.0, pp[i]->z/200.0);
//        }
//        glEnd();
        
//        
//        
//        
//        
//        
//        // Camera matrix
//        glm::mat4 View = glm::lookAt(
//                                     glm::vec3(-5,-5,5), // Camera is at (4,3,3), in World Space
//                                     glm::vec3(0,0,0), // and looks at the origin
//                                     glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
//                                     );
//        
//        glm::mat4 Model = glm::mat4(1.0f);
//        std::vector<glm::vec4> tp;
//        tp.push_back(glm::vec4(0,0,0, 1));
//        tp.push_back(glm::vec4(5,0,0, 1));
//        tp.push_back(glm::vec4(5,1,0, 1));
//        tp.push_back(glm::vec4(0,1,0, 1));
        
//        glBegin(GL_QUADS);
//        
//        //    glm::vec4 result = v * m;
//        for(std::vector<glm::vec4>::iterator iterator = tp.begin(); iterator != tp.end(); ++iterator) {
//            glm::vec4 v =  *iterator;
//            glVertex3f( v[0], v[1], v[2]);
//            //std::cout<<glm::to_string(View*v)<<std::endl;
//        }
//        glEnd();
        
        std::vector<mypoint*> points_to_render;

        plane *temp = plane_to_project->rotate_it(-1*angle_p, 0, 1, 0);
        printf("hinging angle is %f (%f, %f, %f)\n", angle_p, temp->a, temp->b, temp->c);
        points_to_render = temp->project_polygon(points_in_camera, -5, 5, 5);
        
        glColor3f(0.9f, 0.9f, 0.9f);
        glNormal3f(temp->a, temp->b, temp->c);
        
        glBegin(GL_QUADS);
        for(int i=0;i<4;++i){
            printf("%d >> (%f, %f, %f)\n", i, points_to_render[i]->x, points_to_render[i]->y, points_to_render[i]->z);
            glVertex3f( points_to_render[i]->x, points_to_render[i]->y, points_to_render[i]->z);
            //glVertex3f( pp[i]->x/200.0, pp[i]->y/200.0, pp[i]->z/200.0);
        }
        glEnd();

        
        
        glPushMatrix();
        glTranslatef(tr_x, tr_y, tr_z);
        drawmycuboid(1, 2, 5);
        glPopMatrix();
//
//        int i = 0;
//        plane *temp = plane_to_project->rotate_it(-1*angle_p, po[i+1]->x-po[i]->x, po[i+1]->y-po[0]->y, 0);
//        std::vector<mypoint *> pp = temp->project_polygon(po);
//        printf("%f %f %f\n", pp[0]->x, pp[0]->y, pp[0]->z);
//        plot_line(pp, 0);
    }
    
    //plot_points(corner_points);
    glutSwapBuffers();
    //glFlush();  // Render now
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


class sorter_check {
    int test;
public:
    sorter_check(int type) : test(type) {}
    bool operator()(const int o1, const int o2) const {
        return abs(o1-test) < abs(o2-test);
    }
};


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
        //case 'a':
        //    mergelinesa(img);
        //    break;
        case 'm':
            merge_points();
            break;
        case 'p':
            display_type = 1;       // show polygons
            break;
        case 'l':
            display_type = 0;       // show lines
            break;
        case 'o':
            display_type = 2;       // show projected polygon
            break;
        case 'r':                   // display the project polygon
            display_type = 3;
            angle_p  = 1-angle_p;       // on each key press increases the angle by 1
            break;
        case 'q':
            angle_p = angle_p+0.5;
            break;
        case 'a':
            angle_p = angle_p-0.5;
            break;
        case 'x':
            tr_x = tr_x+0.5;
            break;
        case 'y':
            tr_y = tr_y+0.5;
            break;
        case 'z':
            tr_z = tr_z+0.5;
            break;
        case 'j':
            zglut = zglut-1;
            printf("zglut is %f\n", zglut);
            break;
        case '1':
            xview = xview+0.5;
            break;
        case '2':
            yview = yview +0.5;
            break;
        case '3':
            zview = zview+0.5;
            break;
        case '4':
            xview = xview-0.5;
            break;
        case '5':
            yview = yview-0.5;
            break;
        case '6':
            zview = zview-0.5;
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
    
    cornerHarris( imga, dst, 9, 9, 0.05, BORDER_DEFAULT );
    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( dst_norm, dst_norm_scaled );
    
    
    int minv = 1000000;
    int maxv = -1;
    // store it in corner points
    for( int j = 0; j < dst_norm.rows ; j++ ){
        for( int i = 0; i < dst_norm.cols; i++ ){
            if( (int) dst_norm.at<float>(j,i) > HARRIS_THRESH ){
                if(minv > dst_norm.at<float>(j,i)){
                    minv = dst_norm.at<float>(j,i);
                }
                if(maxv < dst_norm.at<float>(j,i)){
                    maxv =dst_norm.at<float>(j,i);
                }
                corner_points.push_back(i2tuple(j, i));
            }
        }
    }
    
    printf("MINIMUM values is %d\n", minv);
    printf("MAXIMUM values is %d\n", maxv);
    return;
}

// returns the dual edges for all lines
std::vector<myline*> get_reverse_lines(std::vector<myline*> vl){
    std::vector<myline*> reverse_lines;
    for(std::vector<myline*>::iterator iterator = vl.begin(); iterator != vl.end(); ++iterator) {
        myline *t = *iterator;
        reverse_lines.push_back(new myline(t->x2, t->x1, t->y2, t->y1));
    }
    return reverse_lines;
}


// this function will be changed later to incorporate sweeping line
// at present we are only using the (number of points intersected)/(distane between this two points)
// can be changed later
std::vector<myline*> get_all_valid_lines(){
    vector<myline*> all_line_pairs;
    
    // generate all line pairs
    for(std::vector<i2tuple>::iterator it = corner_points.begin(); it != corner_points.end(); ++it){
        for(std::vector<i2tuple>::iterator ita = it+1; ita != corner_points.end(); ++ita){
            int x1 = get<0>(*it);
            int y1 = get<1>(*it);
            int x2 = get<0>(*ita);
            int y2 = get<1>(*ita);
            if(!(x1 == x2 && y1 == y2)){
                all_line_pairs.push_back(new myline(x1, x2, y1, y2));
            }
        }
    }
    
    printf("SIZE OF ALL LINES BEFORE %d\n", all_line_pairs.size());
    int valid_count = 0;
    for(std::vector<myline*>::iterator iterator = all_line_pairs.begin(); iterator != all_line_pairs.end();) {
        myline *ml = *iterator;
        std::vector<i2tuple> pp = ml->pointliecount(points_vector);
        float ratio = (pp.size()*1.0)/ml->get_line_length();
        printf("%d %d %d %d %f\n", ml->x1, ml->y1, ml->x2, ml->y2, ratio);
        if( ratio < POINT_PAIR_LYING_THRESH){
            iterator = all_line_pairs.erase(iterator);     // erase a line if the number of points lying on that line
            //printf("size after removal is %d\n", all_line_pairs.size());
                                                // is smaller than the threshold * distance between those two point
        }
        else{
            printf("lying count %d length is %d ratio is %f\n", pp.size(), ml->get_line_length(), ratio);
            valid_count = valid_count +1;
            iterator++;
        }
    }
    
    printf("SIZE OF ALL LINES AFTER %d valid_count is %d\n", all_line_pairs.size(), valid_count);
    return all_line_pairs;
}


void plotpoint(i2tuple pt){
    int i = get<0>(pt);
    int j = get<1>(pt);
    circle( dst_norm_scaled, Point( j, i ), 5,  Scalar(0, 255, 0), 2, 8, 0 );
    return;
}

int mainabs(int argc, char **argv){
    plane* p = new plane(0, 0, 1, new mypoint());
    
    printf("%f %f %f\n", p->a, p->b, p->c);
    p->rotate_it(90, 0, 1, 0);
    printf("%f %f %f\n", p->a, p->b, p->c);
    
    printf("cos is %f\n", cos(90 * PI/180.0));
    return 0;
}


void initGL() {
    int back = 2;
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 0.0f, 0.0f } };
    glClearColor(colors[back][0], colors[back][1], colors[back][2], 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
    glEnable( GL_POINT_SMOOTH );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); //Enable lighting
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Automatically normalize normal
}



void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    
    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}


int mainabh(int argc, char** argv){
    int width = 1;
    int height = 1;
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
    
    
    
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 mvp = Projection * View * Model;
    
    std::vector<glm::vec4> tp;
    std::vector<glm::vec4> orig;
    
    tp.push_back(glm::vec4(0,0,0, 1));
    tp.push_back(glm::vec4(5,0,0, 1));
    tp.push_back(glm::vec4(5,1,0, 1));
    tp.push_back(glm::vec4(0,1,0, 1));
    
    
//    glm::vec4 result = v * m;
    for(std::vector<glm::vec4>::iterator iterator = tp.begin(); iterator != tp.end(); ++iterator) {
        glm::vec4 v =  *iterator;
        std::cout<<glm::to_string(ViewI*(View*v))<<std::endl;
        //orig.push_back(<#const_reference __x#>)
    }
    
    
    
//    std::cout<<glm::to_string(v)<<std::endl;
//    std::cout<<glm::to_string(result)<<std::endl;
    
    return 0;
}






void write_to_file(std::vector<i2tuple> corner_points, std::vector<myline*> valid_lines_undirected){
    // writing to file for future use
    myfile << corner_points.size();
    myfile << "\n";
    for(std::vector<i2tuple>::iterator it = corner_points.begin(); it != corner_points.end(); ++it){
        i2tuple pt = *it;
        int i = std::get<0>(pt);
        int j = std::get<1>(pt);
        myfile << i;  myfile << ",";  myfile << j; myfile << "\n";
    }
    myfile << valid_lines_undirected.size();
    myfile << "\n";
    for(std::vector<myline*>::iterator it = valid_lines_undirected.begin(); it != valid_lines_undirected.end(); ++it){
        myline* pt = *it;
        int x1 = pt->x1;
        int x2 = pt->x2;
        int y1 = pt->y1;
        int y2 = pt->y2;
        myfile << x1; myfile << ","; myfile << y1; myfile << ","; myfile << x2; myfile << ","; myfile << y2; myfile << "\n";
    }
    
    return;
}

void plot_corner_points_and_lines(Mat dst_norm_scaled, std::vector<myline*> valid_lines_undirected, vector<i2tuple> corner_points){
    
    for(std::vector<i2tuple>::iterator it = corner_points.begin(); it != corner_points.end(); ++it){
        i2tuple pt = *it;
        int i = get<0>(pt);
        int j = get<1>(pt);
        circle( dst_norm_scaled, Point( j, i ), 15,  Scalar(0, 255, 0), 2, 8, 0 );
        printf("(%d, %d)\n", i, j);
    }
    
    for(std::vector<myline*>::iterator it = valid_lines_undirected.begin(); it != valid_lines_undirected.end(); ++it){
        myline* pt = *it;
        int x1 = pt->x1;
        int x2 = pt->x2;
        int y1 = pt->y1;
        int y2 = pt->y2;
        line(dst_norm_scaled, Point(y1, x1), Point(y2, x2), Scalar(0, 255, 0), 1, 8, 0);
        printf("(%d, %d) -> (%d, %d)\n", x1, y1, x2, y2);
    }
    
    return;
}



int main(int argc, char** argv){
    
    myfile.open ("/Users/pranjal/Downloads/Graphics/huffman5.txt");
    
    
    
    imga = imread("/Users/pranjal/Desktop/huffman5.png", CV_LOAD_IMAGE_GRAYSCALE);
    //GaussianBlur( imga, imga, Size(3,3), 0, 0, BORDER_DEFAULT );
    
    
//    std::vector<myline*> ap;
//    myline *l = new myline(1, 1);
//    ap.push_back(l);
//    std::vector<myline*> apa;
//    
//    apa.push_back(ap[0]);
    
//    Mat grad_x, grad_y, grad;
//    Mat abs_grad_x, abs_grad_y;
//
//    int ddepth = CV_16S;
//    int delta = 0;
//    int scale = 1;
//
//    Sobel( imga, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
//    convertScaleAbs( grad_x, abs_grad_x );
//    
//    /// Gradient Y
//    //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
//    Sobel( imga, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
//    convertScaleAbs( grad_y, abs_grad_y );
//    
//    /// Total Gradient (approximate)
//    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
//    
//    Mat src, erosion_dst, dilation_dst;
//    
//    int erosion_elem = 0;
//    int erosion_size = 0;
//    int dilation_elem = 0;
//    int dilation_size = 1;
//    int const max_elem = 2;
//    int const max_kernel_size = 21;
//    
//    
    bw   = imga > 128;
    img  = bw > 120;
//
//    
//    int dilation_type;
//    if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
//    else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
//    else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
//    
//    Mat element = getStructuringElement( dilation_type,
//                                        Size( 2*dilation_size + 1, 2*dilation_size+1 ),
//                                        Point( dilation_size, dilation_size ) );
//    /// Apply the dilation operation
//    dilate( bw, dilation_dst, element );
//    imshow( "Dilation Demo", dilation_dst );
//    
//    
//
    
    // get corner points using harris detector
    get_corner_points(imga);
    
    while(merge_points() != 0){
        merge_points();
    }
    
    bitwise_not(bw, img);
    thinning(img);
    //namedWindow( "corners_window_a", CV_WINDOW_AUTOSIZE );
    
    
    
    //bw = grad > 128;
    //imshow( "corners_window_a", bw );
    //map_slopes = sweepline(img);

//    for(std::map<i2tuple, int>::iterator iterator = map_slopes.begin(); iterator != map_slopes.end(); iterator++) {
//        i2tuple key = iterator->first;
//        int theta = iterator->second;
//        printf("(%d, %d)  => %d\n", get<0>(key), get<1>(key), theta);
//    }
//    
//    
    
    
    
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
    
    
    
    
//    Mat dst, dst_norm;
//    dst = Mat::zeros( imga.size(), CV_32FC1 );
//    
//    cornerHarris( imga, dst, 5, 5, 0.03, BORDER_DEFAULT );
//    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
//    convertScaleAbs( dst_norm, dst_norm_scaled );
    
    

    fill_points_vector(img);
    
    
    // get all the valid lines by checking the ratio of points lying on the line and its length
    valid_lines_undirected = get_all_valid_lines();
    
    
    // showing the result of line detection
    plot_corner_points_and_lines(dst_norm_scaled, valid_lines_undirected, corner_points);
    namedWindow( "corners_window", CV_WINDOW_AUTOSIZE );
    imshow( "corners_window", dst_norm_scaled );
    waitKey(0);

    
    corner_points = get_correct_coord_point_and_line(corner_points, valid_lines_undirected);
    
    
    // write corner points and line segments to file for later use
    write_to_file(corner_points, valid_lines_undirected);
    myfile.close();
    
    
    
    std::vector<myline*> rv = get_reverse_lines(valid_lines_undirected);
    
    // add original eges and then
    // add the reverse edges for all edges
    valid_lines_directed.insert(valid_lines_directed.end(), valid_lines_undirected.begin(), valid_lines_undirected.end());
    valid_lines_directed.insert(valid_lines_directed.end(), rv.begin(), rv.end());
    
    
//    for(int i=0;i< corner_points.size();++i){
//        printf(">>>>> %d %d\n", get<0>(corner_points[i]), get<1>(corner_points[i]));
//    }
//    // get all the polygons by taking each line as starting point
    all_polygons = get_all_polygons(valid_lines_directed);
    
    
    get_huffman_label(valid_lines_directed, valid_lines_undirected, corner_points);
    
    
    
    // we start with x = 0 then rotate this plane till we get the best plane to project
    plane_to_project = new plane(1, 0, 0, new mypoint(0, 0, 0));
    //plane_to_project = new plane(-5, 5, 5, new mypoint(-5, 5, 5));
    
    // points in the world camera
    points_in_camera.push_back(glm::vec4(0, 0, 0, 1));
    points_in_camera.push_back(glm::vec4(5, 0, 0, 1));
    points_in_camera.push_back(glm::vec4(5, 2, 0, 1));
    points_in_camera.push_back(glm::vec4(0, 2, 0, 1));
    
    
    // get the points in the camera plane and make its depth equal to 0 since we have lost that information
    // in that image
    for(int i=0;i<4;++i){
        points_in_camera[i] = View*points_in_camera[i];
        points_in_camera[i][2] = 0;
    }
    
    // get world coordinates for these points
    for(int i=0;i<4;++i){
        points_in_camera[i] = ViewI*points_in_camera[i];
    }
    

    
    
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE);
    
    glutInitWindowSize(sa_width, sa_height);
    
    //initRenderinga();
    
    glutCreateWindow("OpenGL Setup Test");  // Create a window with the given title
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable( GL_BLEND );
    
    //glutInitWindowPosition(50, 50);         // Position the window's initial top-left corner
    glutDisplayFunc(displayone);            // Register display callback handler for window re-paint
    glutKeyboardFunc(handleKeypressa);
    glutTimerFunc(25, updatea, 0);
    glutReshapeFunc(reshape);
    initGL();                       // Our own OpenGL initialization
    glutMainLoop();
    
    
    
    
    return 0;
}
#endif
