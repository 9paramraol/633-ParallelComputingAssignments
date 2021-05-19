#!/bin/bash
# 1. compile using make
make
echo "the file is compiled successfully"
# 2. run the job for each N, P 5 times
for iter in 1 2 3 4 5
do
	for P in 16 36 49 64
	do
		~/UGP/allocator/src/allocator.out "$P" 8 > garbage
		for N in 256 1024 4096 16384 65536 262144 1048576 
		do
			echo $iter"-"$P"-"$N
			mpirun -np $P -f hosts ./src $N 50 >> "./data_"$P
		done
	done
done
# 3. plot the data
DIRECTORY=$(pwd)
ssh -X csews1 "cd ${DIRECTORY} ; python plot.py"
