#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "RLE_Parsing.h"


void CalculateUpdate(int my_rank, int step, char* readFrom, char* writeTo, int sizeX, int sizeY);
void Output(int my_rank, int total_ranks, FILE* outputfile, char* mat_start, int sizeX, int sizeY, int step);
void ExchangeGhostlines(int my_rank,int total_ranks,int matrix_dim_x, char* my_received_ghostline_top,char* my_received_ghostline_bot,char* pos_of_my_low,char* pos_of_my_high);
void ExchangeGhostlinesNonblocking(int my_rank,int total_ranks,int size, char* my_received_ghostline_top,char* my_received_ghostline_bot,char* pos_of_my_low,char* pos_of_my_high);
void Printmatrix(char* mat_start, int sizeX, int sizeY);

//Parameters: fileName amountOfSteps pictureEveryXSteps
int main(int argc, char *argv[]){
	int MAX_STEPS = 100;
	int OUTPUT_EVERY_X_STEPS = 20;
	if(argc >= 3){
		MAX_STEPS = atoi(argv[2]);
		OUTPUT_EVERY_X_STEPS = atoi(argv[3]);
	}

	FILE *outputfile;

	MPI_Init(&argc,&argv);
	int my_rank,total_ranks;
	MPI_Comm_size(MPI_COMM_WORLD,&total_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);


	if(my_rank == 0){
	 	outputfile = fopen("conway_output.txt", "w");
		if (outputfile == NULL)
		{
		    printf("Error opening file!\n");
		}
        
	}
	if(my_rank ==0 && argc < 2){
		printf("Please provide a file name");
		MPI_Abort(MPI_COMM_WORLD,1);

	}
	if(total_ranks < 3){
 		if(my_rank==0){printf("Please try with a least 3 ranks!");}
		return 0;
	}


	/*
		We want to have two Matrices in every step: One to read from and one to write to.
		Because we don't want to copy the write matrix to the read matrix after every step, we just make them switch their roles.
		Calculation of the sizes and creation of the arrays is done by the parser.
	*/
	char* my_matrix_part_writeeven;
	char* my_matrix_part_writeodd;
	int matrix_dim_x = -1;
	int total_matsize_y = -1;
    
    /*
		Loading input files
	*/
	loadFile(argv[1],&my_matrix_part_writeodd,&my_matrix_part_writeeven, &matrix_dim_x, &total_matsize_y);
        
	MPI_Barrier(MPI_COMM_WORLD);
    
	/*
		Verify correct parsing
	*/
	if(my_rank == 0){printf("Finished parsing data\n");}
	if(my_rank != 0 || outputfile != NULL){
		Output(my_rank,total_ranks,outputfile,my_matrix_part_writeodd,matrix_dim_x,total_matsize_y,-1);
	}

	for(int step_count = 0; step_count <= MAX_STEPS; step_count++){

		/*
			We give away the data from the matrix we wrote last step, so on an even step we give out writeodd, since last step was odd.
			Position of our lowest (non-ghost) line
		*/
		char* pos_of_my_low = my_matrix_part_writeeven + matrix_dim_x*(total_matsize_y-2);
		if(step_count % 2 == 0){
			pos_of_my_low = my_matrix_part_writeodd + matrix_dim_x*(total_matsize_y-2);
		}

		//Position of our highest (non-ghost) line
		char* pos_of_my_high = my_matrix_part_writeeven + matrix_dim_x;
		if(step_count % 2 == 0){
			pos_of_my_high = my_matrix_part_writeodd + matrix_dim_x;
		}

		//We save the received data in the matrix we will read in this step (so on an even step into writeodd)
		char* my_received_ghostline_top = my_matrix_part_writeodd;
		if(step_count % 2 == 1){
 			my_received_ghostline_top = my_matrix_part_writeeven;
		}
		char* my_received_ghostline_bot = my_matrix_part_writeodd + matrix_dim_x*(total_matsize_y-1);
		if(step_count % 2 == 1){
 			my_received_ghostline_bot = my_matrix_part_writeeven + matrix_dim_x*(total_matsize_y-1);
		}
        
		ExchangeGhostlines(my_rank,total_ranks,matrix_dim_x,my_received_ghostline_top,my_received_ghostline_bot,pos_of_my_low,pos_of_my_high);

		//ExchangeGhostlinesNonblocking(my_rank,total_ranks,matrix_dim_x,my_received_ghostline_top,my_received_ghostline_bot,pos_of_my_low,pos_of_my_high);
        
	 	if(step_count % 2 ==0){           
			CalculateUpdate(my_rank, step_count, my_matrix_part_writeodd,my_matrix_part_writeeven,matrix_dim_x,total_matsize_y);
		}else{
			CalculateUpdate(my_rank, step_count, my_matrix_part_writeeven,my_matrix_part_writeodd,matrix_dim_x,total_matsize_y);
		} 
        
		MPI_Barrier( MPI_COMM_WORLD );

		if(my_rank != 0 || outputfile != NULL){
			if(step_count % OUTPUT_EVERY_X_STEPS == 0){
				if(step_count % 2 == 0){
					Output(my_rank,total_ranks,outputfile,my_matrix_part_writeeven,matrix_dim_x,total_matsize_y,step_count);
				} else {
					Output(my_rank,total_ranks,outputfile,my_matrix_part_writeodd ,matrix_dim_x,total_matsize_y,step_count);
				}
			}
		}
		MPI_Barrier( MPI_COMM_WORLD );
        
    }
	if(my_rank == 0){
		fclose(outputfile);
	}
	free(my_matrix_part_writeeven);
	free(my_matrix_part_writeodd);

	MPI_Finalize();

	return 0;
}


