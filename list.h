//
//  list.h
//  server
//
//  Created by Damian Malarczyk on 10.04.2016.
//  Copyright © 2016 Damian Malarczyk. All rights reserved.
//

#ifndef list_h
#define list_h

#include <stdio.h>
#include "utils.h"

typedef struct _list_node {
    void* data;
    int key;
    struct _list_node* next;
    struct _list_node* previous;
} list_node;

typedef struct _list {
    size_t element_size;
    free_function free_fn;
    copy_function cp_fn;
    list_node* first;
    int highest_key;
    bool auto_increment;
} list;

list* list_init(size_t element_size, free_function, copy_function, bool auto_increment);
void list_auto_append(list* head, void* element);
void list_append(list* head,void* element,int key);
list_node* list_find(list *head,int key);
void list_remove_all(list* head);
void list_remove(list* head,list_node* toDelete);
void list_free(list* head);
void list_remove_by_key(list *head,int key);
void list_iterator(list* head, void* user_data, void (*iterator)(list*,list_node*, void* user_data));

list* string_list();
void print_string_list(list* head);
#endif /* list_h */
