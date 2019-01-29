# parallelComputingMPI
Analyzed and implamented a system that uses Message passing interface library to communicate between two processes.
Aparajita Sahay
Assignmeent2 
Date : 2/14/2015
Heat 2D, output’s screen short : 
Performance for array size 100: 
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 100 2700 3000 500 4 > outi.txt
Elapsed time = 2305953
sahay87@uw1-320-00:~/lab2$ mpirun -n 1 ./heat 100 2700 3000 500 1 > out1.txt
Elapsed time = 643009
sahay87@uw1-320-00:~/lab2$ mpirun -n 1 ./heat 100 2700 3000 500 2 > out1.txt
Elapsed time = 448671
sahay87@uw1-320-00:~/lab2$ mpirun -n 1 ./heat 100 2700 3000 500 4 > out1.txt
Elapsed time = 433135
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 100 2700 3000 500 1 > out1.txt
Elapsed time = 2734426
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 100 2700 3000 500 2 > out1.txt
Elapsed time = 2402561
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 100 2700 3000 500 4 > out1.txt
Elapsed time = 2195498
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 100 2700 3000 500 1 > out1.txt
Elapsed time = 3964155
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 100 2700 3000 500 2 > out1.txt
Elapsed time = 4016628
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 100 2700 3000 500 4 > out1.txt
Elapsed time = 4014358
sahay87@uw1-320-00:~/lab2$ mpirun -n 8 ./heat 100 2700 3000 500 1 > out1.txt
Elapsed time = 7440906
sahay87@uw1-320-00:~/lab2$ mpirun -n 8 ./heat 100 2700 3000 500 2 > out1.txt
Elapsed time = 7941157
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 300 2700 3000 500 4 > out1.txt
Elapsed time = 18539651
sahay87@uw1-320-00:~/lab2$ mpirun -n 1 ./heat 300 2700 3000 500 4 > out1.txt
Elapsed time = 4046106
sahay87@uw1-320-00:~/lab2$ ./heatseq 100 2700 3000 500 >sequentiaoutput.txt
Elapsed time = 310171
Performance for array size : 300 
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 300 2700 3000 500 1 > out1.txt
Elapsed time = 19605172
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 300 2700 3000 500 4 > out1.txt
Elapsed time = 23503858
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 300 2700 3000 500 1 > out1.txt
Elapsed time = 36295849
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 300 2700 3000 500 4 > out1.txt
Elapsed time = 36059800
sahay87@uw1-320-00:~/lab2$ mpirun -n 8 ./heat 300 2700 3000 500 4 > out1.txt
Elapsed time = 27479343
sahay87@uw1-320-00:~/lab2$ mpirun -n 8 ./heat 300 2700 3000 500 1 > out1.txt
Elapsed time = 27891045
sahay87@uw1-320-00:~/lab2$ mpirun -n 1 ./heat 300 2700 3000 500 1 > out1.txt
Elapsed time = 6375685
sahay87@uw1-320-00:~/lab2$ mpirun -n 1 ./heat 300 2700 3000 500 2 > out1.txt
Elapsed time = 5814146
sahay87@uw1-320-00:~/lab2$ mpirun -n 1 ./heat 300 2700 3000 500 4 > out1.txt
Elapsed time = 4218301
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 300 2700 3000 500 1 > out1.txt
Elapsed time = 27304569
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 300 2700 3000 500 2 > out1.txt
Elapsed time = 25327942
sahay87@uw1-320-00:~/lab2$ mpirun -n 2 ./heat 300 2700 3000 500 4 > out1.txt
Elapsed time = 24829418
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 300 2700 3000 500 1 > out1.txt
Elapsed time = 35899552
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 300 2700 3000 500 2 > out1.txt
Elapsed time = 39889572
sahay87@uw1-320-00:~/lab2$ mpirun -n 4 ./heat 300 2700 3000 500 4 > out1.txt
Elapsed time = 37832388
sahay87@uw1-320-00:~/lab2$

1)	Elapsed time using 4 processors and 4 thread = 18539651  ,using 300 as array size 
              Elapsed time using 1 processors and 4 thread = 4046106
             So, performance improvement: .218%
2)	Elapsed time using 2 processors and 1 thread = 2734426 ,using 100 as array size
Elapsed time using 2 processors and 4 thread = 2195498
So, performance improvement: 1.2454%
1)	Elapsed time using 1 processors and 1 thread = 643009 , Using 100 as array size
Elapsed time using 4 processors and 4 thread = 4014358
So, performance improvement: 0.16017%.

MPI techniques:
1)	Used point to point communication technique for sending and receiving data between the processes.
2)	Master process used MPI_Send & MPI_Receive For assigning sections of array among the processes.
3)	Slaves process used MPI_recieve & MPI_Receive accepting their assigned sections and for sending the result back to  master processes.
4)	After the third loop to cover the corner cases I have used MPI_Send and MPI_Recv.
5)	Initially used MPI_Sendrecv to avoid deadlock among the processes but my program hanged in between the communication. Showed segmentation fault and communication error. 
In MPi_send and MPI_recv, I used tags argument to distinguish within the messages. When I was not using tag my MPI_Recv buffer was hanging during the execution and was showing communication error. After using tags communication within the process was working properly.
6)	Only master process will display the intermediate output. Before executing the display function collected data from all the slave processes into master process. And then master process executed display function.
7)	Used omp for to split up loop iterations among the threads. I also used default scheduling clause to divide for loop iteration among all the thread. 
8)	Scope of the variables are provate and firstprivate as there were few variable which were assigned some value before omp for nd to keep the value of variable used firstprivate.

For Lab work:
I have used parallelized the program using MPI.  Output matches with the output of professor’s program.
I used 400 as the matrix size.
Number of processes 	Elapsed time
2	197998
4	220626
8	
16	186936
252162

Time is not consistently increasing with the increase in number of processors. It varies with time.
Time spent : On an average 5 hours per day.

