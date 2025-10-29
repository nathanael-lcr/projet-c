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
    
    Node **nodes = init_node(argv[1]);
    int nb_nodes = count_nodes_with_name(argv[1]);
    
    printf("\nNodes créés:\n");
    for (int i = 0; i < nb_nodes; i++) {
        printf("nodes[%d] -> id=%d, links=%d\n", i, nodes[i]->id, nodes[i]->link_count);
    }
    printf("=============\n\n");

    //Bon ca c'est nos petits trucs pour calculer le nombre de nodes
    printf("nodes: %d\n", count_nodes_with_name(argv[1]));
    printf("links: %d\n", count_links(argc, argv)); //Nombre de Links
    printf("start: %d\n", get_start_node(argc, argv)); //Node de départ
    printf("end: %d\n", get_end_node(argc, argv)); //Node d'arrivée
    
    //Ensuite ici on récupère l'ID du node qui suit #start exemple : 3
    int start_id = get_start_node(argc, argv);
    int end_id = get_end_node(argc, argv);
    
    Node *start_node = NULL;
    Node *end_node = NULL;
    
    for (int i = 0; i < nb_nodes; i++) {
        if (nodes[i]->id == start_id) start_node = nodes[i];
        if (nodes[i]->id == end_id) end_node = nodes[i];
    }
    
    //Puis on appelle ta sublime fonction pour display et paf
    display_nodes(start_node);
    printf("\n");
    Node **unconnected = get_unconnected_nodes(nodes, count_nodes_with_name(argv[1]), start_node);
    print_unconnected_nodes(unconnected);

    reset_nodes(nodes, nb_nodes);

    // Lancer l'algorithme
    find_shortest_path(start_node, end_node, nb_nodes);
    
    // Afficher le résultat
    print_shortest_path(start_node, end_node);
    
    free(unconnected);
    printf("\n");
    return 0;
}