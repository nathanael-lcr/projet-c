#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"


int main(int argc, char *argv[])
{

    printf("=== DEBUG ===\n");
    printf("Nodes count: %d\n", count_nodes_with_name(argv[1]));
    printf("Start node: %d\n", get_start_node(argc, argv));
    printf("End node: %d\n", get_end_node(argc, argv));
    //Calls init_node() to build the full graph structure
    Node **nodes = init_node(argv[1]);
    int nb_nodes = count_nodes_with_name(argv[1]);
    
    printf("\nNodes créés:\n");
    for (int i = 0; i < nb_nodes; i++) {
        printf("nodes[%d] -> id=%d, links=%d\n", i, nodes[i]->id, nodes[i]->link_count);
    }
    printf("=============\n\n");

    printf("nodes: %d\n", count_nodes_with_name(argv[1]));
    printf("links: %d\n", count_links(argc, argv)); //Number of Links
    printf("start: %d\n", get_start_node(argc, argv)); //Starting node
    printf("end: %d\n", get_end_node(argc, argv)); //Ending node
    
    //Next, here we retrieve the ID of the node that follows #start example: 3
    int start_id = get_start_node(argc, argv);
    int end_id = get_end_node(argc, argv);
    
    Node *start_node = NULL;
    Node *end_node = NULL;
    // Convert numeric IDs into actual Node* references for traversal.
    for (int i = 0; i < nb_nodes; i++) {
        if (nodes[i]->id == start_id) start_node = nodes[i];
        if (nodes[i]->id == end_id) end_node = nodes[i];
    }
    
    display_nodes(start_node);
    printf("\n");
    //Detect and display isolated nodes in the graph.
    Node **unconnected = get_unconnected_nodes(nodes, count_nodes_with_name(argv[1]), start_node);
    print_unconnected_nodes(unconnected);

    reset_nodes(nodes, nb_nodes);

    // Launch the algorithm
    find_shortest_path(start_node, end_node, nb_nodes);
    
    // Show result
    print_shortest_path(start_node, end_node);
    
    free(unconnected);
    printf("\n");
    return 0;
}