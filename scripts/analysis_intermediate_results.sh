#!/bin/sh

# source_dot_file=/home/taowu/work/sn/data/graph20_10labeled.dot
source_dot_file=/tmp/test_20121110.dot
for alb_file in `ls /tmp/sn_*.alb | sort`;
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
 
  #./analysis -i $file -o $file

  # sed '2inode[shape="circle", label="", height=0.2];' $dot_file > $file
  # mv $file $dot_file
  # sed 's/style=""/style="filled"/' $dot_file > $file
  # mv $file $dot_file
  # sed '/Infection/d' $dot_file > $file
  # mv $file $dot_file
  
  # sed -i -e '2inode[shape="circle", label="", height=0.2];' -e 's/style=""/style="filled"/' -e '/Infection/d' $inter_result_dot_file
  sed -i -e '2inode[shape="ellipse", height=0.2, fontsize=10];' -e 's/style=""/style="filled"/' -e '/Infection/d' -e 's/accuracy/label/' $inter_result_dot_file
  neato -Tpng $inter_result_dot_file -o$inter_result_file.png

  # dot_file=`echo ${dot_file} | sed "s/\.dot/false_alarm.dot/"`
  # file=`echo ${dot_file} | sed "s/\.dot//"`
  # # sed '2inode[shape="circle", label="", height=0.2];' $dot_file > $file
  # # mv $file $dot_file
  # # sed 's/style=""/style="filled"/' $dot_file > $file
  # # mv $file $dot_file
  # # sed '/Infection/d' $dot_file > $file
  # # mv $file $dot_file
  # sed -i -e '2inode[shape="circle", label="", height=0.2];' -e 's/style=""/style="filled"/' -e '/Infection/d' $dot_file
  # neato -Tpng $dot_file -o$file.png
done
