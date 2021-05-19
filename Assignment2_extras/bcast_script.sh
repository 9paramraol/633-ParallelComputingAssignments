# mpicc our_bcast.c src.c

echo "" > bcast_results
for execution in 1 2 3 
do
	for ppn in 1 8
	do
		./gen_host_file.sh 4 4 $ppn	> gbj
		for D in 16384 262144 2097152
		do
			echo "4X4 iteration ---"$execution"----"$ppn"----"$D"----------------"  >> bcast_results
			nps=$(( 16*ppn ))
			mpirun -np $nps -f hostfile1 ./a.out $D $ppn>> bcast_results
			# echo $nps
		done
	done
done

echo "4X4 done -----------"

for execution in 1 2 3 
do
	for ppn in 1 8
	do
		./gen_host_file.sh 2 8 $ppn	> gbj
		for D in 16384 262144 2097152
		do
			echo "2X8 iteration ---"$execution"----"$ppn"----"$D"----------------"  >> bcast_results
			nps=$(( 16*ppn ))
			mpirun -np $nps -f hostfile1 ./a.out $D $ppn>> bcast_results
			# echo $nps
		done
	done
done


echo "2X8 done -----------"
for execution in 1 2 3
do
	for ppn in 1 8
	do
		./gen_host_file.sh 4 1 $ppn	> gbj
		for D in 16384 262144 2097152
		do
			echo "4X1 iteration ---"$execution"----"$ppn"----"$D"----------------"  >> bcast_results
			nps=$(( 4*ppn ))
			mpirun -np $nps -f hostfile1 ./a.out $D $ppn>> bcast_results
			# echo $nps
		done
	done
done				

echo "4X1 done -----------"
for execution in 1 2 3
do
	for ppn in 1 8
	do
		./gen_host_file.sh 2 2 $ppn	> gbj
		for D in 16384 262144 2097152
		do
			echo "2X2 iteration ---"$execution"----"$ppn"----"$D"----------------"  >> bcast_results
			nps=$(( 4*ppn ))
			mpirun -np $nps -f hostfile1 ./a.out $D $ppn>> bcast_results
			# echo $nps
		done
	done
done				

echo "2X2 done -----------"
for execution in 1 2 3
do
	for ppn in 1 8
	do
		./gen_host_file.sh 1 4 $ppn	
		for D in 16384 262144 2097152
		do
			echo "1X4 iteration ---"$execution"----"$ppn"----"$D"----------------"  >> bcast_results
			nps=$(( 4*ppn ))
			mpirun -np $nps -f hostfile1 ./a.out $D $ppn>> bcast_results
			# echo $nps
		done
	done
done				

echo "1X4 done -----------"
