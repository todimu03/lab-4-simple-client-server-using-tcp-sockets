#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct node {
    int value;
    struct node *next;
} node_t;

struct list {
    node_t *head;
    int size;
};

list_t *list_alloc(void) {
    list_t *l = (list_t *)malloc(sizeof(list_t));
    if (!l) return NULL;
    l->head = NULL;
    l->size = 0;
    return l;
}

void list_free(list_t *l) {
    if (!l) return;
    node_t *cur = l->head;
    while (cur) {
        node_t *next = cur->next;
        free(cur);
        cur = next;
    }
    free(l);
}

int list_length(list_t *l) {
    if (!l) return 0;
    return l->size;
}

void list_print(list_t *l) {
    if (!l) return;
    node_t *cur = l->head;
    while (cur) {
        printf("%d ", cur->value);
        cur = cur->next;
    }
    printf("\n");
}

void list_add_to_front(list_t *l, int value) {
    if (!l) return;
    node_t *n = (node_t *)malloc(sizeof(node_t));
    n->value = value;
    n->next = l->head;
    l->head = n;
    l->size++;
}

void list_add_to_back(list_t *l, int value) {
    if (!l) return;
    node_t *n = (node_t *)malloc(sizeof(node_t));
    n->value = value;
    n->next = NULL;
    if (!l->head) {
        l->head = n;
    } else {
        node_t *cur = l->head;
        while (cur->next) cur = cur->next;
        cur->next = n;
    }
    l->size++;
}

void list_add_at_index(list_t *l, int index, int value) {
    if (!l) return;
    if (index <= 0) { list_add_to_front(l, value); return; }
    if (index >= l->size) { list_add_to_back(l, value); return; }

    node_t *cur = l->head;
    for (int i = 0; i < index - 1; i++) cur = cur->next;

    node_t *n = (node_t *)malloc(sizeof(node_t));
    n->value = value;
    n->next = cur->next;
    cur->next = n;
    l->size++;
}

int list_remove_from_front(list_t *l) {
    if (!l || !l->head) return -1;
    node_t *tmp = l->head;
    int v = tmp->value;
    l->head = tmp->next;
    free(tmp);
    l->size--;
    return v;
}

int list_remove_from_back(list_t *l) {
    if (!l || !l->head) return -1;
    if (!l->head->next) {
        int v = l->head->value;
        free(l->head);
        l->head = NULL;
        l->size--;
        return v;
    }
    node_t *cur = l->head;
    while (cur->next->next) cur = cur->next;
    int v = cur->next->value;
    free(cur->next);
    cur->next = NULL;
    l->size--;
    return v;
}

int list_remove_at_index(list_t *l, int index) {
    if (!l || !l->head) return -1;
    if (index <= 0) return list_remove_from_front(l);
    if (index >= l->size - 1) return list_remove_from_back(l);

    node_t *cur = l->head;
    for (int i = 0; i < index - 1; i++) cur = cur->next;

    node_t *tmp = cur->next;
    int v = tmp->value;
    cur->next = tmp->next;
    free(tmp);
    l->size--;
    return v;
}

int list_get_elem_at(list_t *l, int index) {
    if (!l || index < 0 || index >= l->size) return -1;
    node_t *cur = l->head;
    for (int i = 0; i < index; i++) cur = cur->next;
    return cur->value;
}

char *listToString(list_t *l) {
    static char buf[4096];
    buf[0] = '\0';
    if (!l) return buf;

    strcat(buf, "[");
    node_t *cur = l->head;
    while (cur) {
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%d", cur->value);
        strcat(buf, tmp);
        if (cur->next) strcat(buf, ", ");
        cur = cur->next;
    }
    strcat(buf, "]");
    return buf;
}
