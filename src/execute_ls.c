#include "ls.h"
#include <printf.h>

void execute_multiple_ls(char **dir_paths, size_t count_dirs, t_options options)
{
    // TODO
    (void)dir_paths;
    (void)count_dirs;
    (void)options;
}

static struct directory *add_directory(struct directory *dirs, char *dir_path)
{
    DIR *dir;
    struct directory *new_dir;

    if ((dir = opendir(dir_path)) == NULL)
    {
        print_str_literal("ls: ");
        print_str(dir_path);
        print_str_literal(": No such file or directory\n");
        return dirs;
    }

    // TODO: insert by name
    new_dir = malloc(sizeof(struct directory));
    new_dir->path = dir_path;
    new_dir->dir_stream = dir;
    new_dir->next = dirs;
    return new_dir;
}

static inline void remove_directory(struct directory *dir)
{
    closedir(dir->dir_stream);
    free(dir->path);
    free(dir);
}

static inline char *create_path(char *path, char *dir)
{
    char *res;
    char *path_tmp;

    path_tmp = kl_strjoin(path, "/");
    res = kl_strjoin(path_tmp, dir);
    free(path_tmp);
    return res;
}

static inline void add_entry(struct dir_info *dir_info, char *entry_name, char *entry_path)
{
    struct entry_info *entry_info = malloc(sizeof(struct entry_info));

    if (stat(entry_path, &entry_info->stat) < 0)
    {
        // TODO: Print error?
        free(entry_info);
        return;
    }

    entry_info->next = NULL;
    entry_info->name = kl_strdup(entry_name);

    dir_info->total_size += entry_info->stat.st_size;

    if (dir_info->entrys)
    {
        struct entry_info *it = dir_info->entrys;
        struct entry_info *prev = NULL;

        while (it)
        {
            if (it->name[0] > entry_name[0])
            {
                if (prev)
                    prev->next = entry_info;
                else
                    dir_info->entrys = entry_info;
                entry_info->next = it;
                break;
            }
            prev = it;
            it = it->next;
        }

        if (it == NULL)
        {
            prev->next = entry_info;
        }
    }
    else
        dir_info->entrys = entry_info;
}

static inline void remove_dir_info(struct dir_info *dir_info)
{
    struct entry_info *it = dir_info->entrys;
    struct entry_info *next;

    while (it)
    {
        next = it->next;

        free(it->name);

        it = next;
    }
}

void print_dir_info(struct dir_info *dir_info)
{
    struct entry_info *it = dir_info->entrys;

    while (it)
    {
        print_str(it->name);
        print_str(" ");
        it = it->next;
    }
    print_str_literal("\n");
}

void execute_ls(char *dir_path, t_options options)
{
    struct dirent *entry;
    struct directory *dirs = add_directory(NULL, dir_path);
    bool option_recursive = options & OPTION_RECURSIVE;

    while (dirs)
    {
        struct dir_info dir_info = {};
        struct directory *dir = dirs;
        dirs = dir->next;

        while ((entry = readdir(dir->dir_stream)) != NULL)
        {
            char *path = create_path(dir->path, entry->d_name);
            add_entry(&dir_info, entry->d_name, path);

            if (option_recursive && entry->d_type == DT_DIR
                && !kl_strequal(".", entry->d_name)
                && !kl_strequal("..", entry->d_name))
            {
                dirs = add_directory(dirs, path);
            }
        }
        print_dir_info(&dir_info);
        remove_dir_info(&dir_info);
        remove_directory(dir);
    }
}
