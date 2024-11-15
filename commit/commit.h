#ifndef _COMMIT_H_

#define MAX_SIZE_COMMIT 100

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

//=====================================================================

kvp* createKeyVal(char *key, char *val);
void freeKeyVal(kvp *kv);
char* kvts(kvp *k);
kvp* stkv(char *str);

//=====================================================================

Commit* initCommit();
void freeCommit(Commit *c); //Rajout personnel.
unsigned long hash_sdbm(char *str);
void commitSet(Commit *c, char *key, char *value);
void commitSet_KV(Commit *c, kvp *kv); //Rajout personnel.
Commit* createCommit(char *hash);
char* commitGet(Commit *c, char *key);
char* cts(Commit *c);
Commit* stc(char *ch);
void ctf(Commit *c, char *file);
Commit* ftc(char *file);
char* blobCommit(Commit *c);

//=====================================================================

void initRefs();
void createUpdateRef(char *ref_name, char *hash);
void deleteRef(char *ref_name);
char* getRef(char *ref_name);

//=====================================================================

void myGitAdd(char *file_or_folder);
void myGitCommit(char *branch_name, char *message);

#endif