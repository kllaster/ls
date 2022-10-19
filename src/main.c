#include "ls.h"

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