#include "GLGE/GLGE.h"

int main()
{
    dvec2 a = dvec2_create(3, 6);
    dvec2 b = dvec2_createSingle(2);
    dvec2_negateThis(&b);

    dvec2 sum = dvec2_add(a, b);
    const char* str = dvec2_asString(sum);
    printf("The sum is: %s. Dot product of sum and a is: %f\n", str, dvec2_dot(sum, a));
    free((void*)str);
}