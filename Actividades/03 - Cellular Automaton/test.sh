array_size=(100000 250000 500000 1000000 2000000 5000000)
iterations=(1000 2000 5000 10000 20000 50000)
cluster_nodes=4

#MPI processing
echo "MPI test in progress..."
for size in ${array_size[@]}; do
    for i in ${iterations[@]}; do
        echo "------------------" $size $i >>results/'Matrix.out'
        for i in {1..10}; do
            mpirun --hostfile hosts -np $cluster_nodes ./Matrix $size $i >>results/'cellular.out'
        done
    done
done
echo "done!"