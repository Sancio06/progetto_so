#include "fs.h"
#include "disk.h"
#include <string.h>
#include <stdio.h>

static FileSystem* fs = NULL;
static int current_dir = 0;
Inode* resolve_path(const char* path);
Inode* get_or_create_dir(char* path);
void split_path(const char* path, char* dir, char* name);

Inode* resolve_path(const char* path) {

    if (!fs || !path)
        return NULL;

    int current;

    if (path[0] == '/') {
        current = 0;
        path++;
    } else {
        current = current_dir;
    }

    char temp[256];
    strncpy(temp, path, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char* token = strtok(temp, "/");

    while (token != NULL) {

        Inode* dir = &fs->inodes[current];

        if (!dir->is_dir)
            return NULL;

        int found = -1;

        for (int i = 0; i < dir->num_children; i++) {

            int idx = dir->children[i];
            Inode* child = &fs->inodes[idx];

            if (strcmp(child->name, token) == 0) {
                found = idx;
                break;
            }
        }

        if (found == -1)
            return NULL;

        current = found;

        token = strtok(NULL, "/");
    }

    return &fs->inodes[current];
}

void split_path(const char* path, char* dir, char* name) {

    char temp[256];
    strncpy(temp, path, sizeof(temp));
    temp[sizeof(temp)-1] = '\0';

    char* last = strrchr(temp, '/');

    if (last) {
        *last = '\0';
        strcpy(dir, temp);
        strcpy(name, last + 1);
    } else {
        strcpy(dir, ".");
        strcpy(name, temp);
    }
}

Inode* get_or_create_dir(char* path) {

    if (!fs) return NULL;

    int current;

    if (path[0] == '/') {
        current = 0;
        path++;
    } else {
        current = current_dir;
    }

    char temp[256];
    strncpy(temp, path, sizeof(temp));
    temp[sizeof(temp)-1] = '\0';

    char* token = strtok(temp, "/");

    while (token != NULL) {

        Inode* dir = &fs->inodes[current];

        if (!dir->is_dir) return NULL;
int found = -1;

for (int i = 0; i < dir->num_children; i++) {

    Inode* c = &fs->inodes[dir->children[i]];

    if (strcmp(c->name, token) == 0) {

        if (!c->is_dir) {
            printf("Errore: path contiene file\n");
            return NULL;
        }

        found = dir->children[i];
        break;
    }
}

        
        if (found == -1) {

            int idx = find_free_inode();
            if (idx == -1) return NULL;

            Inode* new_dir = &fs->inodes[idx];

            strcpy(new_dir->name, token);
            new_dir->is_dir = 1;
            new_dir->parent = current;
            new_dir->num_children = 0;
            new_dir->size = 0;

            if (dir->num_children >= MAX_CHILDREN) {
              printf("Directory piena\n");
              return NULL;
}

            dir->children[dir->num_children++] = idx;

            found = idx;

            printf("Directory creata automaticamente: %s\n", token);
        }

        current = found;
        token = strtok(NULL, "/");
    }

    return &fs->inodes[current];
}

int find_free_inode() {
    if (!fs) return -1;

    for (int i = 0; i < MAX_INODES; i++) {
        if (fs->inodes[i].name[0] == '\0') {
            return i;
        }
    }
    return -1;
}

void fs_init(void* disk) {

    fs = (FileSystem*) disk;

    if (fs->sb.magic == 0xDEADBEEF) {
        printf("Filesystem caricato\n");
        current_dir = 0;
        return;
    }

    printf("Inizializzo filesystem...\n");

    memset(fs, 0, sizeof(FileSystem));

    fs->sb.magic = 0xDEADBEEF;
    fs->sb.size = sizeof(FileSystem);
    fs->sb.root_inode = 0;

    Inode* root = &fs->inodes[0];

    strcpy(root->name, "/");
    root->is_dir = 1;
    root->parent = -1;

    current_dir = 0;

    printf("Root creata\n");
}

void fs_mkdir(const char* path) {

    Inode* check = resolve_path(path);
if (check && check->is_dir) {
    printf("Errore: directory già esistente\n");
    return;
}

    Inode* dir = get_or_create_dir((char*)path);

    if (!dir) {
        printf("Errore mkdir\n");
        return;
    }

    printf("Directory '%s' pronta\n", path);
}
void fs_ls(const char* path) {

    Inode* dir;

    if (path == NULL) {
        dir = &fs->inodes[current_dir];
    } else {
        dir = resolve_path(path);
        if (!dir) {
            printf("Errore: directory non valida\n");
            return;
        }
    }

    if (!dir->is_dir) {
        printf("Errore: non è una directory\n");
        return;
    }

    printf("Contenuto directory '%s':\n", dir->name);

    for (int i = 0; i < dir->num_children; i++) {
        int idx = dir->children[i];
        Inode* child = &fs->inodes[idx];

        if (child->is_dir)
            printf("[DIR] %s\n", child->name);
        else
            printf("[FILE] %s\n", child->name);
    }
}

void fs_touch(const char* path) {

    char dir[256], name[256];
    split_path(path, dir, name);

   Inode* parent;

if (strcmp(dir, ".") == 0) {
    parent = &fs->inodes[current_dir];
} else {
    parent = resolve_path(dir);
}

if (!parent || !parent->is_dir) {
    printf("Errore: directory non valida\n");
    return;
}

for (int i = 0; i < parent->num_children; i++) {
    Inode* c = &fs->inodes[parent->children[i]];
    if (strcmp(c->name, name) == 0) {
        printf("Errore: file già esistente\n");
        return;
    }
}

    int idx = find_free_inode();
    if (idx == -1) {
        printf("Errore: nessun inode libero\n");
        return;
    }

    Inode* file = &fs->inodes[idx];

    strcpy(file->name, name);
    file->is_dir = 0;
    file->parent = (parent - fs->inodes);
    file->num_children = 0;
    file->size = 0;

    memset(file->data, 0, MAX_FILE_SIZE);

    if (parent->num_children >= MAX_CHILDREN) {
    printf("Directory piena\n");
    return;
}

parent->children[parent->num_children++] = idx;

    printf("File '%s' creato\n", path);
}

void fs_append(const char* path, const char* text) {

    Inode* file = resolve_path(path);

    if (!file || file->is_dir) {
        printf("Errore: file non valido\n");
        return;
    }

    int len = strlen(text);

    if (file->size + len >= MAX_FILE_SIZE) {
        printf("Errore: file pieno\n");
        return;
    }

    memcpy(file->data + file->size, text, len);
    file->size += len;

    printf("Append su '%s' riuscito\n", path);
}

void fs_cat(const char* path) {

    Inode* file = resolve_path(path);

    if (!file || file->is_dir) {
        printf("Errore: file non valido\n");
        return;
    }

    printf("%.*s\n", file->size, file->data);
}

void fs_cd(const char* path) {

if (strcmp(path, "..") == 0) {

    if (current_dir == 0) {
        printf("Sei già nella root\n");
        return;
    }

    current_dir = fs->inodes[current_dir].parent;
    printf("Salito di directory\n");
    return;
}

    Inode* dir = resolve_path(path);

    if (!dir || !dir->is_dir) {
        printf("Errore: directory non valida\n");
        return;
    }

    current_dir = (dir - fs->inodes);

    printf("Entrato in '%s'\n", dir->name);
}

void fs_rm(const char* path) {

    Inode* target = resolve_path(path);

    if (!target) {
        printf("Errore: non trovato\n");
        return;
    }

    // controllo directory non vuota
    if (target->is_dir && target->num_children > 0) {
        printf("Errore: directory non vuota\n");
        return;
    }

    Inode* parent = &fs->inodes[target->parent];

    for (int i = 0; i < parent->num_children; i++) {

        if (parent->children[i] == (target - fs->inodes)) {

            for (int j = i; j < parent->num_children - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }

            parent->num_children--;
            break;
        }
    }

    memset(target, 0, sizeof(Inode));

    printf("Eliminato\n");
}