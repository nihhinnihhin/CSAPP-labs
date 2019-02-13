echo 'CacheLab partA evaluation'
echo 'Author: MCX;     ID: hinmo'
date
echo '\n'
echo 'Results sorted by operation for csim, i.e., 'L', 'S', 'M'\n'

 echo 'L operation'
 grep   'L' Trial5.txt # >>Trial5-sorted.txt
 
 echo 'S operation'
 grep   'S' Trial5.txt # >>Trial5-sorted.txt
 
 echo 'M operation'
 grep   'M' Trial5.txt # >>Trial5-sorted.txt

 echo 'L operation'
 grep   'L' Trial6.txt # >>Trial5-sorted.txt
 
 echo 'S operation'
 grep   'S' Trial6.txt # >>Trial5-sorted.txt
 
 echo 'M operation'
 grep   'M' Trial6.txt # >>Trial5-sorted.txt

#  echo 'L operation'
#  grep   'L' Trial5-ref.txt # >>Trial5-ref-sorted.txt
#  
#  echo 'S operation'
#  grep   'S' Trial5-ref.txt # >>Trial5-ref-sorted.txt
#  
#  echo 'M operation'
#  grep   'M' Trial5-ref.txt # >>Trial5-ref-sorted.txt
