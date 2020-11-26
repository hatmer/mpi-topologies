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

  // map-reduce node sends data to each worker in the layer, collects the result, sends to next layer
  if (my_rank == 0) { 
    int currentLayer;
    int layer_start, layer_end;
    int i;
    for (currentLayer = 1; currentLayer <= numLayers; currentLayer++) {
      std::cout << "rank 0 is sending to layer " << currentLayer << std::endl;
      layer_start = (currentLayer - 1) * layerSize + 1;
      layer_end = currentLayer * layerSize;
      for (i = layer_start; i <= layer_end; i++) { 
        MPI_Send((void *) my_output, dataSize, MPI_INT, i, my_rank, MPI_COMM_WORLD);
      }
      std::cout << "rank 0 is waiting to receive " << layerSize << " messages" << std::endl;
      for (i = layer_start; i <= layer_end; i++) {
        MPI_Recv(inputs, dataSize, MPI_INT, i, -1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive last layer outputs
        // combine and update my_output
      }
    }

  } else {
    std::cout << "rank " << my_rank << " is receiving" << std::endl;
    MPI_Recv(inputs, dataSize, MPI_INT, 0, -1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    do_work(my_output, dataSize, inputs);
    std::cout << "rank " << my_rank << " is sending" << std::endl;
    MPI_Send((void *) my_output, dataSize, MPI_INT, 0, my_rank, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
