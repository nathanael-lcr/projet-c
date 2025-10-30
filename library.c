#include "library.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//To know how many nodes need to be allocated before creating the graph.
int count_nodes_with_name(char *filename){
    if (filename == NULL) return BAD_FILE_FORMAT;
    int number_nodes = 0;
    char buffer[256];
    FILE *file = fopen(filename, "r");
    if (file == NULL) return FILE_NOT_FOUND;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (buffer[0] >= '0' && buffer[0] <= '9' &&
            strchr(buffer, '-') == NULL)
        {
            number_nodes++;
        }
    }
    fclose(file);
    return number_nodes;
}
//Counts the total number of connections between nodes
int count_links(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL) return 1;
    int number_links = 0;
    char buffer[256];
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) return FILE_NOT_FOUND;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (buffer[0] >= '0' && buffer[0] <= '9' &&
            strchr(buffer, '-') != NULL)
        {
            number_links++;
        }
    }   
    fclose(file);
    return number_links;
}
//Identifies where the pathfinding should begin.
int get_start_node(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL) return BAD_FILE_FORMAT;
    char buffer[256];
    int next_start = 0;
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) return FILE_NOT_FOUND;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, "#start") == 0)
            next_start = 1;
        if (next_start && buffer[0] >= '0' &&
            buffer[0] <= '9' && strchr(buffer, '-') == NULL)
        {
            fclose(file);
            return atoi(buffer);
        }
    }
    fclose(file);
    return NO_START_NODE;
}
//Identifies where the pathfinding should stop.
int get_end_node(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL) return BAD_FILE_FORMAT;
    char buffer[256];
    int next_end = 0;
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) return FILE_NOT_FOUND;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, "#end") == 0)
            next_end = 1;
        if (next_end && buffer[0] >= '0' && buffer[0] <= '9' &&
            strchr(buffer, '-') == NULL)
        {
            fclose(file);
            return atoi(buffer);
        }
    }
    fclose(file);
    return NO_END_NODE;
}

//Helps connect nodes together by looking them up by ID
int find_node_index(Node **nodes, int nb_nodes, int id)
{
    for (int i = 0; i < nb_nodes; i++)
        if (nodes[i]->id == id)
            return i;
    return -1;
}
//Creates and initializes the node structures.
Node** create_nodes(char *filename, int nb_nodes)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) return (Node**)FILE_NOT_FOUND;
    
    Node **nodes = malloc(sizeof(Node*) * nb_nodes);
    if (!nodes) return (Node**)BAD_FILE_FORMAT;
    char buffer[256];
    int index = 0;
    
    while (fgets(buffer, sizeof(buffer), file))
        if (buffer[0] >= '0' && buffer[0] <= '9' && !strchr(buffer, '-')) {
            nodes[index] = malloc(sizeof(Node));
            if (!nodes[index]) return (Node**)BAD_FILE_FORMAT;
            nodes[index]->id = atoi(buffer);
            nodes[index]->links = NULL;
            nodes[index]->link_count = 0;
            nodes[index]->visited = 0;
            index++;
        }
    
    fclose(file);
    return nodes;
}
//Determines how many neighbors (links) each node will have.
int* count_links_filename(char *filename, Node **nodes, int nb_nodes)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) return (Node**)FILE_NOT_FOUND;
    
    int *link_counts = calloc(nb_nodes, sizeof(int));
    if (!link_counts) return (int*)BAD_FILE_FORMAT;
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), file))
        if (strchr(buffer, '-')) {
            int node1, node2;
            sscanf(buffer, "%d-%d", &node1, &node2);
            int nodeindex1 = find_node_index(nodes, nb_nodes, node1);
            int nodeindex2 = find_node_index(nodes, nb_nodes, node2);
            if (nodeindex1 != -1) link_counts[nodeindex1]++;
            if (nodeindex2 != -1) link_counts[nodeindex2]++;
        }
    
    fclose(file);
    return link_counts;
}
//Allocates memory for adjacency lists (neighbor pointers).
void allocate_links(Node **nodes, int *link_counts, int nb_nodes)
{
    if (nodes == NULL || link_counts == NULL) {
        return BAD_FILE_FORMAT;
    }
    for (int i = 0; i < nb_nodes; i++) {
        nodes[i]->links = malloc(sizeof(Node*) * link_counts[i]);
        nodes[i]->link_count = 0;
    }
}
//Builds the actual connections between nodes.
void fill_links(char *filename, Node **nodes, int nb_nodes)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) return FILE_NOT_FOUND;
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file))
        if (strchr(buffer, '-')) {
            int node1, node2;
            sscanf(buffer, "%d-%d", &node1, &node2);
            int nodeidx1 = find_node_index(nodes, nb_nodes, node1);
            int nodeidx2 = find_node_index(nodes, nb_nodes, node2);
            
            if (nodeidx1 != -1 && nodeidx2 != -1) {
                nodes[nodeidx1]->links[nodes[nodeidx1]->link_count++] = nodes[nodeidx2];
                nodes[nodeidx2]->links[nodes[nodeidx2]->link_count++] = nodes[nodeidx1];
            }
        }
    
    fclose(file);
}
//Central function that builds the graph structure from a file.
Node** init_node(char *filename)
{
    //We count the number of nodes in the file
    int nb_nodes = count_nodes_with_name(filename);
    if (nb_nodes <= 0) return BAD_FILE_FORMAT;
    //We create nodes (the real structs) for each node read
    Node **nodes = create_nodes(filename, nb_nodes);
    if (nodes == NULL) return NULL;
    
    //For each line with a `-` (like `3-4`), we parse the both numbers
    //We find the indexes of the relevant nodes and increment their counters
    int *link_counts = count_links_filename(filename, nodes, nb_nodes);
    allocate_links(nodes, link_counts, nb_nodes);
    //We end up filling in the links and therefore making them point to each other
    fill_links(filename, nodes, nb_nodes);
    
    free(link_counts);
    return nodes;
}


