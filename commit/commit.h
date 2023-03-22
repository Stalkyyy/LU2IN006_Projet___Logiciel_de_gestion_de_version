#ifndef _COMMIT_H_

typedef struct key_value_pair {
    char *key;
    char *value;
} kvp;

typedef struct hash_table {
    kvp** T;
    int n;
    int size;
} HashTable;

typedef HashTable Commit;

#endif