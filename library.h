typedef struct n{
    int id;
    struct n **links;
    int link_count;
    int visited;
} Node;

Node** init_node(char *filename);
int count_nodes_with_name(char *filename);
int count_links(int argc, char *argv[]);
int get_start_node(int argc, char *argv[]);
int get_end_node(int argc, char *argv[]);
void display_nodes(Node* start);
Node** get_unconnected_nodes( Node **nodes, int size, Node *head );