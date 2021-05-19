echo "size 16KB----------------------------------"
mpirun -np 128 -f hostfile1 ./a.out 16384
echo "size 256KB----------------------------------"
mpirun -np 128 -f hostfile1 ./a.out 262144
echo "size 2048KB----------------------------------"
mpirun -np 128 -f hostfile1 ./a.out 2097152