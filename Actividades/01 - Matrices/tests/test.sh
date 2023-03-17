for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do

        #serial processing
        echo "Serial test in progress..."
        echo "------------------" $size >>../results/'Serial.csv'
        for i in {1..10}; do
            ../run/./serial $size >>../results/'Serial.csv'
        done
        echo "done!"

        #parallel processing
        echo "Parallel test in progress..."
        for threads in 2 4 8; do
            echo "------------------" $size $threads >>../results/'Threads.csv'
            for i in {1..10}; do
                ../run/./threads $size $threads >>../results/'Threads.csv'
            done
        done
        echo "done!"

        #cache memory line optimization
        echo "Optimized by memory test in progress..."
        echo "------------------" $size >>../results/'Memory.csv'
        for i in {1..10}; do
            ../run/./optimize $size >>../results/'Memory.csv'
        done
        echo "done!"

        #optimized by compiler
        echo "Optimized by compiler test in progress..."
        echo "------------------" $size >>../restults/'Compiler.csv'
        for i in {1..10}; do
            ../run/./o3.out $size >>../results/'Compiler.csv'
        done
        echo "done!"

done