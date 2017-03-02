// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"
#include <stdio.h>
#include <fstream.h>

int readBenchmark(const char *fileName, routingInst *rst){
  char grid [128], capacity[128],num_nets[128];
  FILE *fp;
  fp=fopen(fileName, "r");
  fgets(grid, 128, fp);
  fgets(capacity,128,fp);
  fgets(num_nets,128,fp);
  //printf("%s\n",grid);
  //printf("%s\n",capacity);
  //printf("%s\n",num_nets);
  
  const char s[2]=" ";
  char *token,*token_new;
  int count=0;
  //Parsing grid values
  token = strtok(grid,s);
  while(token !=NULL)
  {
    if(count==1)
    {	
	rst->gx = atoi(token);
    	
    }
    else if(count==2)
	rst->gy = atoi(token);
     count++;
     token=strtok(NULL,s);
  }
  //Parsing capacity
  count=0;
  token = strtok(capacity,s);
  while(token !=NULL)
  {
     if(count==1)
     	rst->cap = atoi(token);
     count++;
     token=strtok(NULL,s);
  }
  //Parsing num-nets
  count=0;
  token = strtok(num_nets,s);
  while(token !=NULL)
  {
     if (count==2)
      rst->numNets = atoi(token);
     count++;
     token=strtok(NULL,s);
  }
 
  //printf("gx:%d\n",rst->gx); 
  //printf("gy:%d\n",rst->gy); 
  //printf("cap:%d\n",rst->cap);  
  //printf("numNets:%d\n",rst->numNets); 
  rst->nets = (net*)malloc(rst->numNets*sizeof(net));

  char buf[128],buf_cpy[128],buf_cpy2[128];
  int index=-1; int pin_index;
  while(fgets(buf,128,fp)!=NULL)
  {
 	count=0;
	strcpy(buf_cpy,buf);//copy cuz parsing using strtok is destructive
        strcpy(buf_cpy2,buf_cpy);
   	
	token_new = strtok(buf_cpy2,s);
	if(strtok(NULL,s)==NULL)	//detecting congestion info at the end of the file
		break;
	
	if(buf[0] == 'n')
	{
		//printf("HERE\n");
		count=0;
		pin_index=0;
		index++; //Think of a better way to find index
		token=strtok(buf_cpy,s);
		while(token !=NULL)
 		{
    		if(count==0)
    		{			
 			//(rst->nets+index)=(net*)malloc(sizeof(net));
			//rst->nets = (net*)malloc(sizeof(net)*num_nets);
			
			rst->nets[index].id = index;//Put ID of the net here;
			//printf("HERE\n");
			//printf("rst->nets[%d].id = %d\n",index,rst->nets[index].id);
		}
		else if(count==1)
		{
			rst->nets[index].numPins = atoi(token);
			//printf("rst->nets[%d].numPins = %d\n",index,rst->nets[index].numPins);
                }
			count++;
			token=strtok(NULL,s);
		}

			rst->nets[index].pins = (point*)malloc(sizeof(point)*rst->nets[index].numPins);
	}
	else
	{	token = strtok(buf_cpy,s);
  		while(token !=NULL)
 		{
    		if(count==0)
    		{	
			rst->nets[index].pins[pin_index].x = atoi(token);
			//printf("rst->nets[%d].pins[%d].x = %d\n",index,pin_index,rst->nets[index].pins[pin_index].x);
    			count++;
    		}
   		 else
		{	
			rst->nets[index].pins[pin_index].y = atoi(token);
			//printf("rst->nets[%d].pins[%d].y = %d\n",index,pin_index,rst->nets[index].pins[pin_index].y);
		}
     			token=strtok(NULL,s);
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
	
	int num_segments = (cur_net.numPins/2) + 1;
	rst->nets[k].nroute.numSegs = num_segments;
	rst->nets[k].nroute.segments = new segment[num_segments]; 

	segment *cur_seg_array = rst->nets[k].nroute.segments;
	
	int indx=0;

	for(int p=0;p<cur_net.numPins-1;p++,indx++) {

		cur_seg_array[indx].p1 = pins[p];
		cur_seg_array[indx].p2 = pins[p+1];

		if ( pins[p].y == pins[p+1].y ) {

		/*Horizontal flat segment*/

		int numEdges = abs(pins[p].x - pins[p+1].x);

		cur_seg_array[indx].numEdges = numEdges;

		cur_seg_array[indx].edges    = new int[numEdges];
			
		int first_edge_indx	     = ( (pins[p].y)*(rst->gx - 1) + pins[p].x );
	
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
			
		int first_edge_indx	     = ( (pins[p].x)*(rst->gy - 1) + pins[p].y + rst->gy * (rst->gx - 1) );

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

		  cur_seg_array[indx].edges    = new int[numEdges];

		   // First horizontal flat segment

		   int h_numEdges	       = abs(p_int.x - pins[p].x);
		   int first_edge_indx         = ( (pins[p].y)*(rst->gx-1) + pins[p].x);
		   
		   cur_seg_array[indx].edges[0] = first_edge_indx;
		
		    for(int e=1; e<h_numEdges; e++) {
			
			cur_seg_array[indx].edges[e] = ++first_edge_indx;
		

		    }

		  //Then vertical flat segment

		  // int v_numEdges		     = abs(pins[p+1].y - p_int.y);
			
		   int first_edge_indx_v	          = ( (p_int.x)*(rst->gy-1) + p_int.y + rst->gy*(rst->gx - 1) );
		  
		   cur_seg_array[indx].edges[h_numEdges] = first_edge_indx_v;
		
		   for(int e=1+h_numEdges; e< numEdges; e++) {
			
			cur_seg_array[indx].edges[e] = ++first_edge_indx_v;
		

		    }

			
		}
		
	
	}
	
  } 


  return 1;
}

int writeOutput(const char *outRouteFile, routingInst *rst){
  /*********** TO BE FILLED BY YOU **********/

  return 1;
}


int release(routingInst *rst){
  /*********** TO BE FILLED BY YOU **********/

  return 1;
}
  

