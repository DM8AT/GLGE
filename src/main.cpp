#include "GLGE/GLGE.h"

int main()
{
    dvec2 a(3, 6);
    dvec2 b = 2;
    b = -b;

    dvec2 sum = a + b;
    std::cout << "The sum is: " << sum << ". Dot product of sum and a is: " << dot(sum, a) << "\n";
}