#include "GLGE/GLGE.h"

int main()
{
    Triangle tri = Triangle(vec3(0,0,1), vec3(1,0,0), vec3(0,1,0));
    vec3 center = (tri.getA() + tri.getB() + tri.getC()) / 3.;

    std::cout << tri.getArea() << "\n";
    std::cout << tri.interpolate(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), center, 0) << "\n";
}