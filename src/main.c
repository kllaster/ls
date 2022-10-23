#include "ls.h"

static inline void execute_ls(char *dir_path, t_options options)
{
    struct directory *dir;
    struct stat *s_stat = get_entry_stat(dir_path);

    if (s_stat == NULL)
    {
        free(dir_path);
        return;
    }

    dir = create_directory_info(dir_path, dir_path, s_stat);
    dir_browsing(dir, options, false);
}

static inline void execute_multiple_ls(char **dir_paths, size_t count_dirs, t_options options)
{
    struct stat *s_stat;
    struct directory *dirs = NULL;

    for (size_t i = 0; i < count_dirs; i++)
    {
        s_stat = get_entry_stat(dir_paths[i]);
        if (s_stat == NULL)
            continue;
        char *path = kl_strdup(dir_paths[i]);
        struct directory *dir = create_directory_info(path, path, s_stat);
        dirs = add_dir_in_list(dirs, dir, options);
    }
    dir_browsing(dirs, options, true);
}

int main(int argc, char *argv[])
{
    t_options options = parse_options(argc, (const char **)argv);

    int count_options = 1;
    while (count_options < argc && argv[count_options][0] == '-' && argv[count_options][1] != '\0')
        count_options++;

    size_t count_dirs = argc - count_options;
    if (count_dirs > 0)
        execute_multiple_ls(&argv[count_options], count_dirs, options);
    else
        execute_ls(kl_strdup("."), options);
    return (0);
}