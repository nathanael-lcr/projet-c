#include "library.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int last_error = 0;

// Counts the number of nodes in the file
int count_nodes_with_name(char *filename){
    if (filename == NULL) {
        last_error = BAD_FILE_FORMAT;
        return -1;
    }
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        last_error = FILE_NOT_FOUND;
        return -1;
    }
    int number_nodes = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (buffer[0] >= '0' && buffer[0] <= '9' && 
            strchr(buffer, '-') == NULL) 
            number_nodes++;
    }
    fclose(file);
    return number_nodes;
}

// Counts the number of links in the file
int count_links(int argc, char *argv[]) {
    if (argc < 2 || argv[1] == NULL) {
        last_error = BAD_FILE_FORMAT;
        return -1;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        last_error = FILE_NOT_FOUND;
        return -1;
    }
    int number_links = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (buffer[0] >= '0' && buffer[0] <= '9' && 
            strchr(buffer, '-') != NULL) 
            number_links++;
    }   
    fclose(file);
    return number_links;
}

// Checks if a buffer line contains a node number (no dash)
int is_node_number(char *buffer) {
    return buffer[0] >= '0' && buffer[0] <= '9' && 
           strchr(buffer, '-') == NULL;
}

// Finds and returns the node ID after a specific tag in the file
int get_node_after_tag(int argc, char *argv[], char *tag, 
                       int error) {
    if (argc < 2 || argv[1] == NULL) {
        last_error = BAD_FILE_FORMAT;
        return -1;
    }
    FILE *file = fopen(argv[1], "r");
    if (!file) { last_error = FILE_NOT_FOUND; return -1; }
    char buffer[256];
    int found_tag = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, tag) == 0) found_tag = 1;
        if (found_tag && is_node_number(buffer)) {
            fclose(file);
            return atoi(buffer);
        }
    }
    fclose(file);
    last_error = error;
    return -1;
}

// Gets the start node ID from the file
int get_start_node(int argc, char *argv[]) {
    return get_node_after_tag(argc, argv, "#start", NO_START_NODE);
}

// Gets the end node ID from the file
int get_end_node(int argc, char *argv[]) {
    return get_node_after_tag(argc, argv, "#end", NO_END_NODE);
}

// Finds the index of a node in the nodes array by its ID
int find_node_index(Node **nodes, int nb_nodes, int id) {
    for (int i = 0; i < nb_nodes; i++)
        if (nodes[i]->id == id)
            return i;
    return -1;
}

// Initializes a node structure with default values
void init_node_struct(Node *node, int id) {
    node->id = id;
    node->links = NULL;
    node->link_count = 0;
    node->visited = 0;
    node->distance = 0;
    node->parent = NULL;
}

// Frees memory allocated for an array of nodes
void cleanup_nodes(Node **nodes, int count) {
    for (int i = 0; i < count; i++)
        free(nodes[i]);
    free(nodes);
}

// Creates and initializes a single node with the given ID
Node* create_single_node(int id) {
    Node *node = malloc(sizeof(Node));
    if (node) init_node_struct(node, id);
    return node;
}

// Reads node IDs from file and creates node structures
int read_and_create_nodes(FILE *file, Node **nodes) {
    char buffer[256];
    int index = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (buffer[0] >= '0' && buffer[0] <= '9' && 
            !strchr(buffer, '-')) {
            nodes[index] = create_single_node(atoi(buffer));
            if (!nodes[index]) return index;
            index++;
        }
    }
    return -1;
}

// Creates an array of nodes from the file
Node** create_nodes(char *filename, int nb_nodes) {
    FILE *file = fopen(filename, "r");
    if (!file) { last_error = FILE_NOT_FOUND; return NULL; }
    Node **nodes = malloc(sizeof(Node*) * nb_nodes);
    if (!nodes) { 
        fclose(file); 
        last_error = BAD_FILE_FORMAT; 
        return NULL; 
    }
    int fail_idx = read_and_create_nodes(file, nodes);
    fclose(file);
    if (fail_idx >= 0) {
        cleanup_nodes(nodes, fail_idx);
        last_error = BAD_FILE_FORMAT;
        return NULL;
    }
    return nodes;
}

