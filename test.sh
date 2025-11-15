#!/bin/bash

# Запрос
QUERY="SELECT * FROM employees WHERE gender='Male' AND full_name LIKE 'F%';"

# Кол-во прогонов
RUNS=5

# Переменная для суммы времени
sum=0

for i in $(seq 1 $RUNS)
do
    echo "Run $i"

    # Очистка кэша ОС
    sudo sync
    sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"

    # Выполнение запроса, вывод результата скрыт, stderr тоже скрыт
    TIME=$(sudo -u postgres psql -d employees 2>/dev/null <<EOF | grep "Time:" | awk '{print $2}'
SET jit = off;
DISCARD ALL; 
\timing on
\o /dev/null
$QUERY
\o
EOF
)

    echo "Execution time: ${TIME} ms"

    sum=$(echo "$sum + $TIME" | bc)
done

# Подсчёт среднего времени
avg=$(echo "scale=2; $sum / $RUNS" | bc)
echo "Average execution time over $RUNS runs: ${avg} ms"
