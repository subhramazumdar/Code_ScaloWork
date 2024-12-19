#include <igraph/igraph.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>
int max_degree=-1;
int round_count=0;
int count_white_new(int vid, const igraph_t *g, const int *visited) {
    igraph_vector_int_t neis;
    igraph_vector_int_init(&neis, 0);
    igraph_neighbors(g, &neis, vid, IGRAPH_ALL);
    
    int count = 0;
    long int i, n = igraph_vector_int_size(&neis);
    for (i = 0; i < n; i++) {
        if (visited[(int)VECTOR(neis)[i]]==0) {
            count++;
        }
    }
    igraph_vector_int_destroy(&neis);
    return count;
}

int bfs(int vid, int level, igraph_t *g, int *white, int *visited) {
    igraph_vector_int_t neis;
    igraph_vector_int_init(&neis, 0);
    igraph_neighbors(g, &neis, vid, IGRAPH_ALL);
    igraph_vit_t vit3,vit4,vit5;
    igraph_vs_t vs3,vs4,vs5;

    int flag = 0;
    long int i, n = igraph_vector_int_size(&neis);
    
    for (i = 0; i < n && white[vid] > 0; i++) {
        int neighbor = VECTOR(neis)[i];
        if (visited[neighbor]!=1 && white[neighbor] > white[vid]) {
            igraph_vector_int_destroy(&neis);
            return -1;
        }
     
                        igraph_vs_adj(&vs4,neighbor,IGRAPH_ALL);
                        igraph_vit_create(g, vs4, &vit4);
                        while (!IGRAPH_VIT_END(vit4)) {

                        // white[IGRAPH_VIT_GET(vit4)]=count_white_new(IGRAPH_VIT_GET(vit4),g,visited);
                           if(visited[IGRAPH_VIT_GET(vit4)]!=1 && white[IGRAPH_VIT_GET(vit4)]>white[vid])
                           {
                                //win=IGRAPH_VIT_GET(vit4);
                                //count=white[IGRAPH_VIT_GET(vit4)];
      
                                return -1;
                                //printf("win1=%d\n",win);

                           }    
                           IGRAPH_VIT_NEXT(vit4);

                        }
                        
                        igraph_vit_destroy(&vit4);
                        igraph_vs_destroy(&vs4);
         }

                  // white[vid]=0;
                   visited[vid]=4;
                   
                 //  printf("vid=%d",vid);
      //             return vid;
      
    
    igraph_vector_int_destroy(&neis);
    return vid;
}

