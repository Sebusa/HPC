array_size=(100000 250000 500000 1000000 2000000 5000000)
iterations=(1000 2000 5000 10000 20000 50000)
cluster_nodes=4

#MPI processing
echo "MPI test in progress..."
for size in ${array_size[@]}; do
    for i in ${iterations[@]}; do
        echo "------------------" $size $i >>results/'mpi.out'
        for i in {1..3}; do
            mpirun --hostfile hosts -np $cluster_nodes ./mpi $size $i >>results/'mpi.out'
        done
    done
done
echo "done!"

#MPI processing
echo "serial test in progress..."
for size in ${array_size[@]}; do
    for i in ${iterations[@]}; do
        echo "------------------" $size $i >>results/'serial.out'
        for i in {1..3}; do
            ./serial $size $i >>results/'serial.out'
        done
    done
done
echo "done!"