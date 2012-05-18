#!/bin/bash
rm time.txt
echo "slow0:" >> time.txt && (time ./slow0) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "slow1:" >> time.txt && (time ./slow1) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "slow2:" >> time.txt && (time ./slow2) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "slow3:" >> time.txt && (time ./slow3) 1> /dev/null 2>> time.txt

echo " " >> time.txt
echo "faster0:" >> time.txt && (time ./faster0) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "faster1:" >> time.txt && (time ./faster1) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "faster2:" >> time.txt && (time ./faster2) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "faster3:" >> time.txt && (time ./faster3) 1> /dev/null 2>> time.txt

echo " " >> time.txt
echo "fastest0:" >> time.txt && (time ./fastest0) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "fastest1:" >> time.txt && (time ./fastest1) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "fastest2:" >> time.txt && (time ./fastest2) 1> /dev/null 2>> time.txt
echo " " >> time.txt
echo "fastest3:" >> time.txt && (time ./fastest3) 1> /dev/null 2>> time.txt
