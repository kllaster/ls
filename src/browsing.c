#include "ls.h"

static char *create_path(const char *path, const char *entry_name)
{
    char *res;
    char *path_tmp;

    path_tmp = kl_strjoin(path, "/");
    res = kl_strjoin(path_tmp, entry_name);
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
            char *entry_name = entry->d_name;
            if (!(entry_name[0] != '.' || option_all))
                continue;

            char *path = create_path(dir->path, entry_name);

            struct stat *s_stat = get_entry_stat(path);
            if (s_stat == NULL)
            {
                free(path);
                continue;
            }

            if (option_long_format == false)
            {
                size_t len = kl_strlen(entry_name);
                if (len > dir->max_name_len)
                    dir->max_name_len = len;
            }

            struct entry_info *entry_info = create_entry_info(dir, entry_name, s_stat);
            add_entry_in_directory(dir, entry_info, options);

            if (S_ISLNK(s_stat->st_mode))
                set_link_info(entry_info, path);

            if (option_recursive && entry->d_type == DT_DIR
                && !kl_strequal(".", entry_name)
                && !kl_strequal("..", entry_name))
            {
                struct directory *new_dir = create_directory_info(entry_name, path, s_stat);
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
