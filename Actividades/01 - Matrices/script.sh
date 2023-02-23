for size in 10 100 200 400 600 800 1000 2000; do
    for tests in {1..10}; do
        ./example $size >>results.doc
    done
    echo "------------------" $size >>results.doc
done
