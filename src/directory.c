#include "ls.h"

typedef long (t_dir_cmp)(struct directory *, struct directory *);

long dir_name_cmp(struct directory *dir1, struct directory *dir2)
{
    return kl_strcmp(dir1->name, dir2->name);
}

long dir_time_cmp(struct directory *dir1, struct directory *dir2)
{
    long res;

    res = dir2->stat->st_mtimespec.tv_sec - dir1->stat->st_mtimespec.tv_sec;
    if (res != 0)
        return res;

    res = dir2->stat->st_mtimespec.tv_nsec - dir1->stat->st_mtimespec.tv_nsec;
    if (res != 0)
        return res;
    return dir_name_cmp(dir1, dir2);
}

struct stat *get_entry_stat(const char *path)
{
    struct stat *s_stat = malloc(sizeof(struct stat));

    if (lstat(path, s_stat) < 0)
    {
        print_str_literal("ls: ");
        print_str(path);
        print_str_literal(": No such file or directory\n");
        return NULL;
    }
    return s_stat;
}

struct directory *create_directory_info(char *name, char *dir_path, struct stat *stat)
{
    DIR *dir;
    struct directory *new_dir;

    if ((dir = opendir(dir_path)) == NULL)
    {
        print_str_literal("ls: ");
        print_str(dir_path);
        print_str_literal(": No such file or directory\n");
        return NULL;
    }

    new_dir = malloc(sizeof(struct directory));
    new_dir->stat = stat;
    new_dir->name = kl_strdup(name);
    new_dir->path = dir_path;
    new_dir->dir_stream = dir;
    new_dir->next = NULL;
    new_dir->entrys = NULL;
    new_dir->total_blocks = 0;
    new_dir->max_name_len = 0;
    new_dir->max_size_len = 0;
    new_dir->max_owner_name_len = 0;
    new_dir->max_group_name_len = 0;
    new_dir->max_hard_links_len = 0;
    return new_dir;
}

struct directory *add_dir_in_list(
    struct directory *dirs, struct directory *new_dir, t_options options
)
{
    t_dir_cmp *dir_cmp;
    bool option_reverse_sort = options & OPTION_REVERSE_SORT;

    if (options & OPTION_SORT_BY_TIME)
        dir_cmp = dir_time_cmp;
    else
        dir_cmp = dir_name_cmp;

    if (dirs)
    {
        struct directory *it = dirs;
        struct directory *prev = NULL;

        while (it)
        {
            bool cmp_res = dir_cmp(it, new_dir) > 0;
            if ((cmp_res && !option_reverse_sort) || (!cmp_res && option_reverse_sort))
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
            prev->next = new_dir;
    }
    else
        dirs = new_dir;
    return dirs;
}

void remove_directory(struct directory *dir)
{
    struct entry_info *it = dir->entrys;
    struct entry_info *next;

    while (it)
    {
        next = it->next;

        free(it->name);
        free(it->linked_name);
        free(it);

        it = next;
    }

    closedir(dir->dir_stream);
    free(dir->path);
    free(dir->name);
    free(dir->stat);
    free(dir);
}
