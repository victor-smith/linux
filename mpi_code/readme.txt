配置过程：https://blog.csdn.net/lusongno1/article/details/61709460
编译配置：https://github.com/pmodels/mpich
matrix计算：https://blog.csdn.net/xx_123_1_rj/article/details/41151863


 mpicc helloworld.c -o helloworld
 mpicc sumpi.c -o sumpi
 mpicc matrix.c -o matrix


 mpirun -n 10 ./helloworld
 mpirun -n 10 ./sumpi
 mpirun -n 2 ./matrix
