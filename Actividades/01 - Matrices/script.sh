for size in 500 600 800 1000; do
    for tests in {1..10}; do
        ./example $size >>Results/Threads.doc
    done
    echo "------------------" $size >>Results/Threads.doc
done
