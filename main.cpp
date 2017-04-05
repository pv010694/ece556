// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"
#include <unistd.h>

int main(int argc, char **argv)
{

 	if(argc!=5){
 		printf("Usage : ./ROUTE.exe -d=<0/1> -n=<0/1> <input_benchmark_name> <output_file_name> \n");
 		return 1;
 	}
	int opt,d,n;
	start = time(NULL);  //store the starting time
	d=0;n=0;
	while((opt = getopt (argc, argv, "d:n:"))!=-1)
	{
		//printf("d %d\n",atoi(&optarg[1]));

		switch(opt)
		{
		case 'd':	{  d=atoi(&optarg[1]);
				   //printf("d %d\n",atoi(&optarg[1]));
				   break;}
		case 'n':	{  n=atoi(&optarg[1]);
				   //printf("n %d\n",atoi(&optarg[1]));
				   break;}
		}

	}
		//printf("d %d\n",d);
		//printf("n %d\n",n);

 	int status;
	char *inputFileName = argv[3];
 	char *outputFileName = argv[4];
	
 	/// create a new routing instance
 	routingInst *rst = new routingInst;
	
 	/// read benchmark
 	status = readBenchmark(inputFileName, rst);
 	if(status==0){
 		printf("ERROR: reading input file \n");
 		return 1;
 	}
	
 	/// run actual routing
 	status = solveRouting(rst,d,n);
 	if(status==0){
 		printf("ERROR: running routing \n");
 		release(rst);
 		return 1;
 	}
	
 	/// write the result
 	status = writeOutput(outputFileName, rst);
 	if(status==0){
 		printf("ERROR: writing the result \n");
 		release(rst);
 		return 1;
 	}

 	release(rst);
 	printf("\nDONE!\n");	
 	return 0;
}
