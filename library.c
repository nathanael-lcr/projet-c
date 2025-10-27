#include "library.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int count_nodes(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL) return 1;
    int number_nodes = 0;
    char buffer[256];
    FILE *file = fopen(argv[1], "r");
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


Node** init_node(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) return NULL;   
    char buffer[256];
    int node_count = count_nodes_with_name(filename);
    Node **nodes = malloc(sizeof(Node*) * node_count);
    int *link_counts = calloc(node_count, sizeof(int));
    rewind(file);
    int index = 0;
    while (fgets(buffer, sizeof(buffer), file))
        if (buffer[0] >= '0' && buffer[0] <= '9' && !strchr(buffer, '-')) {
            nodes[index] = malloc(sizeof(Node));
            nodes[index]->id = atoi(buffer);
            nodes[index++]->links = NULL;
        }

    rewind(file);   
    while (fgets(buffer, sizeof(buffer), file))
        if (strchr(buffer, '-')) {
            int node1, node2;
            sscanf(buffer, "%d-%d", &node1, &node2);
            for (int i = 0; i < node_count; i++)
                if (nodes[i]->id == node1 || nodes[i]->id == node2)
                    link_counts[i]++;
        }  
    /* for (int i = 0; i < count_nodes_with_name(filename); i++)
    {
        printf("Node %d a %d liens\n", nodes[i]->id, link_counts[i]);
    } */
    free(link_counts);
    fclose(file);
    return nodes;
}

void display_nodes(Node* start){
    if (start==NULL){
        return ;}
    if (start->visited==1){
        return ;}
    start->visited = 1; 
    printf("%d ", start->id);
    for (int i=0;i<start->link_count;i++){
        display_nodes(start->links[i]);
    }
}   