#! /bin/bash

# store a variable used to sum up a return value
ret=0
# compile all shader and add up if they failed
glslc simple.vert -o simple.vert.spv
ret=$(($ret + $?))
glslc simple.frag -o simple.frag.spv
ret=$(($ret + $?))

# debug default shader
glslc debug_default.vert -o debug_default.vert.spv
ret=$(($ret + $?))
glslc debug_default.frag -o debug_default.frag.spv
ret=$(($ret + $?))

# return the sum of failures
exit $ret