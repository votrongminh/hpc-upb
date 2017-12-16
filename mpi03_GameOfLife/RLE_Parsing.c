#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <math.h>

#include "RLE_Parsing.h"

//totalSizeX = 2*padding + sizeX of pattern 
int parseRLE(char* input,char* output,int padding, int sizeXofPattern){ 
	//Every line = padding , translation , padding
	int outpos = 0; 
	for(outpos=0;outpos<padding;outpos++){
		output[outpos] = 0;
	}

	int inpos = 0;
	int mult = 0;
	int sizeRem = sizeXofPattern;
	char symb = ' ';
	while(1 == 1){
		symb = input[inpos];		
		if(symb>='0' && symb<='9'){
			mult*=10;
			mult += symb - '0';
		} else if(symb == 'o'){
			if(mult==0){mult=1;}
			//printf("%d x Live\n",mult);

			for(int jj=0;jj<mult;jj++){
				output[outpos] = 1;
				outpos++;
			}
			sizeRem -= mult;
			mult = 0;
		} else if(symb == 'b'){
			if(mult==0){mult=1;}
			//printf("%d x Dead\n",mult);

			for(int jj=0;jj<mult;jj++){
				output[outpos] = 0;
				outpos++;
			}		
			sizeRem -= mult;
			mult = 0;
		} else if(symb == '$' || symb == '!') {
			mult = 0;	
			if(sizeRem > 0){ //If we aren't at the end of a line, fill up with dead cells. Also add the padding for the end of the line
				for(int ii=0;ii<sizeRem + padding; ii++){
					output[outpos] = 0;
					outpos++;
				}				
			}

			if(input[inpos+1] != '\0'){	//If another line is going to be started afterward, add padding for it. Don't add extra padding on the last of our lines, since it is not going to be followed by anything.
				for(int ii=0;ii<padding; ii++){
					output[outpos] = 0;
					outpos++;
				}
			}

			sizeRem = sizeXofPattern;	
		} else if( symb == '\0'){
			break;
		}


		inpos++;
	}


	for(int i = 0;i<padding;i++){
		outpos++;
		input[outpos] = 0;
	}

	return 0;
}


