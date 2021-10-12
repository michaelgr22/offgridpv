#ifndef STRAIGHT_H
#define STRAIGHT_H

#include "coordinate.h"

class Straight
{
public:
    double m;
    double b;

    Straight(double m, double b);
    Straight(Coordinate foot, Coordinate top);
    double calculateGradient(Coordinate foot, Coordinate top);
};

#endif