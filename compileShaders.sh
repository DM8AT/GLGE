#!/bin/bash

res=$((0))
PATH=$PWD/shader
$PATH/glslc $PATH/main.vert -o $PATH/vert.spv
res=$(($?))
$PATH/glslc $PATH/main.frag -o $PATH/frag.spv
res=$(($res+$?))
echo $(($res))