void CalculateUpdate(int my_rank, int step, char* readFrom, char* writeTo, int sizeX, int totalSizeY){
/*
	readFrom = matrix we get our data from
	writeTo = ...
	sizeX = width of a row
	totalSizeY = how many rows does our matrix have (including the ghostlines at 0 and totalSizeY-1)
*/

	for(int y=totalSizeY-2;y>0;y--){
        for(int x=0;x<sizeX;x++){
			writeTo[x + sizeX*y] = readFrom[ x + sizeX*(y-1)];
		}
	}
    
/*
	Task B
	------
	Implement the actual logic for conway's game of life.
	Rules:
		- A living cell is represented as a 1, a dead cell is represented as a 0
		- A living cell stays alive if its surrounded by 2 or 3 living cells
		- A dead cell comes alive if it's surrounded by exactly 3 live cells
		- In all other cases the cell becaomes dead by over/underpopulation
	Notes:
		- writeTo[...] always needs to be updated even if the cells value wouldn't change from the one read (because of the role-flipping)
		- For counting neighbours, cast the values you read to int: (int)readFrom[...]
		- Which cells require special rules? Which don't?
	Solution:
		- The boject in the upper right of TestFile.rle behaves like a proper Lightweight Spaceship (http://conwaylife.com/w/index.php?title=Lightweight_spaceship)
*/
}

/*  Exchange ghost line between ranks:
    Order of sending and recieving of top line is keeps as
    Rank0 send its ghostline to Rank1. 
    Rank1 receives ghostline from Rank0 then send its ghostline to Rule2
    The process continue till rank end
    Since Rank0 top ghostline at very top of matrix, its recieved ghostline has to be initialize as 0
    
    A reversed process is apply for ghostline bottom
    MPI_Ssend is also applied to verify if deadlock happens
    */
