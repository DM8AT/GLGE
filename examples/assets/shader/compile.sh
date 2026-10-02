#! /bin/bash

# store a variable used to sum up a return value
ret=0
# compile all shader and add up if they failed
glslc simple.vert -o simple.vert.spv
ret=$(($ret + $?))
glslc simple.frag -o simple.frag.spv
ret=$(($ret + $?))

glslc crt.comp -o crt.comp.spv
ret=$(($ret + $?))

glslc blur_horizontal.comp -o blur_horizontal.comp.spv
ret=$(($ret + $?))
glslc blur_vertical.comp -o blur_vertical.comp.spv
ret=$(($ret + $?))


# return the sum of failures
exit $ret