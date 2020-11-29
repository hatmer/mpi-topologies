#include <iostream>
#include <stdio.h>
#include "mpi.h"
#include <string>


void do_work(int *output, int outputSize, int *data)
{
  std::fill(output, output+outputSize, 1);
  return;
}

void usage(char *program)
{
  std::cout << "Usage: " << program << " <size> <p> (where size >= 2, 1 < p)" << std::endl;
  exit(1);
}


int main(int argc, char *argv[])
{
  if (argc != 3)
    {
      usage(argv[0]);
    }

  int modelSize, numProcesses;
  try
    {
      modelSize = std::stoi(argv[1]);
      numProcesses = std::stoi(argv[2]);
    }
  catch (std::exception)
    {
      usage(argv[0]);
    }
  if (modelSize < 2 || numProcesses < 2)
    {
      usage(argv[0]);
    }

  int my_rank, comm_sz;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  
  int dataSize = modelSize;
  int* data = new int[dataSize];
  
  // initialize dummy data
  if (my_rank == 0) {
     std::fill(data, data+dataSize, 1);
  }
  
  int* my_output = new int[dataSize];
  int* inputs = new int[dataSize];
  
  // each worker does stuff, then passes data to next worker, until last worker sends to 0
  if (my_rank == 0) {
    std::cout << "rank 0 is starting" << std::endl;
    do_work(my_output, dataSize, data);
    std::cout << "rank 0 is sending" << std::endl;
    MPI_Send((void *) my_output, dataSize, MPI_INT, (my_rank+1), my_rank, MPI_COMM_WORLD);
    std::cout << "rank 0 is receiving" << std::endl;
    MPI_Recv(inputs, dataSize, MPI_INT, numProcesses-1, -1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive last layer outputs
  } else {
    std::cout << "rank " << my_rank << " is receiving" << std::endl;
    MPI_Recv(inputs, dataSize, MPI_INT, (my_rank-1), -1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    do_work(my_output, dataSize, inputs);
    std::cout << "rank " << my_rank << " is sending" << std::endl;
    MPI_Send((void *) my_output, dataSize, MPI_INT, (my_rank+1)%(numProcesses), my_rank, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
