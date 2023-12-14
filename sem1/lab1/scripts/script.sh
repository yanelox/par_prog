binary="../a.out"
echo "start" > res.csv
for ((i=1000; i<15001; i=i+1000))
do
	mpirun -n 1 $binary $i >> res.csv
	mpirun -n 6 $binary $i >> res.csv
done
