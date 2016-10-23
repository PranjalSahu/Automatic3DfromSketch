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

#include <math.h>
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


#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

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

void display3(){
    int nw = maxpx-minpx;
    int nh = maxpy-minpy;
    int i, j, k;
    int mymask[nw+1][nh+1];
    
    for(i=0;i<=nw;++i)
        for(j=0;j<=nh;++j)
            mymask[i][j] = 0;
    
    int size = (int)maskpoly.size();
    int ycount=0;
    for(i=0;i<size;++i){
        mymask[maskpoly.front()->x-minpx][maskpoly.front()->y-minpy] = 1;		//set the flag
        maskpoly.pop_front();
        ++ycount;
    }
    
    unsigned char tempimage[nw+1][nh+1][3];
    memcpy(tempimage, image2, (nw+1)*(nh+1)*3);
    int count =0 ;
    for(i=-5+nw/2;i<=5+nw/2;++i){
        for(j=0;j<=nh;++j){
            //if(mymask[i][j] != 1){
            tempimage[i][j][0] 	= (unsigned char)0;
            tempimage[i][j][1] 	= (unsigned char)255;
            tempimage[i][j][2]  = (unsigned char)0;
            ++count;
            //}
        }
    }
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    printf("ycount = %d count = %d and rest = %d\n", ycount, count, (nw+1)*(nh+1));
    glDrawPixels(nw, nh, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)tempimage);
    glutSwapBuffers();
}

void mytempfun(){
    int nw = maxpx-minpx;
    int nh = maxpy-minpy;
    
    if(win2 != -1)
        glutDestroyWindow(win2);
    
    image2 = (unsigned char *)malloc(sizeof(unsigned char)*nh*nw*3);
    glReadPixels(minpx, w-(h-maxpy)-nh, nw, nh, GL_RGB, GL_UNSIGNED_BYTE, image2);
    
    glutInitWindowSize(nw+1, nh+1);
    glutCreateWindow("Polygon 1");
    glDrawPixels(nw, nh, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)image2);
    glutSwapBuffers();
}

void showcopymasked(){
    if(win2 != -1)
        glutDestroyWindow(win2);
    //glClearColor(0, 0, 0, 0);
    int nw = maxpx-minpx;
    int nh = maxpy-minpy;
    
    unsigned char tempimage[w][h][3];
    unsigned char tempimage1[w][h][3];
    memset(tempimage1, 155, w*h*3);
    //glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, image2);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, tempimage);
    //glutInitWindowSize(nw+1, nh+1);
    glutInitWindowSize(w+1, h+1);
    win2 = glutCreateWindow("Cropped Polygon");
    int size = (int)maskpoly.size();
    
    while(!maskpoly.empty()){
        //mymask[maskpoly.front()->x-minpx][maskpoly.front()->y-minpy] = 1;		//set the flag
        tempimage1[maskpoly.front()->y][maskpoly.front()->x][0] = original[maskpoly.front()->y][maskpoly.front()->x][0];
        tempimage1[maskpoly.front()->y][maskpoly.front()->x][1] = original[maskpoly.front()->y][maskpoly.front()->x][1];
        tempimage1[maskpoly.front()->y][maskpoly.front()->x][2] = original[maskpoly.front()->y][maskpoly.front()->x][2];
        maskpoly.pop_front();
        //++ycount;
    }
    
    /*int ycount=0;
     for(i=0;i<size;++i){
     mymask[maskpoly.front()->x-minpx][maskpoly.front()->y-minpy] = 1;		//set the flag
     maskpoly.pop_front();
     ++ycount;
     }
     glutDisplayFunc(display3);*/
    glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)tempimage1);
    glutSwapBuffers();
    //mytempfun();
}

