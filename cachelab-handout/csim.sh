echo 'cacheLab partA evaluation'
echo 'author: MCX;     ID: hinmo'
date
echo '\n'

echo 'Trial1: \n./csimxxx -s 1 -E 1 -b 1 -t traces/yi2.trace'
echo 'Result: ./csim; ./csim-ref:'
./csim -s 1 -E 1 -b 1 -t traces/yi2.trace
./csim-ref -s 1 -E 1 -b 1 -t traces/yi2.trace
echo '\n'

echo 'Trial2: \n./csimxxx -s 4 -E 2 -b 4 -t traces/yi.trace'
echo 'Result: ./csim; ./csim-ref: '
./csim -s 4 -E 2 -b 4 -t traces/yi.trace
./csim-ref -s 4 -E 2 -b 4 -t traces/yi.trace
echo '\n'

echo 'Trial3: \n./csimxxx -s 2 -E 1 -b 4 -t traces/dave.trace'
echo 'Result: ./csim; ./csim-ref: '
./csim -s 2 -E 1 -b 4 -t traces/dave.trace
./csim-ref -s 2 -E 1 -b 4 -t traces/dave.trace
echo '\n'

echo 'Trial4: \n./csimxxx -s 2 -E 1 -b 3 -t traces/trans.trace'
echo 'Result: ./csim; ./csim-ref: '
./csim -s 2 -E 1 -b 3 -t traces/trans.trace
./csim-ref -s 2 -E 1 -b 3 -t traces/trans.trace
echo '\n'

echo 'Trial5: \n./csimxxx -s 2 -E 2 -b 3 -t traces/trans.trace'
echo 'Result: ./csim; ./csim-ref: '
./csim -s 2 -E 2 -b 3 -t traces/trans.trace
./csim-ref -s 2 -E 2 -b 3 -t traces/trans.trace
echo '\n'

echo 'Trial6: \n./csimxxx -s 2 -E 4 -b 3 -t traces/trans.trace'
echo 'Result: ./csim; ./csim-ref: '
./csim -s 2 -E 4 -b 3 -t traces/trans.trace
./csim-ref -s 2 -E 4 -b 3 -t traces/trans.trace
echo '\n'

echo 'Trial7: \n./csimxxx -s 5 -E 1 -b 5 -t traces/trans.trace'
echo 'Result: ./csim; ./csim-ref: '
./csim -s 5 -E 1 -b 5 -t traces/trans.trace
./csim-ref -s 5 -E 1 -b 5 -t traces/trans.trace
echo '\n'

echo 'Trial8: \n./csimxxx -s 5 -E 1 -b 5 -t traces/long.trace'
echo 'Result: ./csim; ./csim-ref: '
./csim -s 5 -E 1 -b 5 -t traces/long.trace
./csim-ref -s 5 -E 1 -b 5 -t traces/long.trace
