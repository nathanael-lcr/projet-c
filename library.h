#include <stdio.h>

typedef struct n
{
    int id;
    struct n **links;
    int link_count;
    int visited;
    int distance;
    struct n *parent;
} Node;

typedef enum
{
    FILE_NOT_FOUND = 1,
    NO_START_NODE = 2,
    NO_END_NODE = 3,
    BAD_FILE_FORMAT = 4,
} Error;

extern int last_error;

// File parsing functions
int count_nodes_with_name(char *filename);
int count_links(int argc, char *argv[]);
int is_node_number(char *buffer);
int get_node_after_tag(int argc, char *argv[], char *tag, int error);
int get_start_node(int argc, char *argv[]);
int get_end_node(int argc, char *argv[]);

// Node index and initialization
int find_node_index(Node **nodes, int nb_nodes, int id);
void init_node_struct(Node *node, int id);
void cleanup_nodes(Node **nodes, int count);

// Node creation functions
Node *create_single_node(int id);
int read_and_create_nodes(FILE *file, Node **nodes);
Node **create_nodes(char *filename, int nb_nodes);

// Graph connection marking
void mark_connected_nodes(Node *start);

// Link counting functions
void increment_link_count(int *counts, Node **nodes, int nb, int id);
void process_link_line(char *buffer, int *counts, Node **nodes, int nb);
int *count_links_filename(char *filename, Node **nodes, int nb);

// Link allocation and filling
int allocate_links(Node **nodes, int *link_counts, int nb_nodes);
void fill_links(char *filename, Node **nodes, int nb_nodes);

// Main initialization
Node **init_node(char *filename);

// Display functions
void display_nodes(Node *start);

// Unconnected nodes
Node **get_unconnected_nodes(Node **nodes, int size, Node *head);
void print_unconnected_nodes(Node **unconnected_nodes);

// Reset function
void reset_nodes(Node **nodes, int size);

// Pathfinding functions
void explore_neighbor(Node *neighbor, Node *parent, Node **file, 
                      int *tail);
void breadth_first_search(Node *current, Node **file, int *tail);
int validate_path_nodes(Node *start, Node *end);
void find_shortest_path(Node *start, Node *end, int size);

// Path printing
void print_path_array(Node **path, int length);
void print_shortest_path(Node *start, Node *end);

// Error handling
void print_error(int error_code);