void copymasked(){
    int i, j, k;
    unsigned char tempimage[w][h][3];
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, tempimage);
    int size = (int)myq1.size();
    int count = w*h;
    while(!maskpoly.empty()){
        tempimage[maskpoly.front()->y][maskpoly.front()->x][0] = (unsigned char)255;
        tempimage[maskpoly.front()->y][maskpoly.front()->x][1] = (unsigned char)255;
        tempimage[maskpoly.front()->y][maskpoly.front()->x][2] = (unsigned char)0;
        maskpoly.pop_front();
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
    printf("MASKED COPY DONE count = %d\n", count);
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

void croppolygon1(){
    getminmaxforboundingbox();
    int bw = maxpx-minpx+1;				// box width
    int bh = maxpy-minpy+1;				// box height
    int xcoor;
    myclear();							// clear the intersection points
    for(int y=minpy;y<=maxpy;++y){
        for(int i=0;i<polysize;++i){
            if(points[i].y-y == 0){
                scanq[y].push_back(points[i].x);
                if(findinvertexset(points[i].x, y))
                    scanq[y].push_back(points[i].x);
            }
            if((points[i].y-y)*(points[i+1].y-y) < 0){		// in between the two segments
                xcoor = (int)(points[i].x+((points[i+1].x-points[i].x+0.0)*(y-points[i].y+0.0)/(points[i+1].y-points[i].y+0.0)));
                scanq[y].push_back(xcoor);
                if(findinvertexset(xcoor, y))
                    scanq[y].push_back(xcoor);
            }
        }
        if(scanq[y].size()%2 == 0 && scanq[y].size()!=0){
            scanq[y].sort();
            while(!scanq[y].empty()){
                int temp1, temp2;
                temp1 = scanq[y].front();
                scanq[y].pop_front();
                temp2 = scanq[y].front();
                scanq[y].pop_front();
                for(int p=temp1;p<temp2;++p){
                    maskpoly.push_back(new mypointa(p, y));
                }
            }
        }
    }
    
    showcopymasked();
    return;
}

void croppolygon(){
    int rcount =0;
    getminmaxforboundingbox();
    int bw = maxpx-minpx+1;				// box width
    int bh = maxpy-minpy+1;				// box height
    
    mypointa temp1, temp2;
    for(int i=0;i<polysize;++i){		// get the points lying on the line
        if(points[i].x < points[i+1].x)
            mylinefun(points[i].x, points[i].y, points[i+1].x, points[i+1].y, 2);
        else
            mylinefun(points[i+1].x, points[i+1].y, points[i].x, points[i].y, 2);
    }
    
    printf("minpx = %d  minpy = %d  maxpx = %d  maxpy = %d\n", minpx, minpy, maxpx, maxpy);
    for(int i=minpy;i<=maxpy;++i){		// for each scan line insert the line
        int size, tcount=0;
        size = (int)scanq[i].size();
        if(size == 0)					// check if size is zero
            continue;
        
        scanq[i].sort();				// sort by x coordinate
        
        temp1.x = scanq[i].front();		// get first point
        temp1.y = i;
        scanq[i].push_back(temp1.x);	// push the first point
        
        while(tcount<size){
            temp2.x = scanq[i].front();
            temp2.y = i;
            
            maskpoly.push_back(new mypointa(temp2.x, temp2.y));
            scanq[i].pop_front();						// either it is going to be popped or it will be pushed
            ++tcount;									// behind at each step one point is processed
            scanq[i].push_back(temp2.x);
            if(temp2.x-temp1.x != 1 && temp2.x-temp1.x != 0){					// pixels are continuos
                scanq[i].push_back(temp2.x);
            }
            if(findinvertexset(temp2.x, temp2.y)){
                //scanq[i].pop_back();
                scanq[i].push_back(temp2.x);
            }
            temp1 = temp2;								// update the previous pixel
            
        }
        
        int newsize = (int)scanq[i].size();
        if(scanq[i].size()%2 == 0 && scanq[i].size()!=0){
            while(!scanq[i].empty()){
                temp1.x = scanq[i].front();
                scanq[i].pop_front();
                temp2.x = scanq[i].front();
                scanq[i].pop_front();
                for(int p=temp1.x;p<temp2.x;++p){
                    maskpoly.push_back(new mypointa(p, i));
                    ++rcount;
                }
            }
        }
        else if(scanq[i].size() != 1){
            
            printf("#ODD i = %d count = %d ", i, (int)scanq[i].size());
            int stemp = (int)scanq[i].size();
            while(stemp>0){
                printf("%d ", scanq[i].front());
                if(findinvertexset(scanq[i].front(), i)){
                    int y = scanq[i].front();
                    scanq[i].push_back(y);			// count it twice
                    scanq[i].push_back(y);
                    printf("V ");
                }
                else{
                    scanq[i].push_back(scanq[i].front());
                }
                scanq[i].pop_front();
                --stemp;
            }
            printf("\nNEw size after odd = %d\n", (int)scanq[i].size());
            while(!scanq[i].empty()){
                printf("%d ", scanq[i].front());
                scanq[i].pop_front();
            }
            printf("\n");
            if(scanq[i].size()%2 != 0){
                while(!scanq[i].empty()){
                    temp1.x = scanq[i].front();
                    scanq[i].pop_front();
                    temp2.x = scanq[i].front();
                    scanq[i].pop_front();
                    for(int p=temp1.x;p<temp2.x;++p){
                        maskpoly.push_back(new mypointa(p, i));
                        ++rcount;
                    }
                }
            }
        }
    }
    showcopymasked();
    //maskpoly.clear();		// clear the list of points
    return;
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

void write(int x, int y){
    //x = px[1];y = py[1];
    float x1 = (2/(w+0.0))*x;
    y = h-y;
    float y1 = (2/(h+0.0))*y;
    printf("writing text to screen\n");
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2f(-1+x1, -1+y1);
    int len = strlen(etext);
    for(int i = 0; i < len; i++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, etext[i]);
    }
    entertext = 0;
    glutSwapBuffers();
    glRasterPos2f(-1, -1);
    count = 0;
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

void drawconvexpolygon(){
    int i;
    int size = poly1.size();
    mypointa *temp1 = new mypointa(0, 0);				// top two elements of stack
    mypointa *temp2 = new mypointa(0, 0);
    
    for(i=0;i<size;++i){
        temp1 = poly1.front();
        temp1->y = h-temp1->y;
        poly1.pop_front();
        poly1.push_back(new mypointa(temp1));
    }
    
    poly1.sort(sortbyyasc());							// 2
    // 1
    starmean.x     = poly1.front()->x;					//	starmean = miny
    starmean.y     = poly1.front()->y;
    poly1.pop_front();									// remove that point
    poly1.sort(sortbyangle());							// sort the remaining point
    
    
    convexpoly.push_front(new mypointa(starmean));		// again put the leftmost point
    convexpoly.push_front(new mypointa(poly1.front()));	// push in the convex hull set
    poly1.pop_front();
    
    temp1->x = starmean.x;								// top two points of the stack
    temp1->y = starmean.y;								// top two points of the stack
    copyit(temp2,convexpoly.front());
    
    int csize;
    while(poly1.size() != 0){
        csize = (int)convexpoly.size();
        //printf("csize  = %d\n", csize);
        if(csize<2 || isleft(temp1, temp2, poly1.front())){
            copyit(temp1, convexpoly.front());
            convexpoly.push_front(new mypointa(poly1.front()));
            copyit(temp2,convexpoly.front());
            poly1.pop_front();
        }
        else{
            convexpoly.pop_front();
            copyit(temp2, convexpoly.front());
            if(convexpoly.size()>1){
                convexpoly.pop_front();
                copyit(temp1, convexpoly.front());
                convexpoly.push_front(new mypointa(temp2));
            }
        }
    }
    
    int tx1, ty1;
    printf("------------------------\n\n");
    size = (int)convexpoly.size();
    polysize = size;								// for cropping
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<size;++i){
        tx1  = convexpoly.front()->x;
        ty1  = convexpoly.front()->y;
        points[size-i-1].x = tx1;				// store them for cropping
        points[size-i-1].y = ty1;
        glVertex2f(-1+2*tx1/(w+0.0), -1+2*ty1/(h+0.0));
        convexpoly.pop_front();
    }
    glEnd();
    glutSwapBuffers();
    points[polysize].x = points[0].x;				// make it circular
    points[polysize].y = points[0].y;
    croppolygon1();
    convexpoly.clear();
    myclear();
}

void drawstarpolygon(){
    poly1.sort(sortbyangle());
    int tx1, ty1;
    printf("------------------------\n\n");
    int size = (int)poly1.size();
    polysize = size;
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<size;++i){
        tx1  = poly1.front()->x;
        ty1  = poly1.front()->y;
        points[size-i-1].x = tx1;				// store them for cropping
        points[size-i-1].y = h-ty1;
        glVertex2f(-1+2*tx1/(w+0.0), -1+2*(h-ty1)/(h+0.0));
        poly1.pop_front();
    }
    glEnd();
    glutSwapBuffers();
    points[polysize].x = points[0].x;
    points[polysize].y = points[0].y;
    croppolygon1();
    myclear();
}

