#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"


int main(int argc, char *argv[])
{

    /* Je met ce petit message ici :
    Il faut qu'on discute de la question même du display, 
    fais y moi penser en me rappelant le résultats de ./pathfinding test1.txt */

    //Bon ca c'est nos petits trucs pour calculer le nombre de nodes
    printf("nodes: %d\n", count_nodes_with_name(argv[1]));
    printf("links: %d\n", count_links(argc, argv)); //Nombre de Links
    printf("start: %d\n", get_start_node(argc, argv)); //Node de départ
    printf("end: %d\n", get_end_node(argc, argv)); //Node d'arrivée

    //Cela appelle init_node et donc toutes les nouvelles étapes que j'ai expliqué
    //dans library.c
    Node **nodes = init_node(argv[1]);
    
    //Ensuite ici on récupère l'ID du node qui suit #start exemple : 3
    int start_id = get_start_node(argc, argv);
    
    //Puis on itère pour savoir si node[0]->id puis node[1]->id etc == 3
    Node *start_node = NULL;
    //int size = count_nodes_with_name(argv[1]);
    //Node *head = NULL;
    for (int i = 0; i < count_nodes_with_name(argv[1]); i++) {
        if (nodes[i]->id == start_id) {
            start_node = nodes[i];
            break;
        }
    }
    //Puis on appelle ta sublime fonction pour display et paf
    display_nodes(start_node);
    printf("\n");
    Node **unconnected = get_unconnected_nodes(nodes, count_nodes_with_name(argv[1]), start_node);
    print_unconnected_nodes(unconnected);
    
    free(unconnected);
    printf("\n");
    return 0;
}