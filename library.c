#include "library.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




/* Alors pour commencer tu peux aller regarder en bas le init_node car c'est
à partir de lui que beaucoup des nouvelles fonctions sonts appelées

 */













int count_nodes_with_name(char *filename){
    if (filename == NULL) return 1;
    int number_nodes = 0;
    char buffer[256];
    FILE *file = fopen(filename, "r");
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (buffer[0] >= '0' && buffer[0] <= '9' &&
            strchr(buffer, '-') == NULL)
        {
            number_nodes++;
        }
    }
    fclose(file);
    return number_nodes;
}

int count_links(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL) return 1;
    int number_links = 0;
    char buffer[256];
    FILE *file = fopen(argv[1], "r");
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (buffer[0] >= '0' && buffer[0] <= '9' &&
            strchr(buffer, '-') != NULL)
        {
            number_links++;
        }
    }   
    fclose(file);
    return number_links;
}

int get_start_node(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL) return 1;
    char buffer[256];
    int next_start = 0;
    FILE *file = fopen(argv[1], "r");
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, "#start") == 0)
            next_start = 1;
        if (next_start && buffer[0] >= '0' &&
            buffer[0] <= '9' && strchr(buffer, '-') == NULL)
        {
            fclose(file);
            return atoi(buffer);
        }
    }
    fclose(file);
    return 0;
}

int get_end_node(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL) return 1;
    char buffer[256];
    int next_end = 0;
    FILE *file = fopen(argv[1], "r");
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, "#end") == 0)
            next_end = 1;
        if (next_end && buffer[0] >= '0' && buffer[0] <= '9' &&
            strchr(buffer, '-') == NULL)
        {
            fclose(file);
            return atoi(buffer);
        }
    }
    fclose(file);
    return 0;
}


int find_node_index(Node **nodes, int nb_nodes, int id)
{
    for (int i = 0; i < nb_nodes; i++)
        if (nodes[i]->id == id)
            return i;
    return -1;
}

Node** create_nodes(char *filename, int nb_nodes)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) return NULL;
    
    Node **nodes = malloc(sizeof(Node*) * nb_nodes);
    char buffer[256];
    int index = 0;
    
    while (fgets(buffer, sizeof(buffer), file))
        if (buffer[0] >= '0' && buffer[0] <= '9' && !strchr(buffer, '-')) {
            nodes[index] = malloc(sizeof(Node));
            nodes[index]->id = atoi(buffer);
            nodes[index]->links = NULL;
            nodes[index]->link_count = 0;
            nodes[index]->visited = 0;
            index++;
        }
    
    fclose(file);
    return nodes;
}

int* count_links_filename(char *filename, Node **nodes, int nb_nodes)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) return NULL;
    
    int *link_counts = calloc(nb_nodes, sizeof(int));
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), file))
        if (strchr(buffer, '-')) {
            int node1, node2;
            sscanf(buffer, "%d-%d", &node1, &node2);
            int nodeindex1 = find_node_index(nodes, nb_nodes, node1);
            int nodeindex2 = find_node_index(nodes, nb_nodes, node2);
            if (nodeindex1 != -1) link_counts[nodeindex1]++;
            if (nodeindex2 != -1) link_counts[nodeindex2]++;
        }
    
    fclose(file);
    return link_counts;
}

void allocate_links(Node **nodes, int *link_counts, int nb_nodes)
{
    for (int i = 0; i < nb_nodes; i++) {
        nodes[i]->links = malloc(sizeof(Node*) * link_counts[i]);
        nodes[i]->link_count = 0;
    }
}

void fill_links(char *filename, Node **nodes, int nb_nodes)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) return;
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file))
        if (strchr(buffer, '-')) {
            int node1, node2;
            sscanf(buffer, "%d-%d", &node1, &node2);
            int nodeidx1 = find_node_index(nodes, nb_nodes, node1);
            int nodeidx2 = find_node_index(nodes, nb_nodes, node2);
            
            if (nodeidx1 != -1 && nodeidx2 != -1) {
                nodes[nodeidx1]->links[nodes[nodeidx1]->link_count++] = nodes[nodeidx2];
                nodes[nodeidx2]->links[nodes[nodeidx2]->link_count++] = nodes[nodeidx1];
            }
        }
    
    fclose(file);
}

Node** init_node(char *filename)
{
    //On compte simplement le nombre de nodes dans le fichier
    int nb_nodes = count_nodes_with_name(filename);
    //On crée des nodes (les structs les vrais) pour chaque node lu
    //(pas besoin d'aller regarder la fonction c'est juste une boucle qui crée des nodes)
    Node **nodes = create_nodes(filename, nb_nodes);
    if (nodes == NULL) return NULL;
    
    //On relit le fichier, ligne par ligne
    //Pour chaque ligne avec un `-` (comme `3-4`), on parse les deux nombres
    //On trouve les index des nodes concernés et on incrémente leurs compteurs
    int *link_counts = count_links_filename(filename, nodes, nb_nodes);
    //Par rapport au nombre de liens vu avant on allocate la mémoire
    allocate_links(nodes, link_counts, nb_nodes);
    //On finis par remplir les liens et donc les faire pointer les uns vers les autres
    fill_links(filename, nodes, nb_nodes);
    
    //Toujours libérer la mémoire
    free(link_counts);
    return nodes;
}

//Ca je t'explique pas c'est toi qui l'as fais

//voici mes codes :3
void display_nodes(Node* start) {
    if (start == NULL) return;
    if (start->visited == 1) return;//évite les doublons
    
    start->visited = 1;//marque comme visité
    printf("%d ", start->id);
    //Parcourt récursivement tous les nœuds connectés au nœud start
    for (int i = 0; i < start->link_count; i++) {
        display_nodes(start->links[i]);
    }
}

Node** get_unconnected_nodes( Node **nodes, int size, Node *head ){
    display_nodes(head);//marque tous les nœuds accessibles depuis head avec visited = 1
    int count=0;
    Node **unconnected_nodes=malloc(sizeof(Node*)*size);
    for (int i=0; i<size;i++){
        if(nodes[i]->visited == 0){
            unconnected_nodes[count]=nodes[i];
            count++;//nœuds nn connecter ds count
        }
    }
    unconnected_nodes[count] = NULL; //marquer la fin
    return unconnected_nodes;
    free(unconnected_nodes);
}