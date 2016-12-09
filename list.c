//
//  list.c
//  LexialAnalyzer
//
//  Created by Damian Malarczyk on 10.04.2016.
//  Copyright © 2016 Damian Malarczyk. All rights reserved.
//

#include "list.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

list* list_init(size_t element_size, free_function fn, copy_function cp,bool auto_increment){
    
    list* head = malloc(sizeof(list));
    head->element_size = element_size;
    head->first = NULL;
    head->highest_key = -1;
    head->auto_increment = auto_increment;
    head->cp_fn = cp;
    head->free_fn = fn;
    return head;
}

void list_free(list* head) {
    list_remove_all(head);
    free(head);
}

bool _addObject(list *head,list_node* new){
    
    list_node* current = head->first;
    int key = new->key;
    
    if (current == NULL){
        //pierwszy element list
        new->next = new;
        new->previous = new;
        head->first = new;
        return true;
    }
    
    list_node* last = current->previous;
    
    if (key > last->key){
        //nowy klucz jest najwiekszy
        last->next = new;
        new->next = head->first;
        new->previous = head->first->previous;
        head->first->previous = new;
        return true;
    }
    
    
    do{
        if (key == current->key) {
            printf("nie mozna dodac elementu, klucz juz istnieje\n");
            return false; //taki klucz juz istnieje
        }else if (key < current->key){
            
            new->previous = current->previous;
            new->next = current;
            current->previous->next = new;
            current->previous = new;
            
            if (current == head->first) {
                head->first = new;
                
            }
            return true;
        }
        current = current->next;
    }   while (current != head->first);
    return true;
}

void list_auto_append(list* head, void* element) {
#if DEBUG
    assert(head->auto_increment);
#endif
    int newKey = ++head->highest_key;
    list_node* new = malloc(sizeof(list_node));
    new->key = newKey;
    
    if (_addObject(head, new)) {
        if (element && head->cp_fn) {
            new->data = malloc(head->element_size);
            head->cp_fn(element,new->data);
        } else {
            new->data = element;
        }
    } else {
        free(new);
    }
}

void list_append(list* head,void* element,int key) {
#if DEBUG
    assert(!head->auto_increment);
#endif
    list_node* new = malloc(sizeof(list_node));
    new->key = key;
    if (element && head->cp_fn) {
        new->data = malloc(head->element_size);
        head->cp_fn(element,new->data);
    } else {
        new->data = element;
    }
    
    
    _addObject(head, new);
}

list_node* list_find(list *head,int key){
    list_node* first = head->first;
    
    if (first != NULL){
        if (key < first->key || key > first->previous->key)
            return NULL;
        
        //jesli szuakny klucz jest wiekszy niz najwiekszy dzielony przez 2 (~srodkowy) to zacznij szukac od konca
        //w przeciwnym razie szukaj od poczatku
        //** efektywniejsze wyszukiwanie **
        int b = first->previous->key - key;
        
        if (b < first->previous->key / 2){
            list_node* current = first->previous;
            while (current->key >= first->previous->key / 2) {
                if (key == current->key)
                    return current;
                current = current->previous;
            }
            
        }else {
            list_node* current = first;
            while (current->key <= (first->previous->key / 2)) {
                if (key == current->key)
                    return current;
                current = current->next;
            }
        }
    }
    
    return NULL;
    
}
void list_iterator(list* head, void* user_data, void (*iterator)(list*,list_node*, void* user_data)) {
    if (iterator) {
        list_node* current = head->first;
        list_node* next = 0;
        if (current) {
            do {
                next = current->next;
                iterator(head,current, user_data);
                current = next;
            } while(current && current != head->first);
        }
        
    }
}

void list_remove_all(list* head){
    if (!head->first) {
        return;
    }
    list_node* current = head->first->next;
    list_node* next = NULL;
    while (current != head->first) {
        next = current->next;
        current->next = NULL;
        current->previous = NULL;
        if (head->free_fn) {
            head->free_fn(current->data);
        }
        free(current);
        current = next;
        
    }
    current = head->first;
    current->next = NULL;
    current->previous = NULL;
    if (head->free_fn) {
        head->free_fn(current->data);
    }
    free(current);
    current = NULL;
    head->first = NULL;
}

void list_remove(list* head,list_node* toDelete) {
    if (toDelete == toDelete->next) {
        head->first = NULL;
    } else {
        toDelete->previous->next = toDelete->next;
        toDelete->next->previous = toDelete->previous;
        if (head->first == toDelete) {
            head->first = toDelete->next;
        }
    }
    
    
    if (head->free_fn)
        head->free_fn(toDelete->data);
    free(toDelete);
    toDelete = NULL;
}

void list_remove_by_key(list *head,int key){
    list_node* toDelete = list_find(head, key);
    
    if (toDelete != NULL){
        list_remove(head,toDelete);
    }else {
        printf("Nie ma elementu z takim kluczem\n");
    }
}


//////////////////

void free_string(void* var) {
    char** ptr = (char**) var;
    free(*ptr);
    free(ptr);
}

void cp_string(void* src, void* dst) {
    char* s_str = src;
    char** s_dst = (char**)dst;
    *s_dst = strdup(s_str);
}
list* string_list() {
    return list_init(sizeof(char*), free_string, cp_string, true);
}

void print_string(list* head, list_node* var, void* user_data) {
    printf("\t%s\n", *(char**)var->data);
}

void print_string_list(list* head) {
    list_iterator(head, NULL, print_string);
}
