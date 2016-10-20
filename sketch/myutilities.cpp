//
//  myutilities.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/19/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#include <stdio.h>
#include "myutilities.h"


mypoint::mypoint(GLfloat a,GLfloat b, GLfloat c){
    x = a;
    y = b;
    z = c;
}
mypoint::mypoint(mypoint *t){
    x = t->x;
    y = t->y;
    z = t->z;
}
mypoint::mypoint(){
    x = 0;
    y = 0;
    z = 0;
}