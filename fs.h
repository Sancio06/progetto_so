#ifndef FS_H
#define FS_H

#define MAX_NAME 32
#define MAX_INODES 128
#define MAX_CHILDREN 16
#define MAX_FILE_SIZE 256

typedef struct {
    int magic;       
    int size;
    int root_inode;
} Superblock;


typedef struct {
    char name[MAX_NAME];
    int is_dir;      
    int parent;      

    int children[MAX_CHILDREN];
    int num_children;

    char data[MAX_FILE_SIZE]; 
    int size;                 
} Inode;


typedef struct {
    Superblock sb;
    Inode inodes[MAX_INODES];
} FileSystem;

void fs_init(void *disk);
void fs_mkdir(const char* name);
void fs_ls(const char* path);
void fs_touch(const char* name);
void fs_append(const char* name, const char* text);
void fs_cat(const char* name);
void fs_cd(const char* name);
void fs_rm(const char* name);
int find_free_inode();

#endif