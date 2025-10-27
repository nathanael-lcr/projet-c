typedef struct n{
    int id;
    struct n **links;
} Node;

char* open_read_file(int argc, char *argv[], char *buffer);

int count_nodes(int argc, char *argv[]);
int count_links(int argc, char *argv[]);
int get_start_node(int argc, char *argv[]);
int get_end_node(int argc, char *argv[]);