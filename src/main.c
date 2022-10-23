#include "ls.h"

static inline void execute_ls(char *dir_path, t_options options)
{
    struct directory *dirs = add_directory(NULL, dir_path, dir_path);

    dir_browsing(dirs, options, false);
}

static inline void execute_multiple_ls(char **dir_paths, size_t count_dirs, t_options options)
{
    struct directory *dirs = NULL;

    for (size_t i = 0; i < count_dirs; i++)
    {
        char *path = kl_strdup(dir_paths[i]);
        dirs = add_directory(dirs, path, path);
    }
    dir_browsing(dirs, options, true);
}

int main(int argc, char *argv[])
{
    t_options options = parse_options(argc, (const char **)argv);

    int count_options = 1;
    while (count_options < argc && argv[count_options][0] == '-')
        count_options++;

    size_t count_dirs = argc - count_options;
    if (count_dirs > 0)
        execute_multiple_ls(&argv[count_options], count_dirs, options);
    else
        execute_ls(kl_strdup("."), options);
    return (0);
}