void drawypolygon(){
    printf("Draw the polygon\n");
    poly1.sort(sortbyyasc());			// sorting by y
    mypointa *min = new mypointa(poly1.front()->x, poly1.front()->y);		// getting the top and bottom most point
    mypointa *max = new mypointa(poly1.back()->x, poly1.back()->y);
    printf("max (%d, %d) min (%d, %d)\n", max->x, max->y, min->x, min->y);
    int size = poly1.size();
    
    std::list<mypointa*> left;			// for storing the points of polygon on left side
    std::list<mypointa*> right;			// for storing the points of polygon on right side
    std::list<mypointa*> finalpoly;
    
    int i;
    left.push_back(new mypointa(poly1.front()->x, poly1.front()->y));				// put max and min in left one
    poly1.pop_front();		// remove first point
    
    
    for(i=0;i<size-2;++i){
        if(isleft(min, max, poly1.front()))				// which side of line
            left.push_back(new mypointa(poly1.front()->x, poly1.front()->y));
        else
            right.push_back(new mypointa(poly1.front()->x, poly1.front()->y));
        poly1.pop_front();
    }
    left.push_back(new mypointa(poly1.front()->x, poly1.front()->y));				// put max and min in left one
    poly1.pop_front();
    
    left.sort(sortbyyasc());		// sorting from top to bottom
    right.sort(sortbyydes());		// sorting from bottom to top
    
    printf("------------------------\n\n");
    size = (int)left.size();
    for(i=0;i<size;++i){
        //printf("(%d %d)\n", left.front()->x, left.front()->y);
        finalpoly.push_back(new mypointa(left.front()));
        drawpoint(left.front()->x, left.front()->y, 1);
        left.pop_front();
    }
    size = (int)right.size();
    for(i=0;i<size;++i){
        //printf("(%d %d)\n", right.front()->x, right.front()->y);
        finalpoly.push_back(new mypointa(right.front()));
        drawpoint(right.front()->x, right.front()->y, 2);
        right.pop_front();
    }
    
    int tx1, ty1;
    printf("------------------------\n\n");
    size = (int)finalpoly.size();
    polysize = size;
    glBegin(GL_LINE_LOOP);
    for(i=0;i<size;++i){
        tx1  = finalpoly.front()->x;
        ty1  = finalpoly.front()->y;
        points[size-i-1].x = tx1;				// store them for cropping
        points[size-i-1].y = h-ty1;
        glVertex2f(-1+2*tx1/(w+0.0), -1+2*(h-ty1)/(h+0.0));
        finalpoly.pop_front();
    }
    glEnd();
    glutSwapBuffers();
    points[polysize].x = points[0].x;
    points[polysize].y = points[0].y;
    croppolygon1();
    myclear();
    return;
}