void loadFile(char* file, char** output1, char** output2, int* sizeX, int* mySizeY){
	const int MSG_SIZE_MAX = 200000;

	int my_rank,total_ranks;
	MPI_Comm_size(MPI_COMM_WORLD,&total_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	int parsedX, parsedY = -1;
	int amount_per_process = -1;

	if(my_rank == 0){
		FILE *inputfile = fopen(file, "r");
		if (inputfile == NULL)
		{
			printf("Error opening file %s!\n",file);
			exit(1);
			//Do something thats reasonable
		} 


		char* buff = malloc(MSG_SIZE_MAX*sizeof(char));
		do{
			fgets( buff, 200, inputfile );
			printf("Read this: %s\n",buff);
		}while(buff[0] == '#');
			

		parseXY(buff,&parsedX,&parsedY);	
		printf("parsed size xy: %d %d \n",parsedX, parsedY);

		if(parsedY < total_ranks){
			printf("You have more ranks than there are rows. This can't end well, aborting.\n");
			MPI_Abort(MPI_COMM_WORLD,0);
		}
		
		MPI_Bcast(&parsedX,1,MPI_INT,0,MPI_COMM_WORLD);	
		MPI_Bcast(&parsedY,1,MPI_INT,0,MPI_COMM_WORLD);	
		amount_per_process = ceil(1.0f * parsedY / total_ranks);
		
		*output1 = malloc(sizeof(char)*(amount_per_process+2)*parsedX);// size y increased by 2 for ghostline at 0 and last
		*output2 = malloc(sizeof(char)*(amount_per_process+2)*parsedX);// size y increased by 2 for ghostline at 0 and last


		char* sendbuffer = malloc(MSG_SIZE_MAX * sizeof(char));

		int len=0;

		//For ourselves
		for(int nr_lines_to_current_process = 0; nr_lines_to_current_process < amount_per_process; nr_lines_to_current_process++){
			parseNext(inputfile,buff,&len);
			//printf("Parsed for 0: %s\n",buff);
			parseRLE(buff,*output1 + (nr_lines_to_current_process+1)* parsedX, 0,parsedX); 					

		}

		printf("%d lines / process",amount_per_process);

		//For the others
		for(int proz_nr = 1; proz_nr < total_ranks; proz_nr++){

			int len = 0;
			int msg_len = 0;
			sprintf(sendbuffer,"");

			printf("Building message to to %d \n",proz_nr);

			for(int nr_lines_to_current_process = 0; nr_lines_to_current_process < amount_per_process; nr_lines_to_current_process++){
				//Fine until here
				parseNext(inputfile,buff,&len);

				
				if(msg_len + len > MSG_SIZE_MAX){
					printf("Error: Maximum message length is insufficient. TO DO: Split up into smaller messages");
					break;
				}
				
				

				//printf("...");				

				//printf("Parsed for other: %s   with len: %d \n",buff,len);

				sprintf(sendbuffer,"%s%s",sendbuffer,buff);
	
				msg_len += len;

				//! means file is done
				if(buff[len - 1] == '!'){
					printf("End of file reached!\n");
					break;
				}
			}


			if(proz_nr == total_ranks-1 && parsedY % total_ranks != 0){
				int extra_padding = total_ranks - parsedY % total_ranks;
				printf("Amount not divisible by #ranks, padding with %d additional lines of 0es\n",extra_padding);

				for(int i = 0;i<extra_padding;i++){
					sprintf(sendbuffer,"%s%s",sendbuffer,"$");									
				}

			}


			//printf("Sending to %d: %s \n",proz_nr,sendbuffer);
			MPI_Ssend(sendbuffer,strlen(sendbuffer),MPI_CHAR,proz_nr,100,MPI_COMM_WORLD);	
			//printf("Post send to %d (size %d)\n",proz_nr,MSG_SIZE_MAX);
		}


		free(sendbuffer);
		free(buff);

		//fclose(inputfile); //Hangs for some reason

		printf("Process 0 done\n");

	} else {

		MPI_Bcast(&parsedX,1,MPI_INT,0,MPI_COMM_WORLD);	
		MPI_Bcast(&parsedY,1,MPI_INT,0,MPI_COMM_WORLD);		

		//Allocate storage
		amount_per_process = ceil(1.0f * parsedY / total_ranks);

		*output1 = malloc(sizeof(char)*(amount_per_process + 2)*parsedX);// size y increased by 2 for ghostline at 0 and last
		*output2 = malloc(sizeof(char)*(amount_per_process + 2)*parsedX);// size y increased by 2 for ghostline at 0 and last


		//printf("Process %d waiting for message of size %d \n",my_rank,MSG_SIZE_MAX);
		char* rebuffer = malloc(MSG_SIZE_MAX * sizeof(char));
		MPI_Status stata;

		MPI_Recv(rebuffer,MSG_SIZE_MAX,MPI_CHAR,0,100,MPI_COMM_WORLD,&stata);

		parseRLE(rebuffer,*output1 + parsedX, 0,parsedX); 		
		
		free(rebuffer);
		printf("Process %d initialized\n",my_rank);
	}

	*sizeX = parsedX;
	*mySizeY = (amount_per_process) + 2;


	MPI_Barrier( MPI_COMM_WORLD );


}

void parseXY(char* input, int*x,int*y){
	int pos = 0;
	int reading = 0;
	*x = 0;
	*y = 0;
	while(input[pos]!='\0' && input[pos]!='\n'){
		char c = input[pos];
		if(c >= '0' && c<= '9'){
			if(reading == 0){
				*x *= 10;
				*x += c - '0';
			} else if (reading == 1){
				*y *= 10;
				*y += c - '0';
			}
		}
		if(c == 'y'){
			
			reading = 1;

		}
		if( c== ',' && reading==1){
			return;
		}
		pos ++;
	}
}


void parseNext(FILE* file, char* buffer ,int * size){
	int pos_in_buffer = 0;
	int ch = 0;

	do{
		ch = fgetc( file );
 		//printf("in inner 2, char is %c\n",ch);	
		if(ch != '\n' && ch != EOF){
			buffer[pos_in_buffer] = (char)ch;
			pos_in_buffer++;
		}		


	}while(ch != EOF && ch !='$' && ch != '!');

	buffer[pos_in_buffer] = '\0';
	//printf("func res: %s \n",buffer);
	*size = pos_in_buffer;
}