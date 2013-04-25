#!/bin/bash

EXP_SN=20130130_1
NUM=15
LABEL_PERCENT=1
WRONG_LABEL_PERCENT=0.4
RESULT_PATH=/tmp/$EXP_SN

# INPUT_DATASET=/home/taowu/work/sn/data/standford/wiki-Vote.txt
INPUT_DATASET=/home/taowu/work/sn/data/standford/ca-GrQc.txt.backup
IMAGE_LIST=/home/taowu/work/data/frgc-ver2/frgc_images/all_images_list.txt
# IMAGE_PREFIX=/home/taowu/work/data/frgc-ver2/frgc_images_16x16/
IMAGE_PREFIX=/home/taowu/work/data/frgc-ver2/features/

GRAPH=$RESULT_PATH/sub$NUM.dot
GRAPH_PREFIX=$RESULT_PATH/graph
OUTPUT_PREFIX=$RESULT_PATH/res

mkdir -p $RESULT_PATH

# Extract network from stanford dataset
echo "** Extract network from stanford dataset **"
CMD="./extract_standford -n $NUM $INPUT_DATASET $GRAPH"
echo $CMD
$CMD
while [ $? -ne $NUM ]
do
  $CMD
done

# Generate the album
echo "** Generate the album **"
GRAPH_OUTPUT_PREFIX=$RESULT_PATH/graph
CMD="./album_generator -i $IMAGE_LIST -g $GRAPH -o $GRAPH_OUTPUT_PREFIX -l $LABEL_PERCENT -w $WRONG_LABEL_PERCENT"
echo $CMD
$CMD

# Contagion
echo "** Contagion **"
CMD="./contagion --image-prefix $IMAGE_PREFIX --graph-prefix $GRAPH_PREFIX -o $OUTPUT_PREFIX"

echo $CMD
$CMD

# Analysis
echo "** Analysis **"
source_dot_file=$RESULT_PATH/graph.dot
for alb_file in `ls /tmp/sn_*stage.alb | sort`;
do
  dot_file=`echo ${alb_file} | sed "s/alb/dot/"`
  cp $source_dot_file $dot_file 
  file=`echo ${alb_file} | sed "s/\.alb//"`

  inter_result_file=${file}_readout
  inter_result_alb_file=${inter_result_file}.alb
  inter_result_dot_file=${inter_result_file}.dot 
  
  cp $source_dot_file ${inter_result_dot_file}
  ./final_readout -i ${alb_file} -o ${inter_result_alb_file}
  ./analysis -i ${inter_result_file} -o ${inter_result_file}
 
  # sed -i -e '2inode[shape="ellipse", height=0.2, fontsize=10];' -e 's/style=""/style="filled"/' -e '/Infection/d' -e 's/accuracy/label/' $inter_result_dot_file
  # neato -Tpng $inter_result_dot_file -o$inter_result_file.png

done


# # Baseline
# echo "** Baseline  **"
# CMD="./baseline_all --image-prefix $IMAGE_PREFIX --graph-prefix $GRAPH_PREFIX -o ${OUTPUT_PREFIX}_baseline_all"
# echo $CMD
# $CMD
# 
# # Baseline
# echo "** Baseline  **"
# CMD="./baseline --image-prefix $IMAGE_PREFIX --graph-prefix $GRAPH_PREFIX -o ${OUTPUT_PREFIX}_baseline"
# echo $CMD
# $CMD
# result_alb_file=${OUTPUT_PREFIX}_baseline_readout.alb
# result_file=${OUTPUT_PREFIX}_baseline_readout
# ./final_readout -i ${OUTPUT_PREFIX}_baseline.alb -o ${result_alb_file}
# cp ${OUTPUT_PREFIX}.dot ${result_file}.dot
# ./analysis -i ${result_file} -o ${result_file}_result


