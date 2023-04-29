#/bin/bash
: 'Parameters:
- Mesh size: $size
- Number of iterarions: $iterations
- (Threads/Processes) number of threads/processes: $threads/$processes
- (Optional) Output file name: $output
'

mesh_size=(100000 500000 1000000 2000000 5000000 10000000)
sweeps=(100 500 1000 2000 5000 10000)

: '
#serial processing
echo "Serial test in progress..."
for size in ${mesh_size[@]}; do
    for iteration in ${sweeps[@]}; do
        echo "------------------" $size $iteration >> results/'Serial.out'
        for i in {1..10}; do
            run/./jacobiSerial $size $iteration >> results/'Serial.out'
        done
    done
done
echo "done!"

#parallel processing
echo "Parallel test in progress..."
for size in ${mesh_size[@]}; do
    for iteration in ${sweeps[@]}; do
        for thread in 2 4 8; do
            echo "------------------" $size $iteration $thread >> results/'Threads.out'
            for i in {1..10}; do
                run/./jacobiThreads $size $iteration $thread >> results/'Threads.out'
            done
        done
    done
done
echo "done!"

#optimized by compiler
echo "Optimized by compiler test in progress..."
for size in ${mesh_size[@]}; do
    for iteration in ${sweeps[@]}; do
        echo "------------------" $size $iteration >> results/'Optimized.out'
        for i in {1..10}; do
            run/./jacobiO3.out $size $iteration >> results/'Optimized.out'
        done
    done
done
echo "done!"

#Optimized by processes (?)
echo "Optimized by processes test in progress..."
for size in ${mesh_size[@]}; do
    for iteration in ${sweeps[@]}; do
        for process in 2 4 8; do
            echo "------------------" $size $iteration $process >> results/'Processes.out'
            for i in {1..10}; do
                run/./jacobiProcesses $size $iteration $process >> results/'Processes.out'
            done
        done
    done
done
'
#OpenMP
echo "OpenMP test in progress..."
for size in ${mesh_size[@]}; do
    for iteration in ${sweeps[@]}; do
        echo "------------------" $size $iteration >> results/'OpenMP.out'
        for i in {1..10}; do
            run/./jacobiOpenMP $size $iteration >> results/'OpenMP.out'
        done
    done
done
echo "Finished!"
