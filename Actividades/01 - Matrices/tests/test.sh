matrix_size=(500 1000 1500 2000 2500 3000 3500 4000 5000)
cluster_nodes = 4

#MPI processing
echo "MPI test in progress..."
for size in ${matrix_size[@]}; do
    echo "------------------" $size >>results/'MPI.out'
    for i in {1..10}; do
        mpirun -np $cluster_nodes ./MPI $size >>results/'MPI.out'
    done
done
echo "done!"

: '
#serial processing
echo "Serial test in progress..."
for size in ${matrix_size[@]}; do
    echo "------------------" $size >>results/'Serial.csv'
    for i in {1..10}; do
        run/./serial $size >>results/'Serial.csv'
    done
done
echo "done!"


#parallel processing
echo "Parallel test in progress..."
for size in ${matrix_size[@]}; do
    for threads in 2 4 8; do
        echo "------------------" $size $threads >>results/'Threads.csv'
        for i in {1..10}; do
            run/./threads $size $threads >>results/'Threads.csv'
        done
    done
done
echo "done!"

#cache memory line optimization
echo "Optimized by memory test in progress..."
for size in ${matrix_size[@]}; do
    echo "------------------" $size >>results/'Memory.csv'
    for i in {1..10}; do
        run/./memoryLines $size >>results/'Memory.csv'
    done
done
echo "done!"

#optimized by compiler
echo "Optimized by compiler test in progress..."
for size in ${matrix_size[@]}; do
    echo "------------------" $size >>results/'Compiler.csv'
    for i in {1..10}; do
        run/./o3.out $size >>results/'Compiler.csv'
    done
done
echo "done!"

#Optimized by processes (?)
echo "Optimized by processes test in progress..."
for size in ${matrix_size[@]}; do
    for processes in 2 4 8; do
        echo "------------------" $size $processes >>results/'Processes.csv'
        for i in {1..10}; do
            run/./processes $size $processes >>results/'Processes.csv'
        done
    done
done


#OpenMP processing
echo "OpenMP test in progress..."
for size in ${matrix_size[@]}; do
    echo "------------------" $size >>results/'OpenMP.out'
    for i in {1..10}; do
        run/./openMP $size >>results/'OpenMP.out'
    done
done
echo "done!"
'
echo "Finished!"