#serial processing
echo "Serial test in progress..."
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    echo "------------------" $size >>results/'Serial.csv'
    for i in {1..10}; do
        run/./serial $size >>results/'Serial.csv'
    done
done
echo "done!"


#parallel processing
echo "Parallel test in progress..."
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
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
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    echo "------------------" $size >>results/'Memory.csv'
    for i in {1..10}; do
        run/./memoryLines $size >>results/'Memory.csv'
    done
done
echo "done!"

#optimized by compiler
echo "Optimized by compiler test in progress..."
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    echo "------------------" $size >>results/'Compiler.csv'
    for i in {1..10}; do
        run/./o3.out $size >>results/'Compiler.csv'
    done
done
echo "done!"

#Optimized by processes (?)
echo "Optimized by processes test in progress..."
for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    for processes in 2 4 8; do
        echo "------------------" $size $processes >>results/'Processes.csv'
        for i in {1..10}; do
            run/./processes $size $processes >>results/'Processes.csv'
        done
    done
done
echo "Finished!"