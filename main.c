#include <stdio.h>
#include <stdlib.h>
#include "library.h"

int main(int argc, char *argv[])
{

    printf("nodes: %d\n", count_nodes(argc, argv));
    printf("links: %d\n", count_links(argc, argv));
    printf("start: %d\n", get_start_node(argc, argv));
    printf("end: %d\n", get_end_node(argc, argv));
    // printf("%d", count_nodes(contenu));
    return 0;
}