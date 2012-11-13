#!/bin/bash

NUM=568

IMAGE_LIST=/home/taowu/work/data/frgc-ver2/frgc_images/all_images_list.txt
GRAPH=../data/standford/wiki-Vote_sub$NUM.dot
OUTPUT_PREFIX=/tmp/test_20121111
CMD="./album_generator -i $IMAGE_LIST -g $GRAPH -o $OUTPUT_PREFIX"

echo $CMD
$CMD