//Displays the list of connected nodes in the graph.
void display_nodes(Node* start) {
    if (start == NULL) return;
    if (start->visited == 1) return;
    
    start->visited = 1;
    printf("%d ", start->id);
    for (int i = 0; i < start->link_count; i++) {
        display_nodes(start->links[i]);
    }
}
//Finds and returns isolated nodes in the graph.
Node** get_unconnected_nodes( Node **nodes, int size, Node *head ){
    //display_nodes(head);//marks all nodes accessible from head with visited = 1
    int count=0;
    Node **unconnected_nodes=malloc(sizeof(Node*)*size);
    if (!unconnected_nodes) return BAD_FILE_FORMAT;
    for (int i=0; i<size;i++){
        if(nodes[i]->visited == 0){
            unconnected_nodes[count]=nodes[i];
            count++;
        }
    }
    unconnected_nodes[count] = NULL; //mark the end
    return unconnected_nodes;
    free(unconnected_nodes);
}
//Displays all disconnected nodes.
void print_unconnected_nodes(Node **unconnected_nodes)
{
    if (unconnected_nodes == NULL || unconnected_nodes[0] == NULL) {
        return;
    }

    printf("unconnected nodes :\n");
    int i = 0;
    while (unconnected_nodes[i] != NULL) {
        printf("%d ", unconnected_nodes[i]->id);
        i++;
    }
    printf("\n");
}
//Prepares the graph for a new search (like before running BFS again).
void reset_nodes(Node **nodes, int size)
{
    for (int i = 0; i < size; i++) {
        nodes[i]->visited = 0;
        nodes[i]->distance = 0;
        nodes[i]->parent = NULL;
    }
}
//Helper function for one BFS step — processes all neighbors.
void breadth_first_sarch(Node *current, Node **file, int *tail) {
    for (int i = 0; i < current->link_count; i++) {
        if (current->links[i]->visited == 0) {
            current->links[i]->visited = 1;                     
            current->links[i]->parent = current;                
            current->links[i]->distance = current->distance + 1;
            file[(*tail)++] = current->links[i];                
        }
    }
}
//Explores the graph to compute the shortest path using BFS.
void find_shortest_path(Node *start, Node *end, int size){
    int head = 0; 
    int tail = 0;
    if (start == NULL) {
        printf("Start node not found!\n");
        return NO_START_NODE;
    }
    if (end == NULL) {
        printf("End node not found!\n");
        return NO_END_NODE;
    }
    Node* current = start;
    Node** file=malloc(sizeof(Node*)*size);
    if (!file) return BAD_FILE_FORMAT;
    file[tail++] = start; 
    start->visited = 1;
    start->distance = 0;
    //Traverses all nodes in the queue in FIFO order
    while(head < tail){ 
        current=file[head];
        head++;
        if (current == end) break;
    }
    free(file);
    return;
}
//displays the computed shortest path in a readable format.
void print_shortest_path(Node *start, Node *end)
{
    // Check if a path exists
    if (end->parent == NULL && end != start) {
        printf("No path found.\n");
        return;
    }

    // Count the length of the path
    int path_length = 0;
    Node *current = end;
    while (current != NULL) {
        path_length++;
        current = current->parent;
    }

    // Create an array to store the path (in reverse order)
    Node **path = malloc(sizeof(Node*) * path_length);
    if (!path) return BAD_FILE_FORMAT;
    current = end;
    int index = path_length - 1;
    
    // Fill the table by working backwards from end to start
    while (current != NULL) {
        path[index] = current;
        current = current->parent;
        index--;
    }
    
    // Show path
    printf("Shortest path (distance: %d): ", end->distance);
    for (int i = 0; i < path_length; i++) {
        printf("%d", path[i]->id);
        if (i < path_length - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
    
    free(path);
}