#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

int validate_args(int argc) {
    if (argc < 2) {
        print_error(BAD_FILE_FORMAT);
        return BAD_FILE_FORMAT;
    }
    return 0;
}

int load_basic_info(int argc, char *argv[], int *nb_nodes, int *start_id, int *end_id) {
    *nb_nodes = count_nodes_with_name(argv[1]);
    if (*nb_nodes <= 0) {
        return last_error ?: BAD_FILE_FORMAT;
    }
    *start_id = get_start_node(argc, argv);
    if (*start_id < 0) return last_error;
    *end_id = get_end_node(argc, argv);
    if (*end_id < 0) return last_error;
    return 0;
}

Node **load_nodes(char *filename) {
    Node **nodes = init_node(filename);
    if (!nodes) print_error(last_error);
    return nodes;
}

void find_node_ptrs(Node **nodes, int nb_nodes, int start_id, int end_id, Node **start_node, Node **end_node) {
    for (int i = 0; i < nb_nodes; i++) {
        if (nodes[i]->id == start_id) *start_node = nodes[i];
        if (nodes[i]->id == end_id) *end_node = nodes[i];
    }
}

int safe_pathfind(Node *start_node, Node *end_node, int nb_nodes, Node **nodes) {
    find_shortest_path(start_node, end_node, nb_nodes);
    if (last_error != 0) {
        for (int i = 0; i < nb_nodes; i++) {
            if (nodes[i]->links) free(nodes[i]->links);
            free(nodes[i]);
        }
        free(nodes);
        return last_error;
    }
    return 0;
}

void show_info(int argc, char *argv[], int nb_nodes, int start_id, int end_id) {
    printf("nodes: %d\n", nb_nodes);
    printf("links: %d\n", count_links(argc, argv));
    printf("start: %d\n", start_id);
    printf("end: %d\n", end_id);
}

void process_unconnected(Node **nodes, int nb_nodes, Node *start_node) {
    reset_nodes(nodes, nb_nodes);
    Node **unconnected = get_unconnected_nodes(nodes, nb_nodes, start_node);
    print_unconnected_nodes(unconnected);
    free(unconnected);
}

void final_path(Node **nodes, int nb_nodes, Node *start_node, Node *end_node) {
    reset_nodes(nodes, nb_nodes);
    find_shortest_path(start_node, end_node, nb_nodes);
    print_shortest_path(start_node, end_node);
}

void cleanup(Node **nodes, int nb_nodes) {
    for (int i = 0; i < nb_nodes; i++) {
        if (nodes[i]->links) free(nodes[i]->links);
        free(nodes[i]);
    }
    free(nodes);
}

int handle_errors(int code) {
    if (code) print_error(code);
    return code;
}

int main(int argc, char *argv[]) {
    int nb_nodes, start_id, end_id;
    int ret = validate_args(argc);
    if (handle_errors(ret)) return ret;

    ret = load_basic_info(argc, argv, &nb_nodes, &start_id, &end_id);
    if (handle_errors(ret)) return ret;

    Node **nodes = load_nodes(argv[1]);
    if (!nodes) return handle_errors(last_error);

    Node *start_node = NULL, *end_node = NULL;
    find_node_ptrs(nodes, nb_nodes, start_id, end_id, &start_node, &end_node);

    ret = safe_pathfind(start_node, end_node, nb_nodes, nodes);
    if (handle_errors(ret)) return ret;

    show_info(argc, argv, nb_nodes, start_id, end_id);
    process_unconnected(nodes, nb_nodes, start_node);
    final_path(nodes, nb_nodes, start_node, end_node);
    cleanup(nodes, nb_nodes);
    return 0;
}
