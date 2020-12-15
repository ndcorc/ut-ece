all: multiply


test1: multiply
	mpiexec -n 1 ./multiply

test: multiply
	mpiexec -n 6 ./multiply

test3: multiply
	mpiexec -n 3 ./multiply

test15: multiply
	mpiexec -n 15 ./multiply

clean:
	rm result.txt multiply

multiply:
	mpicc matrixMPI.c -o multiply -lpthread


