#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <openssl/sha.h>
#include <igraph/igraph.h>

// Function to flip binary digit
char flip(char c) {
    return (c == '0') ? '1' : '0';
}

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

// Function to print ones complement
void printOnesComplement(const char* bin, int n, char* result) {
    for (int i = 0; i < n; i++) {
        result[i] = flip(bin[i]);
    }
    result[n] = '\0';
    printf("1's complement: %s\n", result);
}

// Function to check if a vertex is in an array
bool contains(igraph_vector_int_t* vec, igraph_integer_t value, igraph_integer_t size) {
    for (igraph_integer_t i = 0; i < size; i++) {
        if (VECTOR(*vec)[i] == value) return true;
    }
    return false;
}

// Structure to store graph extension results
typedef struct {
    igraph_t* graph;
    int min_degree;
} ExtendedGraph;

// Function to extend the graph
ExtendedGraph extend(igraph_t* G) {
    igraph_integer_t num_nodes = igraph_vcount(G);
    igraph_integer_t num_edges = igraph_ecount(G);
    igraph_integer_t desired_edges = 2 * num_edges;
    
    printf("total edges: %ld\n", (long int)num_edges);
    
    // Create a new graph
    igraph_t* G_temp = (igraph_t*)malloc(sizeof(igraph_t));
    igraph_copy(G_temp, G);
    char binary_string[SHA256_DIGEST_LENGTH * 16 + 1];  // Space for binary + complement + null
    // Generate hash string
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char data[] = "this string holds important and private information";
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, strlen(data));
    SHA256_Final(hash, &sha256);
    
    // Convert hash to binary string
    char binary_hash[SHA256_DIGEST_LENGTH * 8 + 1];
    int binary_pos = 0;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        for (int j = 7; j >= 0; j--) {
            binary_hash[binary_pos++] = ((hash[i] >> j) & 1) ? '1' : '0';
        }
    }
    binary_hash[binary_pos] = '\0';
    printf("Binary hash: %s\n", binary_hash);
    // Get ones complement
    char complement_string[SHA256_DIGEST_LENGTH * 8 + 1];
    for(int i = 0; i < binary_pos; i++) {
        complement_string[i] = flip(binary_hash[i]);
    }
    complement_string[binary_pos] = '\0';
    
    // Combine original binary and complement
    snprintf(binary_string, sizeof(binary_string), "%s%s", binary_hash, complement_string);
    

 
    
    
    printf("Binary hash: %s,%d\n", binary_string, strlen(binary_string));
    
    
    // Get ones complement
    //char ones_complement[SHA256_DIGEST_LENGTH * 8 + 1];
    //printOnesComplement(binary_hash, binary_pos, ones_complement);
    
    // Add new nodes and edges
    igraph_vector_int_t edges;
    igraph_vector_int_init(&edges, 0);
    
    int t = 0;
    int l = 0;
    int start = 0;
    
    // Find minimum degree
    igraph_vector_t degrees;
    igraph_vector_init(&degrees, 0);
    igraph_degree(G, &degrees, igraph_vss_all(), IGRAPH_ALL, IGRAPH_LOOPS);
    int min_degree = (int)igraph_vector_min(&degrees);
    igraph_vector_destroy(&degrees);
    
    // Add new nodes and connect based on hash
    for (igraph_integer_t v = 0; v < num_nodes; v++) {
        igraph_add_vertices(G_temp, 1, 0);
        igraph_integer_t new_node = num_nodes + start;
        
        igraph_vector_int_t neis;
        igraph_vector_int_init(&neis, 0);
        igraph_neighbors(G, &neis, v, IGRAPH_ALL);
        
        l += igraph_vector_int_size(&neis);
        if(l>=sizeof(binary_string))
        {

            t=0;
            l=0;
            l += igraph_vector_int_size(&neis);
        }
        //l=l%sizeof(binary_string);  
        int counter = 0;
        
        for (int i = t; i < l; i++) {
            if (binary_string[i] == '1') {
                igraph_vector_int_push_back(&edges, VECTOR(neis)[counter]);
                igraph_vector_int_push_back(&edges, new_node);
                //printf("Edge added: (%ld, %ld)\n", (long int)VECTOR(neis)[counter], (long int)new_node);
            }
            counter++;
        }
        
        t += igraph_vector_int_size(&neis);
        
        //t=t%sizeof(binary_string);
        start++;
        igraph_vector_int_destroy(&neis);
    }
    
    // Add the collected edges
    if (igraph_vector_int_size(&edges) > 0) {
        igraph_add_edges(G_temp, &edges, 0);
    }
    igraph_vector_int_destroy(&edges);
    
    // Add random edges between new nodes
    int num_random_edges = (min_degree * (num_nodes - 1)) / 2 + 2;
    igraph_vector_int_t random_edges;
    igraph_vector_int_init(&random_edges, 0);
    
    for (int i = 0; i < num_random_edges; i++) {
        igraph_integer_t start_node = num_nodes + (rand() % num_nodes);
        igraph_integer_t end_node = num_nodes + (rand() % num_nodes);
        
        igraph_bool_t connected;
        igraph_are_connected(G_temp, start_node, end_node, &connected);
        
        if (start_node != end_node && !connected) {
            igraph_vector_int_push_back(&random_edges, start_node);
            igraph_vector_int_push_back(&random_edges, end_node);
        }
    }
    
    if (igraph_vector_int_size(&random_edges) > 0) {
        igraph_add_edges(G_temp, &random_edges, 0);
    }
    igraph_vector_int_destroy(&random_edges);
    
    printf("total nodes: %ld\n", (long int)igraph_vcount(G_temp));
    printf("total edges: %ld\n", (long int)igraph_ecount(G_temp));
    
    ExtendedGraph result = {G_temp, min_degree};
    return result;
}

