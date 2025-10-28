typedef struct n
{
    int id;
    struct n **links;
    int link_count;
    int visited;
} Node;

int count_nodes_with_name(char *filename);
int count_links(int argc, char *argv[]);
int get_start_node(int argc, char *argv[]);
int get_end_node(int argc, char *argv[]);
int find_node_index(Node **nodes, int nb_nodes, int id);
Node **create_nodes(char *filename, int nb_nodes);
int *count_links_filename(char *filename, Node **nodes, int nb_nodes);
void allocate_links(Node **nodes, int *link_counts, int nb_nodes);
void fill_links(char *filename, Node **nodes, int nb_nodes);
Node **init_node(char *filename);
void display_nodes(Node *start);
Node **get_unconnected_nodes(Node **nodes, int size, Node *head);
void print_unconnected_nodes(Node **unconnected_nodes);