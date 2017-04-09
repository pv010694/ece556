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

  rst->numEdges = rst->gy*(rst->gx -1) + rst->gx*(rst->gy -1);
   
  rst->edgeCaps  = new int[ rst->numEdges ];
  rst->edgeUtils = new int[ rst->numEdges ];

  /* Filling edge utilization array by zero */

  memset( rst->edgeUtils, 0 , rst->numEdges );

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

  /* Filling default capacity for each edge */

  memset( rst->edgeCaps, rst->cap , rst->numEdges );

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
 
  rst->nets = new net[ rst->numNets ];

//(net*)malloc(rst->numNets*sizeof(net));

  char buf[128],buf_cpy[128],buf_cpy2[128];
  int index=-1; int pin_index;
  while(fgets(buf,128,fp)!=NULL)
  {
 	count=0;
	strcpy(buf_cpy,buf);//copy cuz parsing using strtok is destructive
        strcpy(buf_cpy2,buf_cpy);
   	
	token_new = strtok(buf_cpy2," \t\n");
	

	if(strtok(NULL," \t\n")==NULL) {	//detecting congestion info at the end of the file

	
		char *token_conges;
	
		
		while( fgets(buf,128,fp) != NULL ) {
	
			token_conges = strtok( buf," \t\n");
			
			int count_conges = 0;
			int x1=0,y1=0,x2=0,y2=0,reduced_capacity=0;
			
			while( token_conges != NULL ) {

				if( count_conges == 0 ) {
					x1 = atoi( token_conges );
				}

				if( count_conges == 1) {

					y1 = atoi(token_conges);
				}

				if( count_conges == 2) {

					x2 = atoi (token_conges);
				}

				if( count_conges == 3) {

					y2 = atoi (token_conges);
				}

				if( count_conges == 4) {

					reduced_capacity = atoi (token_conges);
				}

				count_conges+=1;
				token_conges = strtok(NULL," \t\n");
		
			}
		
			int edgeID;

//			printf("\nBlocking points: %d,%d,%d,%d,%d",x1,y1,x2,y2,reduced_capacity);

			/* Converting edge points into edgeIDs */
			
			if (x1 == x2 ) {

				int y_min;

				if( y1 < y2 ) {

					y_min = y1;
				}

				else 
					y_min = y2;

				edgeID = x1*( rst->gy -1 ) + y_min + rst->gy*( rst->gx - 1 );

			}

			else { 

				int x_min;

				if( x1 < x2 ) {

					x_min = x1;
				}

				else 
					x_min = x2;

				edgeID = y1*( rst->gx -1 ) + x_min;

			  }
			
		rst->edgeCaps [ edgeID ] = reduced_capacity;	
		
		}
		

	break;

	}
	
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

			rst->nets[index].pins = new point[ rst->nets[index].numPins ];
			//(point*)malloc(sizeof(point)*rst->nets[index].numPins);
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




int ripnreroute(routingInst *rst);


int solveRouting(routingInst *rst, int d, int n){
  /*********** TO BE FILLED BY YOU **********/

 if(n==0 && d==0)					//Part 1 solution
 {
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
	
		rst->edgeUtils[ first_edge_indx ]++;
		


         	cur_seg_array[indx].edges[0] = first_edge_indx;
		
		for(int e=1; e< numEdges; e++) {
			
			int eindx  = ++first_edge_indx;
			cur_seg_array[indx].edges[e] = eindx;
		
			rst->edgeUtils [ eindx ]++;

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

		rst->edgeUtils [ first_edge_indx ]++;
		
		for(int e=1; e< numEdges; e++) {
			
			int eindx = ++first_edge_indx;
			cur_seg_array[indx].edges[e] = eindx;
			rst->edgeUtils[ eindx ]++;

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
		   rst->edgeUtils[ first_edge_indx ]++;
			
		
		    for(int e=1; e<h_numEdges; e++) {
			
			int eindx = ++first_edge_indx;
			cur_seg_array[indx].edges[e] = eindx;
			rst->edgeUtils[ eindx ]++;
		

		    }

		  //Then vertical flat segment

		  // int v_numEdges		     = abs(pins[p+1].y - p_int.y);
		
		  	
		int source_y = pins[p+1].y;

		if ( pins[p+1].y > p_int.y ){ 
			
			source_y = p_int.y;

		}
		   
		int first_edge_indx_v	          = ( (p_int.x)*(rst->gy-1) + source_y + rst->gy*(rst->gx - 1) );
		  
		   cur_seg_array[indx].edges[h_numEdges] = first_edge_indx_v;

		   rst->edgeUtils[ first_edge_indx_v ]++;
		
		   for(int e=1+h_numEdges; e< numEdges; e++) {
			
			int eindx = ++first_edge_indx_v;
			cur_seg_array[indx].edges[e] = eindx;
			rst->edgeUtils[ eindx ]++;
		

		    }

			
		} //end of L-shaped else
		
	    p++;	
	}
	
  } 
 
 }

 if(d==1)	//Apply net decomposition
 {

  net *net_array = rst->nets;
  
  for(int k=0; k<rst->numNets;k++)
  {
	net cur_net = net_array[k]; 		//cur_net is the current net
	point *pins = cur_net.pins;		//pins is the pins of the current net

	int *flag = new int [cur_net.numPins];	
	for (int j=0; j< cur_net.numPins; j++)
		flag[j]=0;

	//Sort the pins to find the corner most one
	point corner_pin = pins[0];
	int corner_pin_index=0,nearest_pin_index=0;
	int min_dist = 2147483647;
	for(int j=1; j<cur_net.numPins; j++)
	{
		if ( (pins[j].x < corner_pin.x ) || ((pins[j].x == corner_pin.x) && (pins[j].y < corner_pin.y)))
			{ corner_pin = pins[j]; corner_pin_index = j; }
	}
	flag[corner_pin_index]=1;
	
	//Find the nearest pin to corner pin
	for(int j=0; j<cur_net.numPins; j++)
	{
		if(flag[j]!=1)
		{
			int dist = abs(corner_pin.x - pins[j].x) + abs(corner_pin.y - pins[j].y);
			if(dist < min_dist)
			{
				nearest_pin_index=j;
				min_dist = dist;
			}
		}
	}
	flag[nearest_pin_index]=1;

	//printf(" Corner pin x: %d y: %d Nearest pin x: %d y: %d\n",pins[corner_pin_index].x,pins[corner_pin_index].y,pins[nearest_pin_index].x,pins[nearest_pin_index].y);
	//break;
	
	//Connect the corner pin with the closest pin
	int num_segments = cur_net.numPins-1 ;		

	rst->nets[k].nroute.numSegs  = num_segments;
	rst->nets[k].nroute.segments = new segment[num_segments]; 

	segment *cur_seg_array = rst->nets[k].nroute.segments;
	segment *alt_seg_array = new segment[num_segments];
	int indx=0;
	int L_FLAG=0;
        point p_int, p_int2;
	point oldp_int, oldp_int2;
        int old_numEdges;	
  	if( pins[corner_pin_index].y == pins[nearest_pin_index].y )
	{
		/*Horizontal flat segment*/
		// point p_int;
		p_int.x = pins[nearest_pin_index].x;
		p_int.y = pins[nearest_pin_index].y;
		oldp_int = p_int;
		int numEdges = abs(pins[corner_pin_index].x - pins[nearest_pin_index].x);
		//indx=0;
		cur_seg_array[indx].numEdges = numEdges;
		
		cur_seg_array[indx].edges    = new int[numEdges];
		
		int source_x = pins[corner_pin_index].x;

		if ( pins[corner_pin_index].x > pins[nearest_pin_index].x )
			source_x = pins[nearest_pin_index].x;

		int first_edge_indx	     = ( (pins[corner_pin_index].y)*(rst->gx - 1) + source_x );
	
         	cur_seg_array[indx].edges[0] = first_edge_indx;

		rst->edgeUtils[ first_edge_indx ]++;	
	
		for(int e=1; e< numEdges; e++) {
			int eindx = ++first_edge_indx;
			cur_seg_array[indx].edges[e] = eindx;
			rst->edgeUtils[ eindx ]++;
		    }
		old_numEdges = numEdges;
	}	

	else if ( pins[corner_pin_index].x == pins[nearest_pin_index].x) {
		//point p_int;
		p_int.x = pins[nearest_pin_index].x;
		p_int.y = pins[nearest_pin_index].y;
		oldp_int = p_int;
		/*Vertical flat segment*/

		int numEdges		     = abs(pins[corner_pin_index].y - pins[nearest_pin_index].y);
		//indx=0;
		cur_seg_array[indx].numEdges = numEdges;

		cur_seg_array[indx].edges    = new int[numEdges];
			
		int source_y = pins[corner_pin_index].y;
		if ( pins[corner_pin_index].y > pins[nearest_pin_index].y )
			source_y = pins[nearest_pin_index].y;
	
		int first_edge_indx	     = ( (pins[corner_pin_index].x)*(rst->gy - 1) + source_y + rst->gy * (rst->gx - 1) );

		cur_seg_array[indx].edges[0] = first_edge_indx;
		
		rst->edgeUtils[ first_edge_indx ]++;
	
		for(int e=1; e< numEdges; e++) {

			int eindx = ++first_edge_indx;
			cur_seg_array[indx].edges[e] = eindx;
			rst->edgeUtils[ eindx ]++;	
		    }
		old_numEdges = numEdges;
		}

	else   {
		  //Routing both alternative paths between 2 points!
		  L_FLAG=1;
		  //point p_int, p_int2;
		  p_int.y = pins[corner_pin_index].y;
		  p_int.x = pins[nearest_pin_index].x;
		
		  p_int2.y = pins[nearest_pin_index].y;
		  p_int2.x = pins[corner_pin_index].x;
		  
		  oldp_int = p_int;
		  oldp_int2 = p_int2;

		  int numEdges = abs(pins[corner_pin_index].x - p_int.x) + abs(pins[nearest_pin_index].y - p_int.y);
		  cur_seg_array[indx].numEdges = numEdges;
		  cur_seg_array[indx].edges    = new int[numEdges];
		  
		  alt_seg_array[indx].numEdges = numEdges;
		  alt_seg_array[indx].edges   = new int[numEdges];
		   // First horizontal flat segment for cur_seg_array and vertical flat segment for alt_seg_array
		   int h_numEdges	       = abs(p_int.x - pins[corner_pin_index].x);
		   int source_x = pins[corner_pin_index].x;

		   int v_numEdges   =  abs(p_int2.y - pins[corner_pin_index].y);
		   int source_y = pins[corner_pin_index].y;

		if ( pins[corner_pin_index].x > p_int.x )	
			source_x = p_int.x;

		   int first_edge_indx         = ( (pins[corner_pin_index].y)*(rst->gx-1) + source_x );
		   
		   cur_seg_array[indx].edges[0] = first_edge_indx;
		
		   rst->edgeUtils[ first_edge_indx ]++;	

		    for(int e=1; e<h_numEdges; e++) {
		
			cur_seg_array[indx].edges[e] = ++first_edge_indx;
			rst->edgeUtils[ first_edge_indx ]++;	
		    }

		if ( pins[corner_pin_index].y > p_int2.y )	
			source_y = p_int2.y;

		   int first_edge_indx_v         = ( (pins[corner_pin_index].x)*(rst->gy-1) + source_y + rst->gy*(rst->gx - 1) );
		   alt_seg_array[indx].edges[0] = first_edge_indx_v;
		     
		   rst->edgeUtils[ first_edge_indx_v ]++;
	
		   for(int e=1; e<v_numEdges; e++) {

			alt_seg_array[indx].edges[e] = ++first_edge_indx_v;
			
			rst->edgeUtils[ first_edge_indx_v ]++;	
		    }
		 	
		  //Then vertical flat segment for cur_seg_array and horizonal flat segment for alt_seg_array
		int source_y2 = pins[nearest_pin_index].y;

		if ( pins[nearest_pin_index].y > p_int.y ){ 	
			source_y2 = p_int.y;
		}
		   
		int first_edge_indx_v2	          = ( (p_int.x)*(rst->gy-1) + source_y2 + rst->gy*(rst->gx - 1) );
		
		 cur_seg_array[indx].edges[h_numEdges] = first_edge_indx_v2;

		 rst->edgeUtils[ first_edge_indx_v2 ]++;	

		   for(int e=1+h_numEdges; e< numEdges; e++) {
			cur_seg_array[indx].edges[e] = ++first_edge_indx_v2;
			
			rst->edgeUtils[ first_edge_indx_v2 ]++;	
		    }

		int source_x2 = pins[nearest_pin_index].x;

		if ( pins[nearest_pin_index].x > p_int2.x ){ 	
			source_x2 = p_int2.x;
		}
		   
		int first_edge_indx2	          = ( (p_int2.y)*(rst->gx-1) + source_x2 );
		 alt_seg_array[indx].edges[v_numEdges] = first_edge_indx2;
		
		rst->edgeUtils[ first_edge_indx2 ]++;	

		   for(int e=1+v_numEdges; e< numEdges; e++) {
			alt_seg_array[indx].edges[e] = ++first_edge_indx2;
			
			rst->edgeUtils[ first_edge_indx2 ]++;	
		    }

		old_numEdges = numEdges;
			
		} //end of L-shaped else

    //Applying the same heuristic on top to all other points
    for(int indx=1; indx<num_segments; indx++)
    {
	int delete_flag=0;
	min_dist = 2147483647;
	//Find the nearest pin to intermediate pin or last pin
	for(int j=0; j<cur_net.numPins; j++)
	{
		if( (flag[j]!=1) )
		{
			int dist = abs(oldp_int.x - pins[j].x) + abs(oldp_int.y - pins[j].y);
			if(dist < min_dist)
			{
				nearest_pin_index=j;
				min_dist = dist;
			}
		}
		if( (flag[j]!=1) && (L_FLAG==1) )
		{
			int dist1 = abs(oldp_int2.x - pins[j].x) + abs(oldp_int2.y - pins[j].y);
			if(dist1 < min_dist)
			{
				nearest_pin_index=j;
				min_dist = dist1;
				delete_flag=1;
			}
		}
	}
	flag[nearest_pin_index]=1;
		//FIXME: Do I need old and new copies of p_int and p_int2??
		if(delete_flag==1)			//The best connection out of the two is chosen in cur_seg_array if there's 2 options available
		{
			for(int e=0; e<old_numEdges; e++)
			{
				rst->edgeUtils[cur_seg_array[indx-1].edges[e]]--;		//Reducing utilization of cur_seg edges because that route is being discarded
				cur_seg_array[indx-1].edges[e] = alt_seg_array[indx-1].edges[e];
				
			}
			oldp_int.x= p_int2.x;	oldp_int.y=p_int2.y;
			delete_flag=0;
			//delete [] alt_seg_array;

		}
		else
		{
			for(int e=0; e<old_numEdges; e++)
			{
				rst->edgeUtils[alt_seg_array[indx-1].edges[e]]--;		//Reducing utilization of alt_seg edges because that route is being discarded
			}
		}
			
		L_FLAG=0;
		
		if( oldp_int.y == pins[nearest_pin_index].y )
	{
		/*Horizontal flat segment*/
		// point p_int;
		p_int.x = pins[nearest_pin_index].x;
		p_int.y = pins[nearest_pin_index].y;
		//oldp_int = p_int;
		int numEdges = abs(oldp_int.x - pins[nearest_pin_index].x);

		cur_seg_array[indx].numEdges = numEdges;
		
		cur_seg_array[indx].edges    = new int[numEdges];
		
		int source_x = oldp_int.x;

		if ( oldp_int.x > pins[nearest_pin_index].x )
			source_x = pins[nearest_pin_index].x;

		int first_edge_indx	     = ( (oldp_int.y)*(rst->gx - 1) + source_x );
	
         	cur_seg_array[indx].edges[0] = first_edge_indx;
		rst->edgeUtils[ first_edge_indx ]++;	

		for(int e=1; e< numEdges; e++) {
		
			cur_seg_array[indx].edges[e] = ++first_edge_indx;
			
			rst->edgeUtils[ first_edge_indx ]++;	
		 }

 		oldp_int = p_int;
		old_numEdges = numEdges;
	}

		else if ( oldp_int.x == pins[nearest_pin_index].x) {
		//point p_int;
		p_int.x = pins[nearest_pin_index].x;
		p_int.y = pins[nearest_pin_index].y;
		//oldp_int = p_int;
		/*Vertical flat segment*/

		int numEdges		     = abs(oldp_int.y - pins[nearest_pin_index].y);

		cur_seg_array[indx].numEdges = numEdges;

		cur_seg_array[indx].edges    = new int[numEdges];
			
		int source_y = oldp_int.y;
		if ( oldp_int.y > pins[nearest_pin_index].y )
			source_y = pins[nearest_pin_index].y;
	
		int first_edge_indx	     = ( (oldp_int.x)*(rst->gy - 1) + source_y + rst->gy * (rst->gx - 1) );

		cur_seg_array[indx].edges[0] = first_edge_indx;
		
		rst->edgeUtils[ first_edge_indx ]++;	

		for(int e=1; e< numEdges; e++) {

			cur_seg_array[indx].edges[e] = ++first_edge_indx;
			
			rst->edgeUtils[ first_edge_indx ]++;	
		    }
		p_int = oldp_int;
		old_numEdges = numEdges;
		}
	
		else   {
		  //Routing both alternative paths between 2 points!
		  L_FLAG=1;
		  //point p_int, p_int2;
		  p_int.y = oldp_int.y;
		  p_int.x = pins[nearest_pin_index].x;
		
		  p_int2.y = pins[nearest_pin_index].y;
		  p_int2.x = oldp_int.x;
		  
		  //oldp_int = p_int;
		  //oldp_int2 = p_int2;

		  int numEdges = abs(oldp_int.x - p_int.x) + abs(pins[nearest_pin_index].y - p_int.y);
		  cur_seg_array[indx].numEdges = numEdges;
		  cur_seg_array[indx].edges    = new int[numEdges];
		  
		  alt_seg_array[indx].numEdges = numEdges;
		  alt_seg_array[indx].edges   = new int[numEdges];
		   // First horizontal flat segment for cur_seg_array and vertical flat segment for alt_seg_array
		   int h_numEdges	       = abs(p_int.x - oldp_int.x);
		   int source_x = oldp_int.x;

		   int v_numEdges   =  abs(p_int2.y - oldp_int.y);
		   int source_y = oldp_int.y;

		if ( oldp_int.x > p_int.x )	
			source_x = p_int.x;

		   int first_edge_indx         = ( (oldp_int.y)*(rst->gx-1) + source_x );
		   
		   rst->edgeUtils[ first_edge_indx ]++;	
		   cur_seg_array[indx].edges[0] = first_edge_indx;
		
		    for(int e=1; e<h_numEdges; e++) {
			cur_seg_array[indx].edges[e] = ++first_edge_indx;
			
			rst->edgeUtils[ first_edge_indx ]++;	
		    }

		if ( oldp_int.y > p_int2.y )	
			source_y = p_int2.y;

		   int first_edge_indx_v         = ( (oldp_int.x)*(rst->gy-1) + source_y + rst->gy*(rst->gx - 1) );
		   alt_seg_array[indx].edges[0] = first_edge_indx_v;
		
		    rst->edgeUtils[ first_edge_indx_v ]++;	
		    for(int e=1; e<v_numEdges; e++) {
			alt_seg_array[indx].edges[e] = ++first_edge_indx_v;
			
			rst->edgeUtils[ first_edge_indx_v ]++;	
		    }
		 	
		  //Then vertical flat segment for cur_seg_array and horizonal flat segment for alt_seg_array
		int source_y2 = pins[nearest_pin_index].y;

		if ( pins[nearest_pin_index].y > p_int.y ){ 	
			source_y2 = p_int.y;
		}
		   
		int first_edge_indx_v2	          = ( (p_int.x)*(rst->gy-1) + source_y2 + rst->gy*(rst->gx - 1) );
		 cur_seg_array[indx].edges[h_numEdges] = first_edge_indx_v2;

		    rst->edgeUtils[ first_edge_indx_v2 ]++;	
		   for(int e=1+h_numEdges; e< numEdges; e++) {
			cur_seg_array[indx].edges[e] = ++first_edge_indx_v2;
			
			rst->edgeUtils[ first_edge_indx_v2 ]++;	
		    }

		int source_x2 = pins[nearest_pin_index].x;

		if ( pins[nearest_pin_index].x > p_int2.x ){ 	
			source_x2 = p_int2.x;
		}
		   
		int first_edge_indx2	          = ( (p_int2.y)*(rst->gx-1) + source_x2 );
		 alt_seg_array[indx].edges[v_numEdges] = first_edge_indx2;
		
		rst->edgeUtils[ first_edge_indx2 ]++;	

		   for(int e=1+v_numEdges; e< numEdges; e++) {
			alt_seg_array[indx].edges[e] = ++first_edge_indx2;
			
			rst->edgeUtils[ first_edge_indx2 ]++;	
		    }

		oldp_int = p_int; oldp_int2= p_int2;
		old_numEdges = numEdges;	
		} //end of L-shaped else	

		
    }
	

  }	//end of for-loop	
	
 }		//End of net decomposition



  if( n == 1) {

	ripnreroute( rst );
 
   }



  return 1;
}


