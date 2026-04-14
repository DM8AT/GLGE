#! /bin/bash

# store a variable used to sum up a return value
ret=0
# compile all shader and add up if they failed
glslc finalize.comp -o finalize.comp.spv
ret=$(($ret + $?))
glslc rt_sphere.comp -o rt_sphere.comp.spv
ret=$(($ret + $?))
glslc simple.vert -o simple.vert.spv
ret=$(($ret + $?))
glslc simple.frag -o simple.frag.spv
ret=$(($ret + $?))
glslc culling.comp -o culling.comp.spv
ret=$(($ret + $?))

# return the sum of failures
exit $ret