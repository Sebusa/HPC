for size in 500 1000 1500 2000 2500 3000 3500 4000 5000; do
    echo "------------------" $size >>../tests/'Sequential.csv'
    for i in {1..10}; do
        .././secuencial $size >>../tests/'Sequential.csv'
    done
done