void ExchangeGhostlines(int my_rank,int total_ranks,int size, char* my_received_ghostline_top,char* my_received_ghostline_bot,char* pos_of_my_low,char* pos_of_my_high){

    /* Exchange ghost line bottom of one node to ghost line top of  the next node */
    if(my_rank==0) {
        MPI_Ssend(pos_of_my_low, size, MPI_CHAR, my_rank+1, 1, MPI_COMM_WORLD);
        for(int i=0;i<size;i++){
        my_received_ghostline_top[i]=0;
        }
    } else if (my_rank < total_ranks -1) {
        MPI_Recv(my_received_ghostline_top, size, MPI_CHAR, my_rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Ssend(pos_of_my_low, size, MPI_CHAR, my_rank+1, 1, MPI_COMM_WORLD);        
    } else {
        MPI_Recv(my_received_ghostline_top, size, MPI_CHAR, my_rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    
    /* Exchange ghost line top of one node to ghost line bottom the previous node */
    if(my_rank==total_ranks -1) {
        MPI_Ssend(pos_of_my_high, size, MPI_CHAR, my_rank-1, 1, MPI_COMM_WORLD);
        for(int i=0;i<size;i++){
        my_received_ghostline_bot[i]=0;
        }
    } else if ((my_rank < total_ranks -1)&&(my_rank>0)) {
        MPI_Recv(my_received_ghostline_bot, size, MPI_CHAR, my_rank+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Ssend(pos_of_my_high, size, MPI_CHAR, my_rank-1, 1, MPI_COMM_WORLD);        
    } else {
        MPI_Recv(my_received_ghostline_bot, size, MPI_CHAR, my_rank+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
/*
	Task A
	------
	Implement the proper exchange of ghostlines using MPI_Send and MPI_Recv
	Rules:
		-solution should not rely on buffering of MPI_Send operations for functioning. You can use MPI_Ssend instead of MPI_Send to make sure no buffering happens.
		-try to have as much parallel exchange as possible
		-"pos_of_my_low" is to be sent to the following rank, which places it into its "my_received_ghostline_top" ("size" is length of a line)
		-other pairing analogous
	Notes:
		-adjacent ranks need to call the MPI_Send / MPI_Recv in a different order for successful (and safe) communication
		-the topmost block (rank 0) creates an empty recieved line from top, the lowest (total_ranks-1) does likewise for its bottom.
	Solution:
		-Blocks should fall through to the bottom without changing in shape (try having them fall up, too)
*/

};



void ExchangeGhostlinesNonblocking(int my_rank,int total_ranks,int size, char* my_received_ghostline_top,char* my_received_ghostline_bot,char* pos_of_my_low,char* pos_of_my_high){
    MPI_Request myRequest;
    
    /* Exchange ghost line bottom of one node to ghost line top of  the next node */
    if(my_rank==0) {
        MPI_Isend(pos_of_my_low, size, MPI_CHAR, my_rank+1, 1, MPI_COMM_WORLD, &myRequest);
        MPI_Wait(&myRequest, MPI_STATUS_IGNORE);

        for(int i=0;i<size;i++){
        my_received_ghostline_top[i]=0;
        }
    } else if (my_rank < total_ranks -1) {
        MPI_Irecv(my_received_ghostline_top, size, MPI_CHAR, my_rank-1, 1, MPI_COMM_WORLD, &myRequest);
        MPI_Wait(&myRequest, MPI_STATUS_IGNORE);

        MPI_Isend(pos_of_my_low, size, MPI_CHAR, my_rank+1, 1, MPI_COMM_WORLD, &myRequest);
        MPI_Wait(&myRequest, MPI_STATUS_IGNORE);        

    } else {
        MPI_Irecv(my_received_ghostline_top, size, MPI_CHAR, my_rank-1, 1, MPI_COMM_WORLD, &myRequest);
        MPI_Wait(&myRequest, MPI_STATUS_IGNORE);    
    }
    
    
    /* Exchange ghost line top of one node to ghost line bottom the previous node */
    if(my_rank==total_ranks -1) {
        MPI_Isend(pos_of_my_high, size, MPI_CHAR, my_rank-1, 1, MPI_COMM_WORLD, &myRequest);
        MPI_Wait(&myRequest, MPI_STATUS_IGNORE);

        for(int i=0;i<size;i++){
        my_received_ghostline_bot[i]=0;
        }
    } else if ((my_rank < total_ranks -1)&&(my_rank>0)) {
        MPI_Irecv(my_received_ghostline_bot, size, MPI_CHAR, my_rank+1, 1, MPI_COMM_WORLD, &myRequest);
        MPI_Wait(&myRequest, MPI_STATUS_IGNORE);

        MPI_Isend(pos_of_my_high, size, MPI_CHAR, my_rank-1, 1, MPI_COMM_WORLD, &myRequest);        
        MPI_Wait(&myRequest, MPI_STATUS_IGNORE);

    } else {
        MPI_Irecv(my_received_ghostline_bot, size, MPI_CHAR, my_rank+1, 1, MPI_COMM_WORLD, &myRequest);
        MPI_Wait(&myRequest, MPI_STATUS_IGNORE);

    }

/*
	Task C
	------
	Implement the proper exchange of ghostlines using the nonbocking variants MPI_Isend and MPI_Irecv
	Rules:
		-simplify the code as much as possible under the nonblocking communication model
		-MPI_Wait at the end makes sure a communication is finished before progressing (you need the MPI_Request id's to check for)
	Notes:
		-Order of the operations is no longer important
		-What are the constraints for safe usage of nonblocking communications. Why are they fulfilled here?
	Solution:
		-Behaviour is identical to the behaviour when ExchangeGhostlines(...) is used
		-Code is simpler
*/


};



void Output(int my_rank, int total_ranks, FILE* outputfile, char* mat_start, int sizeX, int sizeY, int step){
//SizeY includes the two lines for ghost elements
 	static unsigned char white[3];
	white[0]=255; white[1]=255; white[2]=255;

 	static unsigned char black[3];
	black[0]=0; black[1]=0; black[2]=0;

	if(my_rank == 0) {

		//PPM output ---- file header
		char filename[100];
        if ((step>=0) && (step<=9)) {
            sprintf(filename,"ConwayOutput_Step0%d.ppm",step);
        } else if (step==-1) {
            sprintf(filename,"ConwayOutput_Step_%d.ppm",step);
        } else {
            sprintf(filename,"ConwayOutput_Step%d.ppm",step);
        }
		//sprintf(filename,"ConwayOutput_Step%d.ppm",step);
		FILE *fp = fopen(filename, "wb"); /* b - binary mode */
		(void) fprintf(fp, "P6\n%d %d\n255\n", sizeX, (sizeY-2)*total_ranks - 1);

		fprintf(outputfile, "Step %d: \n", step);
		char* subbuff = malloc((sizeX + 2)*sizeof(char));

		for(int iy=1;iy<sizeY - 1;iy++){
			memcpy( subbuff,  &mat_start[iy * sizeX], sizeX );

			for(int ib = 0; ib < sizeX; ib++){
				if(subbuff[ib]==0){
					subbuff[ib]='-';
					(void) fwrite(black, 1, 3, fp);
				}
				else {
					subbuff[ib]='O';
					(void) fwrite(white, 1, 3, fp);
				};
			}

			subbuff[sizeX] = '\n';
			subbuff[sizeX + 1] = '\0';
            // if (iy== sizeY-3) {
                // subbuff[sizeX-1] = '\n';
            // }
			fprintf(outputfile, "%s", subbuff );

		}

		char* recvbuff = malloc( (sizeX*(sizeY))*sizeof(char) );
		for(int proc = 1; proc < total_ranks; proc++){
			MPI_Recv(recvbuff,sizeX * (sizeY-2),MPI_CHAR,proc,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			for(int line=0;line<sizeY-2;line++){
				memcpy( subbuff,  &recvbuff[line * sizeX], sizeX );
				for(int ib = 0; ib < sizeX; ib++){

					if(subbuff[ib]==0){
						subbuff[ib]='-';
						fwrite(black, 1, 3, fp);
					}
					else {
						subbuff[ib]='O';
						fwrite(white, 1, 3, fp);
					};
				}

				subbuff[sizeX] = '\n';
				subbuff[sizeX + 1] = '\0';
				fprintf(outputfile, "%s", subbuff );
			}
		}

		//PPM output
		fclose(fp);

		free(subbuff);
	} else {
		MPI_Send(mat_start + 1*sizeX,sizeX * (sizeY-2),MPI_CHAR,0,4,MPI_COMM_WORLD);
	}
}
void Printmatrix(char* mat_start, int sizeX, int sizeY){
    char* subbuff = malloc( (sizeX*(sizeY))*sizeof(char) );
 
    for(int iy=0;iy<sizeY;iy++){
        memcpy( subbuff,  &mat_start[iy * sizeX], sizeX );
        for(int ib = 0; ib < sizeX; ib++){
            if(subbuff[ib]==0){
                subbuff[ib]='-';
            }
            else {
                subbuff[ib]='O';
            };
        }

        subbuff[sizeX] = '\n';
        subbuff[sizeX + 1] = '\0';
        printf("%s", subbuff );
    }
}
