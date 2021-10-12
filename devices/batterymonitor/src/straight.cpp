#include "straight.h"

Straight::Straight(double m, double b)
    : m(m), b(b) {}

Straight::Straight(Coordinate foot, Coordinate top)
{
    m = calculateGradient(foot, top);
    b = foot.y - m * foot.x;
}

double Straight::calculateGradient(Coordinate foot, Coordinate top)
{
    return (top.y - foot.y) / (top.x - foot.x);
}