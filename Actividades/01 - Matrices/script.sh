for size in 500 600 800 1000 2000 2500 3000 4000; do
    for threads in {1..10}; do
        for i in {1..10}; do
            ./threads $size $threads >>Results/Threads.doc
        done
        echo "------------------" $size $threads >>Results/Threads.doc
    done
done
