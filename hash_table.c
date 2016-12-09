//
//  hash_table.c
//  LexialAnalyzer
//
//  Created by Damian Malarczyk on 15.11.2016.
//  Copyright © 2016 Damian Malarczyk. All rights reserved.
//

#include "hash_table.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

/**
 * hash computation http://www.azillionmonkeys.com/qed/hash.html
 **/
#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
+(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

size_t compute_hash (const char * data, size_t len) {
    size_t hash = len, tmp;
    int rem;
    
    if (data == NULL) return 0;
    
    rem = len & 3;
    len >>= 2;
    
    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }
    
    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
            hash ^= hash << 16;
            hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
            hash += hash >> 11;
            break;
        case 2: hash += get16bits (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1: hash += (signed char)*data;
            hash ^= hash << 10;
            hash += hash >> 1;
    }
    
    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    
    return hash;
}


/////////////////////////////////////////////////////////////

hashtable_entry* newEntry() {
    hashtable_entry* new = malloc(sizeof(hashtable_entry));
    new->key = NULL;
    new->value = NULL;
    new->next = NULL;
    return new;
}

hashtable* hashtable_init(int size, size_t elementSize, free_function fn, copy_function cp) {
    hashtable* new = malloc(sizeof(hashtable));
    new->size = size;
    new->elementSize = elementSize;
    new->fn = fn;
    new->cp = cp;
    new->table = calloc(size, sizeof(hashtable_entry*));
    return new;
}

void* hashtable_insert(hashtable* hash, const char* key, void* value) {
    int index = compute_hash(key, strlen(key)) % hash->size;
    
    bool isNew = true;

    hashtable_entry* entry = hash->table[index];
    void* oldValue = NULL;
    if (!entry) {
        entry = newEntry();
        hash->table[index] = entry;
    } else {
        hashtable_entry* previous = entry;
        while (entry) {
            if (!strcmp(entry->key, key)) {
                oldValue = entry->value;
                isNew = false;
                break;
            }
            previous = entry;
            entry = entry->next;
        }
        if (isNew) {
            previous->next = newEntry();
            entry = previous->next;
        } else {
            if (hash->fn) {
                hash->fn(entry->value);
                oldValue = NULL;
            }
        }
        
    }
    
    copy_function cp = hash->cp;
    if (cp) {
        entry->value = malloc(hash->elementSize);
        cp(value, entry->value);
    } else {
        entry->value = value;
    }
    
    if (isNew) {
        char* keyCopy = strdup(key);
        
        if (!keyCopy) {
            printf("Hash error, not enough memory");
            exit(1);
        }
        entry->key = keyCopy;
    }
    
    
    return oldValue;
}


void* hashtable_get_entry(hashtable* hash, const char* key, bool remove) {
    int index = compute_hash(key, strlen(key)) % hash->size;
    hashtable_entry* entry = hash->table[index];
    hashtable_entry* previous = entry;
    while (entry) {
        if (!strcmp(entry->key, key)) {
            if (remove) {
                void* val = entry->value;
                if (previous != entry) {
                    if (entry->next) {
                        previous->next = entry;
                    } else {
                        previous->next = NULL;
                    }
                    
                } else {
                    if (entry->next) {
                        hash->table[index] = entry->next;
                    } else {
                        hash->table[index] = NULL;
                    }
                }
                free_function fn = hash->fn;
                void* value = entry->value;
                if (fn && value) {
                    fn(value);
                    
                }
                free(entry->key);
                free(entry);

                return val;
            }
            return entry->value;
        }
        previous = entry;
        entry = entry->next;
    }
    return NULL;
}

void hashtable_iterator(hashtable* hash, void* user_data,void (*iterator)(hashtable*, hashtable_entry*, void* user_data)) {
    hashtable_entry* entry = NULL;
    for (int i = 0; i < hash->size; i++) {
        entry = hash->table[i];
        
        while (entry) {
            iterator(hash, entry, user_data);

            entry = entry->next;
        }
    }
    
}

list* hashtable_values(hashtable* hash) {
    list* result = list_init(hash->elementSize, NULL, NULL, true);
    void* value = NULL;
    hashtable_entry* entry = NULL;
    for (int i = 0; i < hash->size; i++) {
        entry = hash->table[i];
        
        while (entry) {
            value = entry->value;
            if (value) {
                list_auto_append(result, value);
            }
            entry = entry->next;
        }
    }
    return result;
}

void* hashtable_value(hashtable* hash, const char* key) {
    return hashtable_get_entry(hash, key, false);
}

void* hashtable_remove(hashtable* hash, const char* key) {
    return hashtable_get_entry(hash, key, true);
}

void hashtable_freeEntries(hashtable* hash, hashtable_entry* entry) {
    if (entry->next) {
        hashtable_freeEntries(hash, entry->next);
    }
    free_function fn = hash->fn;
    void* value = entry->value;
    
    if (fn && value) {
        fn(value);
    }
    
    free(entry->key);
    free(entry);
}

void hashtable_cleanup(hashtable* hash) {
    for (int i = 0; i < hash->size; i++) {
        hashtable_entry* entry = hash->table[i];
        if (entry) {
            hashtable_freeEntries(hash,entry);
        }
        hash->table[i] = NULL;
    }
}

void hashtable_free(hashtable* hash) {
    hashtable_cleanup(hash);
    free(hash->table);
    free(hash);
}
