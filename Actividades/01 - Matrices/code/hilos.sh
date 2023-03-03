for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    for threads in 2 4; do
        echo "------------------" $size $threads >>../Results/'Threads.csv'
        for i in {1..10}; do
            .././hilos $size $threads >>../Results/'Threads.csv'
        done
    done
done
