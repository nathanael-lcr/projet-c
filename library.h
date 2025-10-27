typedef struct n{
    int id;
    struct n **links;
} Node;

Node** init_node(char *filename);
int count_nodes_with_name(char *filename);
int count_nodes(int argc, char *argv[]);
int count_links(int argc, char *argv[]);
int get_start_node(int argc, char *argv[]);
int get_end_node(int argc, char *argv[]);