void mousemotiona(int button, int state, int x, int y){
    printf("%d %d %d %d %d %d\n", button, state, x, y, x, h-y);
    if(donepoly){
        donepoly = 0;
        drawpoly = 0;
        if(polytype == 1)
            drawypolygon();
        else if(polytype == 2){
            starmean.x = starmean.x/poly1.size();
            starmean.y = starmean.y/poly1.size();
            drawstarpolygon();
        }
        else{
            drawconvexpolygon();
        }
        return;
    }
    if(state == GLUT_DOWN){
        px[count] = x;
        py[count] = y;
        ++count;
    }
    else{
        if(drawpoly && button == 0){
            if(drawpoly == 1){					// taking points of polygon
                drawpoly = 2;					// ignore the point
                starmean.x=0;
                starmean.y=0;
                return;
            }
            printf("taking point\n");
            if(drawpoly == 2){
                drawpoint(x, y, 1);
                poly1.push_back(new mypointa(x, y));
                starmean.x = starmean.x+x;
                starmean.y = starmean.y+y;
            }
            return;
        }
        if(entertext){
            count = 0;
            printf("Enter your text\n");
            gets(etext);
            write(x, y);
            return;
        }
        if(count>=2){
            count = 0;
            if(drawline){
                int tx[2], ty[2];
                if(px[0]<px[1]){
                    tx[0] = px[0];	ty[0] = py[0];
                    tx[1] = px[1];	ty[1] = py[1];
                }
                else{
                    tx[0] = px[1];	ty[0] = py[1];
                    tx[1] = px[0];	ty[1] = py[0];
                }
                mylinefun(tx[0], h-ty[0], tx[1], h-ty[1], 1);		// change it to support other slopes
            }
            else if(cropimage){
                drawrectangle();
            }
        }
    }
}

