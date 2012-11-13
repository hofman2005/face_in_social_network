#!/bin/bash

IMAGE_PREFIX=/home/taowu/work/data/frgc-ver2/frgc_images_16x16/
GRAPH_PREFIX=/tmp/test_20121110
OUTPUT_PREFIX=/tmp/res_20121110

CMD="./contagion --image-prefix $IMAGE_PREFIX --graph-prefix $GRAPH_PREFIX -o $OUTPUT_PREFIX"

echo $CMD
$CMD
