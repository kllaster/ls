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

    if (stat(path, s_stat) < 0)
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
            if (dir_cmp(it, new_dir) > 0)
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
        free(it);

        it = next;
    }

    closedir(dir->dir_stream);
    free(dir->path);
    free(dir->name);
    free(dir->stat);
    free(dir);
}


static char *create_path(char *path, char *dir)
{
    char *res;
    char *path_tmp;

    path_tmp = kl_strjoin(path, "/");
    res = kl_strjoin(path_tmp, dir);
    free(path_tmp);
    return res;
}

static struct directory *add_new_dirs_to_end(struct directory *dirs, struct directory *new_dirs)
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

void dir_browsing(
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
            struct stat *s_stat = get_entry_stat(path);

            if (s_stat == NULL)
            {
                free(path);
                continue;
            }

            if (option_long_format == false)
            {
                size_t len = kl_strlen(entry->d_name);
                if (len > dir->max_name_len)
                    dir->max_name_len = len;
            }

            struct entry_info *entry_info = create_entry_info(dir, entry->d_name, s_stat);
            add_entry_in_directory(dir, entry_info, options);

            if (option_recursive && entry->d_type == DT_DIR
                && !kl_strequal(".", entry->d_name)
                && !kl_strequal("..", entry->d_name))
            {
                struct directory *new_dir = create_directory_info(entry->d_name, path, s_stat);
                new_dirs = add_dir_in_list(new_dirs, new_dir, options);
            }
            else
            {
                free(path);
                free(s_stat);
            }
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
