#!/bin/bash

res=$((0))
PATH=$PWD/shader
$PATH/glslc.sh $PATH/main.vert -o $PATH/vert.spv
res=$(($?))
$PATH/glslc.sh $PATH/main.frag -o $PATH/frag.spv
res=$(($res+$?))
echo $(($res))