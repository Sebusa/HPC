for size in 500 1000 1500 2000 2500 3000 3500 4000 4500 5000; do
    echo "------------------" $size >>../Results/'Sequential.csv'
    for i in {1..10}; do
        .././secuencial $size >>../Results/'Sequential.csv'
    done
done