int ripnreroute(routingInst *rst){

    int *edge_weights; //Edge weight array
    int *edge_ovfhist; //Edge overflow-history array


    edge_weights = new int[rst->numEdges];
    edge_ovfhist = new int[rst->numEdges];

    /* Calculating edge weights for first time */

    for (int k=0;k< rst->numEdges; k++) {

		int overflow = rst->edgeUtils[k] - rst->edgeCaps[k];
	
		if ( overflow < 0 ) {
			
			overflow = 0; 
		}

		edge_weights[k] = overflow;

    		edge_ovfhist[k] = 1;    // Initializing overflow history to 1 for each edge 

    }
  //declarations
 
  typedef adjacency_list<listS, vecS, undirectedS, no_property,
    property<edge_weight_t, int> > mygraph_t;
  typedef property_map<mygraph_t, edge_weight_t>::type WeightMap;
  typedef mygraph_t::vertex_descriptor vertex;
  typedef mygraph_t::edge_descriptor edge_descriptor;
  typedef mygraph_t::vertex_iterator vertex_iterator;
  typedef std::pair<int, int> edge;

struct found_goal {}; // exception for termination

// visitor that terminates when we find the goal
template <class Vertex>
class astar_goal_visitor : public boost::default_astar_visitor
{
public:
  astar_goal_visitor(Vertex goal) : m_goal(goal) {}
  template <class Graph>
  void examine_vertex(Vertex u, Graph& g) {
    if(u == m_goal)
      throw found_goal();
  }
private:
  Vertex m_goal;
};

// manhattan distance heuristic
template <class Graph, class CostType, point>
class distance_heuristic : public astar_heuristic<Graph, CostType>
{
public:
  typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
  distance_heuristic(point l, Vertex goal)
    : m_location(l), m_goal(goal) {}
  CostType operator()(Vertex u)
  {
  	int goal_y = m_goal/rst->gx; //FIXME
  	int goal_x = m_goal%rst->gx;
    CostType dx = abs(m_location.x - goal_x);
    CostType dy = abs(m_location.y - goal_y);
    return ::(dx + dy);
  }
private:
  point m_location;
  Vertex m_goal;
};
  //Creating Edges
   Edge edge_array[rst->numEdges]; 
  for(int j=0; j<rst->numEdges;j++)
  {
  	int edgeid = j;
  	if  ( edgeid < ( rst->gy*(rst->gx-1) ) ) { //then horizontal edge
			
				int height = edgeid/( rst->gx - 1);
				int source_x = edgeid - height*(rst->gx - 1);
				int dest_x = source_x + 1;
				int src_node_mapping = source_x + rst->gx*height;
				//fprintf(fp,"(%d,%d)-(%d,%d)\n",source_x,height,dest_x,height);
				edge_array[j]= edge(src_node_mapping, src_node_mapping+1);

			}
				

			else 				  {  // then vertical edge
	 			
				int offset   =  (edgeid - rst->gy*(rst->gx-1) );
				int width    =  offset/(rst->gy - 1);
				int source_y =  offset - width*( rst->gy - 1);
				int src_node_mapping = width + rst->gx*source_y;
				edge_array[j]=edge(src_node_mapping,src_node_mapping+rst->gx);
				//fprintf(fp,"(%d,%d)-(%d,%d)\n",width,source_y,width,source_y+1);
			}
	}

    int num_arcs = rst->numEdges;
    int num_nodes = rst->gx * rst-> gy;

    //Creating graph
    mygraph_t g(num_nodes);
    WeightMap weightmap = get(edge_weights, g);
    for(std::size_t j = 0; j < rst->numEdges; j++) {
    edge_descriptor e;
    bool inserted;
    tie(e, inserted) = add_edge(edge_array[j].first,
                                edge_array[j].second, g);
    weightmap[e] = edge_weights[j];
  }
  /* Start ripnreroute loop */

  //int ticks = 100000000;
  //int cur_ticks = 0;
  

  
  do {

   /* Finding the nets with overflow */

  
   net *net_array = rst->nets;

   int net_cost=0;
   std::priority_queue <int> net_indxQ;  //Using a max-queue to store net indices in descending order

   for(int k=0;k<rst->numNets;k++) {

	segment *cur_seg_array = net_array[k].nroute.segments;
	
	net_cost = 0;

	for(int j=0;j<net_array[k].nroute.numSegs;j++) {

		int *edgearray = cur_seg_array[j].edges;
		int numEdges = cur_seg_array[j].numEdges;

		for (int l=0;l<numEdges;l++) {

			int edgeid = edgearray[l];			
			net_cost  += edge_weights[ edgeid ];

		}

	}

	if (net_cost > 0) {

		net_indxQ.push( k );

  	}	


  }
  

  /* Rip-up and reroute the bad nets  */


   while ( net_indxQ.empty() == false ) {

	int net_indx = net_indxQ.top();

	net_indxQ.pop();
			
	segment *cur_seg_array = net_array[ net_indx ].nroute.segments;


	/* Ripup the net */
	
	for(int j=0;j<net_array[ net_indx ].nroute.numSegs;j++) {

		int *edgearray = cur_seg_array[j].edges;
		int numEdges = cur_seg_array[j].numEdges;

		for (int l=0;l<numEdges;l++) {

			int edgeid = edgearray[l];
			
			rst->edgeUtils[ edgeid ]--;	
				
			int overflow = rst->edgeUtils[ edgeid ] - rst->edgeCaps[ edgeid ];
	
			if ( overflow < 0 ) {
			
				overflow = 0; 
			}

			if ( overflow > 0 ) {

				edge_ovfhist[ edgeid ]++;

			}		
	

			edge_weights[ edgeid ] = overflow*edge_ovfhist[ edgeid ] ;

		}

	}


	/* Reroute the net */

	//Using A* to re-route the net
	//Loop through all the segments. For each segment from Point P1 to P2 there's number of edges. It is these edges which are being re-routed through A*
	for(int indx=0; indx<net_array[ net_indx ].nroute.numSegs; indx++)
	{
	 
   		point start_vertex = cur_seg_array[indx].p1;
		point end_vertex   = cur_seg_array[indx].p2;

		int src_node = start_vertex.y * rst->gx + start_vertex.x;
		int dst_node = end_vertex.y * rst->gx + end_vertex.x;

		vector<mygraph_t::vertex_descriptor> p(num_vertices(g));  
		vector<int>d(num_vertices(g));

		try {

						//calling astar here
			astar_search(g, src_node, distance_heuristic<mygraph_t,int, location*>(locations, dst_node), 
				predecessor_map(&p[0]).distance_map(&d[0]).visitor(astar_goal_visitor<vertex>(goal)));

		} catch(found_goal fg) { // found a path to the goal
    list<vertex> shortest_path;
    for(vertex v = goal;; v = p[v]) {
      shortest_path.push_front(v);
      if(p[v] == v)
        break;
    }
    list<vertex>::iterator spi = shortest_path.begin();
 	int e=0;
    for(++spi; spi != shortest_path.end(); ++spi)
      { int vert = *spi;
      	int vert2 = *(spi+1);
      	int src_x = vert%rst->gx;
      	int src_y = vert/rst->gx;
      	int src_x2 = vert2%rst->gx;
      	int src_y2 = vert2/rst->gx;
      	int source_y,source_x;
      	int edge_indx;
      	if(src_x == src_x2)
      	{
        	if ( src_y2 > src_y )
				source_y = src_y;
		edge_indx	     = ( src_x)*(rst->gy - 1) + source_y + rst->gy * (rst->gx - 1) );
		}
		else 
      	{
        	if ( src_x2 > src_x )
				source_x = src_x;
		edge_indx	     = ( src_y)*(rst->gx - 1) + source_x;
		}
		cur_seg_array[indx].edges[e++] = edge_indx;
	  }
	} //end of catch
   }  //end of segment for the current (for loop)
  }// for all nets
	
   /* Updating edge weights at end of current iteration */
   //FIXME: Do we need to update edge weights at every iteration?? This could be done only when a net is ripped up 
    for (int k=0;k< rst->numEdges; k++) {

		int overflow = rst->edgeUtils[k] - rst->edgeCaps[k];
	
		if ( overflow < 0 ) {
			
			overflow = 0; 
		}

		if ( overflow > 0 ) {

			edge_ovfhist[k]++;
		}

		edge_weights[k] = overflow*edge_ovfhist[k];


    }

  for(std::size_t j = 0; j < rst->numEdges; j++) {
    edge_descriptor e;
    bool inserted;
    tie(e, inserted) = add_edge(edge_array[j].first,
                                edge_array[j].second, g);
    weightmap[e] = edge_weights[j];
  }

   end = time(NULL);
   elapsed_time = difftime(end, start); 

   } while (elapsed_time < 90);


  return 0;

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
  

