#include "ls.h"

static struct directory *add_directory(struct directory *dirs, char *name, char *dir_path)
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

    new_dir = malloc(sizeof(struct directory));
    new_dir->name = kl_strdup(name);
    new_dir->path = dir_path;
    new_dir->dir_stream = dir;
    new_dir->next = NULL;
    new_dir->entrys = NULL;
    new_dir->total_blocks = 0;
    new_dir->max_size_len = 0;
    new_dir->max_owner_name_len = 0;
    new_dir->max_group_name_len = 0;
    new_dir->max_hard_links_len = 0;

    if (dirs)
    {
        struct directory *it = dirs;
        struct directory *prev = NULL;

        while (it)
        {
            if (kl_strcmp(it->name, name) > 0)
            {
                if (prev)
                    prev->next = new_dir;
                else
                    dirs = new_dir;
                new_dir->next = it;
                break;
            }
            prev = it;
            it = it->next;
        }
        if (it == NULL)
        {
            prev->next = new_dir;
        }
    }
    else
        dirs = new_dir;
    return dirs;
}

static inline void remove_directory(struct directory *dir)
{
    struct entry_info *it = dir->entrys;
    struct entry_info *next;

    while (it)
    {
        next = it->next;

        free(it->name);
        free(it);

        it = next;
    }

    closedir(dir->dir_stream);
    free(dir->path);
    free(dir->name);
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

static inline struct directory *add_new_dirs_to_end(
    struct directory *dirs, struct directory *new_dirs
)
{
    if (new_dirs)
    {
        struct directory *start_new_dir = new_dirs;

        while (new_dirs->next)
            new_dirs = new_dirs->next;
        new_dirs->next = dirs;

        dirs = start_new_dir;
    }
    return dirs;
}

static inline void dir_browsing(
    struct directory *dirs, t_options options, bool print_dir_path
)
{
    bool option_all = options & OPTION_ALL;
    bool option_recursive = options & OPTION_RECURSIVE;
    bool option_long_format = options & OPTION_LONG_FORMAT;
    bool is_first_dir = true;

    while (dirs)
    {
        struct dirent *entry;
        struct directory *dir = dirs;
        struct directory *new_dirs = NULL;

        dirs = dir->next;

        while ((entry = readdir(dir->dir_stream)) != NULL)
        {
            if (!(entry->d_name[0] != '.' || option_all))
                continue;

            char *path = create_path(dir->path, entry->d_name);
            add_entry(dir, entry->d_name, path);

            if (option_recursive && entry->d_type == DT_DIR
                && !kl_strequal(".", entry->d_name)
                && !kl_strequal("..", entry->d_name))
            {
                new_dirs = add_directory(new_dirs, entry->d_name, path);
            }
            else
                free(path);
        }

        if (is_first_dir == false)
            print_str_literal("\n");
        if (option_long_format)
            print_long_format(dir, print_dir_path);
        else
            print_simple(dir, print_dir_path);

        remove_directory(dir);
        dirs = add_new_dirs_to_end(dirs, new_dirs);
        print_dir_path = true;
        is_first_dir = false;
    }
}

void execute_ls(char *dir_path, t_options options)
{
    struct directory *dirs = add_directory(NULL, dir_path, dir_path);

    dir_browsing(dirs, options, false);
}

void execute_multiple_ls(char **dir_paths, size_t count_dirs, t_options options)
{
    struct directory *dirs = NULL;

    for (size_t i = 0; i < count_dirs; i++)
    {
        char *path = kl_strdup(dir_paths[i]);
        dirs = add_directory(dirs, path, path);
    }
    dir_browsing(dirs, options, true);
}
