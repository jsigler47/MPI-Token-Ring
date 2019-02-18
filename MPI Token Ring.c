#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	int debug = 0; //Set to one for debug output.
	int laps;
	if(argc < 2)
	{
		printf("Argument \"N\" required.\n");
		return 0;
	}
	else
	{
		laps = atoi(argv[1]);
	}
	MPI_Init(&argc, &argv);

	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int token = 0;

	if(my_rank == 0)
	{
		printf("Token = # of laps * 2\nStarting with token: %d.\n", token);
		//Repeat the send and receive process until the token = 2 * laps (token = token + 2 each lap).
		//Token change happens before 0 sends token out.
		while(token < (2 * laps ))
		{
			if(debug == 1)
				printf("Master lap #%d\n", token / 2);
			
			token += 2;

			if(debug == 1)
				printf("0 sending %d to 1.\n", token);
			//0 will start the sending and wait for the tokens return once per lap.
			MPI_Send(&token, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD, NULL);
			
			if(debug == 1)
				printf("0 received %d, from %d\n", token, world_size - 1);
		}
		printf("Ending with token: %d.\n", token);
	}
	else
	{
		while(token < (2 * laps))
		{
			if(debug == 1)
				printf("Slave Lap #%d\n", token / 2);
			//The final node sends the token back to 0.
			if(my_rank == world_size - 1)
			{
				MPI_Recv(&token, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, NULL);
				if(debug == 1)
				{
					printf("%d received %d from %d\n", my_rank, token, my_rank - 1);
					printf("%d sending %d to 0\n", my_rank, token);
				}
				MPI_Send(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				
			}
			//All other nodes push the token to the next node.
			else
			{
				MPI_Recv(&token, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, NULL);
				if(debug == 1)
				{
					printf("%d received %d from %d\n", my_rank, token, my_rank - 1);
					printf("%d sending %d to %d\n", my_rank, token, my_rank + 1);
				}
				MPI_Send(&token, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
			}
		}
	}
	MPI_Finalize();
	return 0;
}