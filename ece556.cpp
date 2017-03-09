// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"

int readBenchmark(const char *fileName, routingInst *rst){
  char grid [128], capacity[128],num_nets[128];
  FILE *fp;
  fp=fopen(fileName, "r");
  fgets(grid, 128, fp);
  fgets(capacity,128,fp);
  fgets(num_nets,128,fp);

  char *token,*token_new;
  int count=0;
  //Parsing grid values
  token = strtok(grid," \t\n");
  while(token !=NULL)
  {
    if(count==1)
    {	
	rst->gx = atoi(token);
    	
    }
    else if(count==2)
	rst->gy = atoi(token);
     count++;
     token=strtok(NULL," \t\n");
  }
  //Parsing capacity
  count=0;
  token = strtok(capacity," \t\n");
  while(token !=NULL)
  {
     if(count==1)
     	rst->cap = atoi(token);
     count++;
     token=strtok(NULL," \t\n");
  }
  //Parsing num-nets
  count=0;
  token = strtok(num_nets," \t\n");
  while(token !=NULL)
  {
     if (count==2)
      rst->numNets = atoi(token);
     count++;
     token=strtok(NULL," \t\n");
  }
 
  rst->nets = (net*)malloc(rst->numNets*sizeof(net));

  char buf[128],buf_cpy[128],buf_cpy2[128];
  int index=-1; int pin_index;
  while(fgets(buf,128,fp)!=NULL)
  {
 	count=0;
	strcpy(buf_cpy,buf);//copy cuz parsing using strtok is destructive
        strcpy(buf_cpy2,buf_cpy);
   	
	token_new = strtok(buf_cpy2," \t\n");
	if(strtok(NULL," \t\n")==NULL)	//detecting congestion info at the end of the file
	break; 
	
	if(buf[0] == 'n')
	{
		count=0;
		pin_index=0;
		index++; 
		token=strtok(buf_cpy," \t\n");
		while(token !=NULL)
 		{
    		if(count==0)
    		{			
 						
			rst->nets[index].id = index;//Put ID of the net here;
		}
		else if(count==1)
		{
			rst->nets[index].numPins = atoi(token);
	
                }
			count++;
			token=strtok(NULL," \t\n");
		}

			rst->nets[index].pins = (point*)malloc(sizeof(point)*rst->nets[index].numPins);
	}
	else
	{	token = strtok(buf_cpy," \t\n");
  		while(token !=NULL)
 		{
    		if(count==0)
    		{	
			rst->nets[index].pins[pin_index].x = atoi(token);
    			count++;
    		}
   		 else
		{	
			rst->nets[index].pins[pin_index].y = atoi(token);
		}
     			token=strtok(NULL," \t\n");
  		}
		pin_index++;
       }
   }
  return 1;
}

