#!/bin/bash

NUM=568
INPUT=/home/taowu/work/sn/data/standford/wiki-Vote.txt
OUTPUT=/home/taowu/work/sn/data/standford/wiki-Vote_sub$NUM.dot

CMD="./extract_standford -n $NUM $INPUT $OUTPUT"

echo $CMD
$CMD
