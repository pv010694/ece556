// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"

int readBenchmark(const char *fileName, routingInst *rst){
  /*********** TO BE FILLED BY YOU **********/  

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
  

