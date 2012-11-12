#!/bin/sh

#source_dot_file=/home/taowu/work/sn/data/graph20.dot
source_dot_file=/tmp/test_20121110.dot
for alb_file in `ls /tmp/sn_*.alb | sort`;
do
  dot_file=`echo ${alb_file} | sed "s/alb/dot/"`
  cp $source_dot_file $dot_file 
  file=`echo ${alb_file} | sed "s/\.alb//"`
  ./analysis -i $file -o $file

  # sed '2inode[shape="circle", label="", height=0.2];' $dot_file > $file
  # mv $file $dot_file
  # sed 's/style=""/style="filled"/' $dot_file > $file
  # mv $file $dot_file
  # sed '/Infection/d' $dot_file > $file
  # mv $file $dot_file
  sed -i -e '2inode[shape="circle", label="", height=0.2];' -e 's/style=""/style="filled"/' -e '/Infection/d' $dot_file
  neato -Tpng $dot_file -o$file.png

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
