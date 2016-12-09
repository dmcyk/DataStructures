//
//  hash_table.h
//  LexialAnalyzer
//
//  Created by Damian Malarczyk on 15.11.2016.
//  Copyright © 2016 Damian Malarczyk. All rights reserved.
//

#ifndef hash_table_h
#define hash_table_h

#include <stdio.h>
#include "list.h"
#include "utils.h"

typedef struct _hash_element {
    char* key;
    void* value;
    struct _hash_element* next;
} hashtable_entry;

typedef struct {
    int size;
    hashtable_entry** table;
    free_function fn;
    copy_function cp;
    size_t elementSize;
} hashtable;

// having free - copy functions all values will be copied and automatically removed during function
// having only copy function, using hashtable_free will not free up memory, one should use hashtable_values in such case and handle memory management on his/her own
// having only free function will result in automatic memory clearing for values passed to the table - in such case one can pass values to hashtable and doesn't have to bother about freeing them later one - of course trying to access such memory after using hashtable_free will result in crashes/errors  
hashtable* hashtable_init(int size, size_t elementSize, free_function fn, copy_function cp);
list* hashtable_values(hashtable* hash);
void* hashtable_value(hashtable* hash, const char* key);
void* hashtable_insert(hashtable* hash, const char* key, void* value);

void* hashtable_remove(hashtable* hash, const char* key);
void hashtable_cleanup(hashtable* hash);
void hashtable_iterator(hashtable* hash, void* user_data,void (*iterator)(hashtable*, hashtable_entry*, void*));

// if freeFunction of hashtable is null the memory wont be freed, thus one shall use hashtable_values to get list of all the values
// and take care of their memory management 
void hashtable_free(hashtable* hash);
#endif /* hash_table_h */
