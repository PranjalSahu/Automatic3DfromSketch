//
//  mypoint.h
//  sketch
//
//  Created by Pranjal Sahu on 11/4/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#ifndef mypoint_h
#define mypoint_h

class mypoint{
public:
    float x, y, z;
    mypoint(float a, float b, float c);
    mypoint(mypoint *t);
    mypoint();
};

#endif /* mypoint_h */