// Recursively marks all nodes connected to the start node as visited
void mark_connected_nodes(Node* start) {
    if (start == NULL || start->visited == 1) return;
    start->visited = 1;
    for (int i = 0; i < start->link_count; i++)
        mark_connected_nodes(start->links[i]);
}

// Increments the link count for a node with the given ID
void increment_link_count(int *counts, Node **nodes, int nb, 
                          int id) {
    int idx = find_node_index(nodes, nb, id);
    if (idx != -1) counts[idx]++;
}

// Processes a link line and increments link counts for both nodes
void process_link_line(char *buffer, int *counts, Node **nodes, 
                       int nb) {
    int n1, n2;
    if (sscanf(buffer, "%d-%d", &n1, &n2) == 2) {
        increment_link_count(counts, nodes, nb, n1);
        increment_link_count(counts, nodes, nb, n2);
    }
}

// Counts how many links each node has from the file
int* count_links_filename(char *filename, Node **nodes, int nb) {
    FILE *f = fopen(filename, "r");
    if (!f) { last_error = FILE_NOT_FOUND; return NULL; }
    int *counts = calloc(nb, sizeof(int));
    if (!counts) { 
        fclose(f); 
        last_error = BAD_FILE_FORMAT; 
        return NULL; 
    }
    char buf[256];
    while (fgets(buf, sizeof(buf), f))
        if (strchr(buf, '-')) 
            process_link_line(buf, counts, nodes, nb);
    fclose(f);
    return counts;
}

// Allocates memory for each node's links array based on link counts
int allocate_links(Node **nodes, int *link_counts, int nb_nodes) {
    if (nodes == NULL || link_counts == NULL) {
        last_error = BAD_FILE_FORMAT;
        return -1;
    }
    for (int i = 0; i < nb_nodes; i++) {
        if (link_counts[i] > 0) {
            nodes[i]->links = malloc(sizeof(Node*) * link_counts[i]);
            if (!nodes[i]->links) {
                last_error = BAD_FILE_FORMAT;
                return -1;
            }
        }
        nodes[i]->link_count = 0;
    }
    return 0;
}

// Reads the file and fills in the bidirectional links between nodes
void fill_links(char *filename, Node **nodes, int nb_nodes) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {last_error = FILE_NOT_FOUND; return;}
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strchr(buffer, '-')) {
            int node1, node2;
            sscanf(buffer, "%d-%d", &node1, &node2);
            int idx1 = find_node_index(nodes, nb_nodes, node1);
            int idx2 = find_node_index(nodes, nb_nodes, node2);
            if (idx1 != -1 && idx2 != -1) {
                nodes[idx1]->links[nodes[idx1]->link_count++] = 
                    nodes[idx2];
                nodes[idx2]->links[nodes[idx2]->link_count++] = 
                    nodes[idx1];
            }
        }
    }
    fclose(file);
}

// Initializes the complete graph structure from the file
Node** init_node(char *filename) {
    last_error = 0;
    int nb_nodes = count_nodes_with_name(filename);
    if (nb_nodes <= 0) {
        if (last_error == 0){ last_error = BAD_FILE_FORMAT; return NULL;}
    }
    Node **nodes = create_nodes(filename, nb_nodes);
    if (nodes == NULL) return NULL;
    
    int *link_counts = count_links_filename(filename, nodes, nb_nodes);
    if (link_counts == NULL) { cleanup_nodes(nodes, nb_nodes); return NULL;}
    if (allocate_links(nodes, link_counts, nb_nodes) < 0) {
        cleanup_nodes(nodes, nb_nodes);
        free(link_counts);
        return NULL;
    }
    fill_links(filename, nodes, nb_nodes);
    free(link_counts);
    return nodes;
}

// Recursively displays all connected nodes starting from a given node
void display_nodes(Node* start) {
    if (start == NULL || start->visited == 1) return;
    start->visited = 1;
    printf("%d ", start->id);
    for (int i = 0; i < start->link_count; i++)
        display_nodes(start->links[i]);
}

