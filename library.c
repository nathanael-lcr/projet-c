#include "library.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *open_read_file(int argc, char *argv[], char *buffer)
{
    if (argc < 2 || argv[1] == NULL)
        return NULL;

    FILE *fichier = fopen(argv[1], "r");
    if (fichier == NULL)
        return NULL;

    // Lire tout le contenu du fichier
    size_t len = fread(buffer, 1, 255, fichier);
    buffer[len] = '\0'; // fin de chaîne

    fclose(fichier);
    return buffer;
}

int count_nodes(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL)
        return 1;
    int number_nodes = 0;
    char buffer[256];
    FILE *file = fopen(argv[1], "r");
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (buffer[0] >= '0' && buffer[0] <= '9' && strchr(buffer, '-') == NULL)
        {
            number_nodes++;
        }
    }
    fclose(file);
    return number_nodes;
}

int count_links(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL)
        return 1;
    int number_links = 0;
    char buffer[256];
    FILE *file = fopen(argv[1], "r");
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (buffer[0] >= '0' && buffer[0] <= '9' && strchr(buffer, '-') != NULL)
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
        if (strcmp(buffer, "#start") == 0) next_start = 1;
        if (next_start && buffer[0] >= '0' && buffer[0] <= '9' && strchr(buffer, '-') == NULL)
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
        if (strcmp(buffer, "#end") == 0) next_end = 1;
        if (next_end && buffer[0] >= '0' && buffer[0] <= '9' && strchr(buffer, '-') == NULL)
        {
            fclose(file);
            return atoi(buffer);
        }
    }
    fclose(file);
    return 0;
}
