#! /bin/bash

# store a variable used to sum up a return value
ret=0
# compile all shader and add up if they failed
glslc finalize.comp -o finalize.comp.spv
ret=$(($ret + $?))
glslc test.comp -o test.comp.spv
ret=$(($ret + $?))
glslc simple.vert -o simple.vert.spv
ret=$(($ret + $?))
glslc simple.frag -o simple.frag.spv
ret=$(($ret + $?))
glslc fancy.frag -o fancy.frag.spv
ret=$(($ret + $?))

# return the sum of failures
exit $ret