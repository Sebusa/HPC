#/bin/bash
: 'Parameters:
- Mesh size: $size
- Number of iterarions: $iterations
- (Threads/Processes) number of threads/processes: $threads/$processes
- (Optional) Output file name: $output
'
#serial processing
echo "Serial test in progress..."
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    echo "------------------" $size >>results/'Serial.csv'
    for i in {1..10}; do
        run/./jacobiSerial $size >>results/'Serial.csv'
    done
done
echo "done!"


#parallel processing
echo "Parallel test in progress..."
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    for threads in 2 4 8; do
        echo "------------------" $size $threads >>results/'Threads.csv'
        for i in {1..10}; do
            run/./jacobiThreads $size $threads >>results/'Threads.csv'
        done
    done
done
echo "done!"

#optimized by compiler
echo "Optimized by compiler test in progress..."
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    echo "------------------" $size >>results/'Compiler.csv'
    for i in {1..10}; do
        run/./jacobiO3.out $size >>results/'Compiler.csv'
    done
done
echo "done!"

#Optimized by processes (?)
echo "Optimized by processes test in progress..."
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    for processes in 2 4 8; do
        echo "------------------" $size $processes >>results/'Processes.csv'
        for i in {1..10}; do
            run/./jacobiProcesses $size $processes >>results/'Processes.csv'
        done
    done
done
echo "Finished!"