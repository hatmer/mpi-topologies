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
  std::cout << "Usage: " << program << " <size> <layerSize> <numLayers> (where size >= 2, layerSize > 0, numLayers > 0)" << std::endl;
  exit(1);
}


int main(int argc, char *argv[])
{
  if (argc != 4)
    {
      usage(argv[0]);
    }

  int modelSize, layerSize, numLayers;
  try
    {
      modelSize = std::stoi(argv[1]);
      layerSize = std::stoi(argv[2]);
      numLayers = std::stoi(argv[3]);
    }
  catch (std::exception)
    {
      usage(argv[0]);
    }
  if (modelSize < 2 || layerSize < 1 || numLayers < 1)
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
  int i;
  int next_layer_start, next_layer_end, prev_layer_start, prev_layer_end;

  // each worker does stuff, then broadcasts data to all workers in next layer, until last layer sends to first layer
  if (my_rank < layerSize) { // in first layer
    std::cout << "rank " << my_rank << " is starting" << std::endl;
    do_work(my_output, dataSize, data);
    std::cout << "rank " << my_rank << " is broadcasting to next layer" << std::endl;
    next_layer_start = layerSize;
    next_layer_end = next_layer_start + layerSize - 1;
    for (i = next_layer_start; i <= next_layer_end; i++) { 
      MPI_Send((void *) my_output, dataSize, MPI_INT, i, my_rank, MPI_COMM_WORLD);
    }
    std::cout << "rank " << my_rank << " is waiting to receive " << layerSize << " messages" << std::endl;
    prev_layer_start = layerSize * numLayers - layerSize;
    prev_layer_end = layerSize * numLayers - 1;
    for (i = prev_layer_start; i <= prev_layer_end; i++) {
      MPI_Recv(inputs, dataSize, MPI_INT, i, -1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive last layer outputs
      // combine into final output
    }

  } else {
    std::cout << "rank " << my_rank << " is receiving" << std::endl;
    prev_layer_start = (my_rank / layerSize - 1) * layerSize;
    prev_layer_end = (my_rank / layerSize) * layerSize - 1;
    for (i = prev_layer_start; i <= prev_layer_end; i++) {
      MPI_Recv(inputs, dataSize, MPI_INT, i, -1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // combine inputs here
    }
    do_work(my_output, dataSize, inputs);
    std::cout << "rank " << my_rank << " is sending" << std::endl;
    next_layer_start = ((my_rank / layerSize + 1) * layerSize) % (layerSize * numLayers);
    next_layer_end = next_layer_start + layerSize - 1;
    for (i = next_layer_start; i <= next_layer_end; i++) {
      std::cout << " -> " << i << std::endl;
      MPI_Send((void *) my_output, dataSize, MPI_INT, i, my_rank, MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();
  return 0;
}
