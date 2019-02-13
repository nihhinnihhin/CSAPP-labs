#!/bin/bash
echo 'CacheLab partA evaluation'
echo 'Author: MCX;     ID: hinmo'
date
echo '\n'
sh csim.sh > partA-Results.txt

./csim-ref -v -s 2 -E 2 -b 3 -t traces/trans.trace | grep -n '.*' > Trial5-ref.txt
./csim -v -s 2 -E 2 -b 3 -t traces/trans.trace | grep -n '.*' > Trial5.txt

sh resultHelper.sh > Trial5-sorted.txt
sh resultHelper-ref.sh > Trial5-sorted-ref.txt