// Function to check if a set is dominating
bool is_dominating_set(igraph_t* G, igraph_vector_int_t* set,int *visited,int *white) {
    igraph_vector_bool_t covered;
    igraph_vector_bool_init(&covered, igraph_vcount(G));
    igraph_vector_bool_fill(&covered, 0);
    
    // Mark all nodes in the set and their neighbors as covered
    for (igraph_integer_t i = 0; i < igraph_vector_int_size(set); i++) {
        igraph_integer_t node = VECTOR(*set)[i];
        VECTOR(covered)[node] = 1;
        
        igraph_vector_int_t neis;
        igraph_vector_int_init(&neis, 0);
        igraph_neighbors(G, &neis, node, IGRAPH_ALL);
        
        for (igraph_integer_t j = 0; j < igraph_vector_int_size(&neis); j++) {
            VECTOR(covered)[VECTOR(neis)[j]] = 1;
        }
        
        igraph_vector_int_destroy(&neis);
    }
    
    // Check if all nodes are covered
    bool result = true;
    for (igraph_integer_t i = 0; i < igraph_vcount(G); i++) {
        if (!VECTOR(covered)[i]) {
            result = false;
           // printf("Node %d is not covered,%d,%d\n", i,visited[i],white[i]); 
            //break;
        }
    }
    
    igraph_vector_bool_destroy(&covered);
    return result;
}

int main(int argc, char* argv[]) {
    igraph_t G;
    int *height;    
    clock_t t,e; 
    unsigned long int totalBytes;
    srand(time(NULL));         
    igraph_integer_t eid,from,to,value,vid,eid1,flow,source,sink,count=0;
    igraph_vit_t vit3,vit4,vit5;
    igraph_vs_t vs3,vs4,vs5;
    igraph_set_attribute_table(&igraph_cattribute_table);
    
    FILE *fp=fopen(argv[1],"r"); 
    FILE *fp1=fopen(argv[2],"a"); 
    
    if(!fp)
    {
        perror("file does not exist\n");
        exit(1);
    }
    
    igraph_read_graph_edgelist(&G, fp, 0, IGRAPH_UNDIRECTED);
    
  
    
    // Extend the graph and find dominating set
    clock_t start = clock();
    ExtendedGraph extended = extend(&G);
    igraph_t* G_temp = extended.graph;
    
    // Find dominating set using greedy approach
    igraph_vector_int_t dominating_set;
    igraph_vector_int_init(&dominating_set, 0);
    
    // Create a copy of the graph for modification
    igraph_t G_working;
    igraph_copy(&G_working, G_temp);
    
    int n=igraph_vcount(G_temp);
    int *white=(int*)malloc(igraph_vcount(G_temp)*sizeof(int));
    int *visited=(int*)malloc(igraph_vcount(G_temp)*sizeof(int));
    int max_degree=-1;
    
    int i;  // Declare loop variable
    
    for(i=0; i<n; i++) {

        white[i]=count_white_new(i, G_temp, visited);
       // printf("%d,%d,%d\n",i,white[i],n);
        
        visited[i] = 0;  // Initialize visited array
    }
    //exit(1);
    while (1) {
        // Find node with highest degree
        igraph_integer_t max_deg_node = -1;
        igraph_real_t max_degree = 0;    
        
        for (igraph_integer_t i = 0; i < igraph_vcount(&G_working); i++) {
            if (white[i] > max_degree && visited[i]!=1) {
                max_degree = white[i];
                max_deg_node = i;
            }
        }
        visited[max_deg_node]=1;
        white[max_deg_node]=0;
        // Add to dominating set
        igraph_vector_int_push_back(&dominating_set, max_deg_node);
        
        // Remove the node and its neighbors
        igraph_vector_int_t neighbors;
        igraph_vector_int_init(&neighbors, 0);
        igraph_neighbors(&G_working, &neighbors, max_deg_node, IGRAPH_ALL);
        
        for (igraph_integer_t j = 0; j < igraph_vector_int_size(&neighbors); j++) {
            if(visited[VECTOR(neighbors)[j]]==0) {
                //printf("%d\n",VECTOR(neighbors)[j]);
                //igraph_delete_vertices(&G_working, 1, &VECTOR(neighbors)[j]);
                visited[VECTOR(neighbors)[j]]=2;
            }
        }
        
        igraph_vector_int_destroy(&neighbors);
        int done=1;
        // Update white counts
        for(i=0; i<n; i++) {
            if(visited[i]!=1) {
                white[i]=count_white_new(i, G_temp, visited);
                if(white[i]>0)
                {
                    done=0;
                }
            }
        }
        
        // Check if we're done (no more white nodes)
        
        if(done) break;
    }
    clock_t end = clock();
    clock_t start1 = clock();
    

    // Verify dominating set
    if (is_dominating_set(G_temp, &dominating_set,visited,white)) {
        printf("Verified: The set is a valid dominating set\n");
    } else {
        printf("Error: The set is not a valid dominating set\n");
    }
    clock_t end1 = clock();
    
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    double time_spent1 = (double)(end1 - start1) / CLOCKS_PER_SEC;
    fprintf(fp1,"%ld,%ld,%ld,%fs,%fs\n",
        igraph_vcount(&G),
        igraph_ecount(&G),
        (long int)igraph_vector_int_size(&dominating_set),
        time_spent,time_spent1);
    
    fclose(fp1);
    fclose(fp);
    
    // Clean up
    igraph_vector_int_destroy(&dominating_set);
    igraph_destroy(&G_working);
    igraph_destroy(&G);
    igraph_destroy(G_temp);
    free(visited);
    
    free(white);
    
    return 0;
}