int solveRouting(routingInst *rst){
  /*********** TO BE FILLED BY YOU **********/

 net *net_array = rst->nets;

  for(int k=0;k<rst->numNets;k++) {

	net cur_net = net_array[k];
	point *pins = cur_net.pins;
	
	int num_segments = cur_net.numPins - 1;

	rst->nets[k].nroute.numSegs  = num_segments;
	rst->nets[k].nroute.segments = new segment[num_segments]; 

	segment *cur_seg_array = rst->nets[k].nroute.segments;
	
	int p=0;

	for(int indx=0;indx<num_segments;indx++) {

		cur_seg_array[indx].p1 = pins[p];
		cur_seg_array[indx].p2 = pins[p+1];

		if ( pins[p].y == pins[p+1].y ) {

		/*Horizontal flat segment*/

		int numEdges = abs(pins[p].x - pins[p+1].x);

		cur_seg_array[indx].numEdges = numEdges;

		cur_seg_array[indx].edges    = new int[numEdges];
		
		int source_x = pins[p].x;

		if ( pins[p].x > pins[p+1].x ){ 
		
			source_x = pins[p+1].x;
		}	

		int first_edge_indx	     = ( (pins[p].y)*(rst->gx - 1) + source_x );
	
         	cur_seg_array[indx].edges[0] = first_edge_indx;
		
		for(int e=1; e< numEdges; e++) {
			
			cur_seg_array[indx].edges[e] = ++first_edge_indx;
		

		    }


		}


		else if ( pins[p].x == pins[p+1].x) {

		/*Vertical flat segment*/

		int numEdges		     = abs(pins[p].y - pins[p+1].y);

		cur_seg_array[indx].numEdges = numEdges;

		cur_seg_array[indx].edges    = new int[numEdges];
			
		int source_y = pins[p].y;

		if ( pins[p].y > pins[p+1].y ){ 
			
			source_y = pins[p+1].y;

		}

	
		int first_edge_indx	     = ( (pins[p].x)*(rst->gy - 1) + source_y + rst->gy * (rst->gx - 1) );

		cur_seg_array[indx].edges[0] = first_edge_indx;
		
		for(int e=1; e< numEdges; e++) {
			
			cur_seg_array[indx].edges[e] = ++first_edge_indx;
		

		    }

		}


		else {
		
		  /*L-shaped segment*/

		  point p_int;
		  p_int.y = pins[p].y;
		  p_int.x = pins[p+1].x;
		
		  int numEdges = abs(pins[p].x - p_int.x) + abs(pins[p+1].y - p_int.y);

		  
		  cur_seg_array[indx].numEdges = numEdges;

		  cur_seg_array[indx].edges    = new int[numEdges];

		   // First horizontal flat segment

		   int h_numEdges	       = abs(p_int.x - pins[p].x);

		
		int source_x = pins[p].x;

		if ( pins[p].x > p_int.x ){ 
			
			source_x = p_int.x;

		}


		   int first_edge_indx         = ( (pins[p].y)*(rst->gx-1) + source_x );
		   
		   cur_seg_array[indx].edges[0] = first_edge_indx;
		
		    for(int e=1; e<h_numEdges; e++) {
			
			cur_seg_array[indx].edges[e] = ++first_edge_indx;
		

		    }

		  //Then vertical flat segment

		  // int v_numEdges		     = abs(pins[p+1].y - p_int.y);
		
		  	
		int source_y = pins[p+1].y;

		if ( pins[p+1].y > p_int.y ){ 
			
			source_y = p_int.y;

		}
		   
		int first_edge_indx_v	          = ( (p_int.x)*(rst->gy-1) + source_y + rst->gy*(rst->gx - 1) );
		  
		   cur_seg_array[indx].edges[h_numEdges] = first_edge_indx_v;
		
		   for(int e=1+h_numEdges; e< numEdges; e++) {
			
			cur_seg_array[indx].edges[e] = ++first_edge_indx_v;
		

		    }

			
		} //end of L-shaped else
		
	    p++;	
	}
	
  } 


  return 1;
}

int writeOutput(const char *outRouteFile, routingInst *rst){
  /*********** TO BE FILLED BY YOU **********/

  FILE *fp;
  fp=fopen(outRouteFile, "w");

  net *net_array = rst->nets;

  for(int k=0;k<rst->numNets;k++) {

	segment *cur_seg_array = net_array[k].nroute.segments;
	
	fprintf(fp,"n%d\n",k);
 		
	for(int j=0;j<net_array[k].nroute.numSegs;j++) {

		int *edgearray = cur_seg_array[j].edges;
		int numEdges = cur_seg_array[j].numEdges;

		for (int l=0;l<numEdges;l++) {

			int edgeid = edgearray[l];

			if  ( edgeid < ( rst->gy*(rst->gx-1) ) ) { //then horizontal edge
			
				int height = edgeid/( rst->gx - 1);
				int source_x = edgeid - height*(rst->gx - 1);
				int dest_x = source_x + 1;
				fprintf(fp,"(%d,%d)-(%d,%d)\n",source_x,height,dest_x,height);

			}
				

			else 				  {  // then vertical edge
	 			
				int offset   =  (edgeid - rst->gy*(rst->gx-1) );
				int width    =  offset/(rst->gy - 1);
				int source_y =  offset - width*( rst->gy - 1);
				fprintf(fp,"(%d,%d)-(%d,%d)\n",width,source_y,width,source_y+1);

			

			}
	 


		}

	}

        	
	fprintf(fp,"!\n"); 		

  }

  return 1;
}


int release(routingInst *rst){
  /*********** TO BE FILLED BY YOU **********/

  net *net_array = rst->nets;

  for(int k=0;k<rst->numNets;k++) {

	segment *cur_seg_array = net_array[k].nroute.segments;
	
	for(int j=0;j<net_array[k].nroute.numSegs;j++) {

		int *edgearray = cur_seg_array[j].edges;
  
		delete [] edgearray;
	}

	delete [] cur_seg_array;

	point *pin_array = net_array[k].pins;
	delete [] pin_array;



  }

  delete [] net_array;
  delete rst;


   return 1;
}
  

