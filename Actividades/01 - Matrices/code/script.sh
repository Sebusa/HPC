for size in 500 600 800 1000 2000 2500 3000 4000; do
    for threads in 2 4 8 16; do
        for i in {1..10}; do
            .././hilos $size $threads >>../Results/'Threads Linux.doc'
        done
        echo "------------------" $size $threads >>../Results/'Threads Linux.doc'
    done
done