// Returns an array of nodes not connected to the head node
Node** get_unconnected_nodes(Node **nodes, int size, Node *head) {
    mark_connected_nodes(head);
    int count = 0;
    Node **unconnected = malloc(sizeof(Node*) * (size + 1));
    if (!unconnected) {
        last_error = BAD_FILE_FORMAT;
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        if(nodes[i]->visited == 0) {
            unconnected[count] = nodes[i];
            count++;
        }
    }
    unconnected[count] = NULL;
    return unconnected;
}

// Prints all unconnected nodes to the console
void print_unconnected_nodes(Node **unconnected_nodes) {
    if (unconnected_nodes == NULL || unconnected_nodes[0] == NULL)
        return;
    printf("unconnected nodes:\n");
    int i = 0;
    while (unconnected_nodes[i] != NULL) {
        printf("%d ", unconnected_nodes[i]->id);
        i++;
    }
    printf("\n");
}

// Resets visited, distance, and parent fields for all nodes
void reset_nodes(Node **nodes, int size) {
    for (int i = 0; i < size; i++) {
        nodes[i]->visited = 0;
        nodes[i]->distance = 0;
        nodes[i]->parent = NULL;
    }
}

// Marks a neighbor node as visited and adds it to the BFS queue
void explore_neighbor(Node *neighbor, Node *parent, Node **file, 
                      int *tail) {
    neighbor->visited = 1;
    neighbor->parent = parent;
    neighbor->distance = parent->distance + 1;
    file[(*tail)++] = neighbor;
}

// Explores all unvisited neighbors of the current node in BFS
void breadth_first_search(Node *current, Node **file, int *tail) {
    for (int i = 0; i < current->link_count; i++) {
        if (current->links[i]->visited == 0) {
            explore_neighbor(current->links[i], current, file, tail);
        }
    }
}

// Validates that start and end nodes exist for pathfinding
int validate_path_nodes(Node *start, Node *end) {
    if (!start) { last_error = NO_START_NODE; return 0; }
    if (!end) { last_error = NO_END_NODE; return 0; }
    return 1;
}

// Finds the shortest path between start and end nodes using BFS
void find_shortest_path(Node *start, Node *end, int size) {
    if (!validate_path_nodes(start, end)) return;
    Node **file = malloc(sizeof(Node*) * size);
    if (!file) { last_error = BAD_FILE_FORMAT; return; }
    
    int head = 0, tail = 0;
    file[tail++] = start;
    start->visited = 1;
    start->distance = 0;
    
    while(head < tail) {
        Node *current = file[head++];
        if (current == end) break;
        breadth_first_search(current, file, &tail);
    }
    if (end->visited == 0) last_error = BAD_FILE_FORMAT;
    free(file);
}

// Prints the path array with formatting
void print_path_array(Node **path, int length) {
    printf("pathfinding:\n");
    for (int i = 0; i < length; i++) {
        printf("%d", path[i]->id);
        if (i < length - 1) printf(" ");
    }
    printf("\n");
}

// Reconstructs and prints the shortest path from start to end
void print_shortest_path(Node *start, Node *end) {
    if (end->parent == NULL && end != start) return;
    int path_length = 0;
    Node *current = end;
    while (current != NULL) {
        path_length++;
        current = current->parent;
    }
    Node **path = malloc(sizeof(Node*) * path_length);
    if (!path) { last_error = BAD_FILE_FORMAT; return;}
    current = end;
    int index = path_length - 1;
    while (current != NULL) {
        path[index--] = current;
        current = current->parent;
    }
    print_path_array(path, path_length);
    free(path);
}

// Prints the appropriate error message based on error code
void print_error(int error_code) {
    switch(error_code) {
        case FILE_NOT_FOUND:
            printf("FILE_NOT_FOUND\n");
            break;
        case NO_START_NODE:
            printf("NO_START_NODE\n");
            break;
        case NO_END_NODE:
            printf("NO_END_NODE\n");
            break;
        case BAD_FILE_FORMAT:
            printf("BAD_FILE_FORMAT\n");
            break;
        default:
            printf("UNKNOWN_ERROR\n");
            break;
    }
}