void find_dom(igraph_t *g,int *visited)
{
	    const int n = igraph_vcount(g);
	    int counter=0,vertex,flag=0,breaking=0;
	    
	    int *white=(int*)malloc(igraph_vcount(g)*sizeof(int));

	    int *vertex_id=(int*)malloc(igraph_vcount(g)*sizeof(int));
	   
	    //igraph_integer_t mdeg;
	    igraph_vit_t vit5;
	    igraph_vs_t vs5;
	    //igraph_vector_int_init(&v, igraph_vss_all());
	    int i,j;	    
            max_degree=igraph_vcount(g);
           // printf("%d\n",max_degree);
	    #pragma omp parallel for num_threads(4) 
	     for(i=0;i<n;i++)
	    {
	    	visited[i]=0; 
	    	
	    }
	    #pragma omp parallel for num_threads(4) 
	    for(i=0;i<n;i++)
	    {
	    
	    	white[i]=count_white_new(i,g,visited);
	    	
	    	if(white[i]!=0 && max_degree>white[i])
	    	  max_degree=white[i];
	    	//dom_set[i]=-1;
	    }
	    
	    while(1)
	    {
	    //printf("sub");
	    flag=0;
	     //#pragma omp parallel for num_threads(4) 
	     
	     
	    
	   //#pragma omp parallel for num_threads(4)
	     for(j=0;j<n;j++)
	     {
	         
	         if(visited[j]!=1 && white[j]>0)
	         {
	           
	         	
	         	
	         	
//	         	#pragma omp critical (flag)
	           flag=1;
	             	
	           vertex=bfs(j,2,g,white,visited);

	         	
	         }
	         
	
	     }
	     //#pragma omp critical
	     if(flag==0)
	     {
	      
	         break;
	     }
	     breaking=0;
	     
	for(j=0;j<n;j++)
        {
            if(visited[j]==4)
            {
                visited[j]=1;
                white[j]=0;
                
                igraph_vector_int_t neis;
                igraph_vector_int_init(&neis, 0);
                igraph_neighbors(g, &neis, j, IGRAPH_ALL);
                
                long int i, m = igraph_vector_int_size(&neis);
                
                for (i = 0; i < m; i++) {
                    int neighbor = VECTOR(neis)[i];
                    if (visited[neighbor]==0) {
                        visited[neighbor]=2;
                    }
                }
                
                igraph_vector_int_destroy(&neis);
            }
        }
	     
	  
	     //#pragma omp parallel for num_threads(4)
	for(int i = 0; i < n; i++) {
            if(white[i] > 0) {
                white[i] = count_white_new(i, g, visited);
                if(white[i] > 0) breaking = 1;
            }
        }
        
        if(!breaking) break;

	     //break;
         round_count++;
     }

   free(white);
   free(vertex_id);
	    
}
int main(int argc, char *argv[]) {
  
    igraph_t g;
    int *height;	
    clock_t t,e; 
    unsigned long int totalBytes;
    srand(time(NULL));         
    igraph_integer_t eid,from,to,value,vid,eid1,flow,source,sink,count=0;
    igraph_vit_t vit3,vit4,vit5;
    igraph_vs_t vs3,vs4,vs5;
    igraph_set_attribute_table(&igraph_cattribute_table);
    
    omp_set_num_threads(2000);
    
    FILE *fp=fopen(argv[1],"r"); 
    FILE *fp1=fopen(argv[2],"a"); 
    
    if(!fp)
    {
        perror("file does not exist\n");
        exit(1);
    }
    
    igraph_read_graph_edgelist(&g, fp, 0, IGRAPH_UNDIRECTED);
    int *visited=(int*)malloc(igraph_vcount(&g)*sizeof(int));
    int *mark=(int*)malloc(igraph_vcount(&g)*sizeof(int));
    
    int start=clock();
    
    find_dom(&g,visited);
    int end=clock();
    fclose(fp);
    
    int i,counter=0,eff_count=0;
       clock_t start1 = clock();
    
    for(i=0;i<igraph_vcount(&g);i++)
        mark[i]=0;

    for(i=0;i<igraph_vcount(&g);i++)
    {
        if(visited[i]==1)
        {
            counter++;

            if(mark[i]==0) 
            {
                mark[i]=1;
                eff_count++;
            }
            
            igraph_vector_int_t neis;
            igraph_vector_int_init(&neis, 0);
            igraph_neighbors(&g, &neis, i, IGRAPH_ALL);
            
            long int j, n = igraph_vector_int_size(&neis);
            
            for (j = 0; j < n; j++) {
                int neighbor = VECTOR(neis)[j];
                if (mark[neighbor]==0) {
                    eff_count++;
                    mark[neighbor]=1;
                }
            }
            
            igraph_vector_int_destroy(&neis);
        }
    }
    

    // Verify dominating set
    clock_t end1 = clock();

    double time_spent1 = (double)(end1 - start1) / CLOCKS_PER_SEC;
    int k=(int)log(1+max_degree);
    fprintf(fp1,"%ld,%ld,%d,%d,%ld,%fs,%fs,round=%d\n",
        igraph_vcount(&g),
        igraph_ecount(&g),
        counter,
        eff_count,
        (igraph_integer_t)((igraph_vcount(&g)*(1+k))/(1+max_degree)),
        ((double) (end - start)) / CLOCKS_PER_SEC,time_spent1,round_count);
    
    fclose(fp1);
    //cleanup_resources();
    igraph_destroy(&g);
    free(visited);
    free(mark);
    return 0;
}



