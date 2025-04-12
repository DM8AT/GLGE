#include "GLGE/GLGE.h"

int main()
{
    //all graphic APIs that could work
    APIs selectFrom[] = {
        API_VULKAN_1_2_INDIRECT,
        API_VULKAN_1_2_DEFAULT,
        API_OPENGL_4_6,
        API_OPENGL_3_3,
        API_SOFTWARE
    };
    //get the best supported graphics api
    APIs best = getBestGraphicAPI(selectFrom, sizeof(selectFrom)/sizeof(*selectFrom));
    //error check if no good API was found
    if (best == API_FALLBACK_ERROR)
    {
        //print an error
        printf("[ERROR] No graphic API was supported\n");
    }
    printf("Selected grapihcs API: %s\n", apiToString(best));

    dvec2 a = dvec2_create(3, 6);
    dvec2 b = dvec2_createSingle(2);
    dvec2_negateThis(&b);

    dvec2 sum = dvec2_add(a, b);
    const char* str = dvec2_asString(sum);
    printf("The sum is: %s. Dot product of sum and a is: %f\n", str, dvec2_dot(sum, a));
    free((void*)str);
}