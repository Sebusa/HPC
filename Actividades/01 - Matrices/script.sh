for size in 500 600 800 1000 2000 2500 3000 4000; do
    echo "------------------" $size >>results.doc
    for tests in {1..10}; do
        ./example $size >>results.doc
    done
done