void display(void){
    if(!godisp)
        return;
    int k;
    glClear (GL_COLOR_BUFFER_BIT);
    
    printf("width = %d and height = %d\n", w, h);
    
    unsigned char image1[w][h][3];
    unsigned int a;
    int i, j;
    
    for(i=0;i<w;++i){
        for(j=0;j<h;++j){
            fscanf(fp, "%u", &a);
            image1[i][j][0] = (unsigned char)a;
            myimage[i][j][0] = image1[i][j][0];
            fscanf(fp, "%u", &a);
            image1[i][j][1] = (unsigned char)a;
            myimage[i][j][1] = image1[i][j][1];
            fscanf(fp, "%u", &a);
            image1[i][j][2] = (unsigned char)a;
            myimage[i][j][2] = image1[i][j][2];
        }
    }
    
    unsigned char temp[3];
    
    for(i=0;i<h/2;++i){			// displaying upright image
        for(j=0;j<w;++j){
            for(k=0;k<3;++k){
                temp[k]	 		 	  = image1[i][j][k];
                image1[i][j][k]  	  = image1[h-i-1][j][k];
                image1[h-i-1][j][k]   = temp[k];
                original[h-i-1][j][k] = image1[h-i-1][j][k];
                original[i][j][k]     = image1[i][j][k];
            }
        }
    }
    
    for(i=0;i<w;++i){			// copying again upright image
        for(j=0;j<h;++j){
            myimage[i][j][0] = image1[i][j][0];
            myimage[i][j][1] = image1[i][j][1];
            myimage[i][j][2] = image1[i][j][2];
        }
    }
    
    // copying again the image with line
    /*for(i=0;i<w;++i){
     for(j=0;j<h;++j){
     image1[i][j][0] = myimage[i][j][0];
     image1[i][j][1] = myimage[i][j][1];
     image1[i][j][2] = myimage[i][j][2];
     }
     }
     */
    glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)image1);
    glutSwapBuffers();
    godisp = 0;
}

void keyboard(unsigned char key, int x, int y){
    count = 0;
    if(key == 'r'){				// reset operation
        //glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)image1);
        //glutSwapBuffers(); 	
    }   
    else if(key == 't'){			// enter text
        entertext = 1;
        cropimage = 0;
        drawline  = 0;
    }
    else if(key == 'c'){			// crop image
        entertext = 0;
        cropimage = 1;
        drawline  = 0;
    }
    else if(key == 'l'){			// draw line
        entertext = 0;
        cropimage = 0;
        drawline  = 1;
    }
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

void menufunaaa(int value){
    count = 0;
    if(value == 0){
        glutDestroyWindow(winmenu);
        exit(0);
    }
    else if(value == 1){		// draw line
        entertext = 0;	   cropimage = 0;	   drawline  = 1;   drawpoly  = 0;
    } 	
    else if(value == 2){		// crop image
        entertext = 0;	   cropimage = 1;	   drawline  = 0;   drawpoly  = 0;
    }
    else if(value == 3){		// Annotate point
        entertext = 1;	   cropimage = 0;	   drawline  = 0;	drawpoly  = 0;	
    }
    else if(value == 4){		// reset
        reset();
    }
    else if(value == 6){		// star monotone
        drawpoly = 1;
        donepoly = 0;
        polytype = 2;
    }
    else if(value == 7){		// y monotone
        drawpoly = 1;
        donepoly = 0;
        polytype = 1;
    }
    else if(value == 8){		// convex polygon
        drawpoly = 1;
        donepoly = 0;
        polytype = 3;
    }
    else if(value == 9){		// done polygon	
        drawpoly = 0;
        donepoly = 1;		
    }
}

void createmymenua(void){		
    int polygonmenu = glutCreateMenu(menufunaaa);
    glutAddMenuEntry("Closed Contour", 5);
    glutAddMenuEntry("Star polygon", 6);
    glutAddMenuEntry("Y monotone", 7);
    glutAddMenuEntry("Convex polygon", 8);
    
    menuid = glutCreateMenu(menufunaaa);	
    glutAddMenuEntry("Draw Line", 1);
    glutAddMenuEntry("Crop Image", 2);
    glutAddMenuEntry("Annotate", 3);
    glutAddSubMenu("Polygon", polygonmenu);
    glutAddMenuEntry("Done", 9);
    glutAddMenuEntry("Reset", 4);
    glutAddMenuEntry("Exit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
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



int main()
{
    Mat imga = imread("/Users/pranjal/Desktop/img3.png", CV_LOAD_IMAGE_GRAYSCALE);
    Mat bw = imga > 128;
    Mat img = bw > 120;
    
    bitwise_not(bw, img);
    thinning(img);
    
    
    //IplImage* img = cvLoadImage( "/Users/pranjal/Desktop/img2_2.png" , CV_LOAD_IMAGE_GRAYSCALE);
    cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE );
    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", img );                   // Show our image inside it.

    cvWaitKey(0);
    cvDestroyWindow("Example1");
    return 0;
}
