#include "ls.h"

void print_long_format(struct directory *dir, t_options options)
{
    (void)dir;
    (void)options;
    print_str_literal("total ");
    print_num((int)dir->total_blocks);
    print_str_literal("\n");
}

void print_simple(struct directory *dir, t_options options, bool show_path)
{
    struct entry_info *it = dir->entrys;
    bool show_all_option = options & OPTION_ALL;

    if (show_path)
    {
        print_str_literal("\n");
        print_str(dir->path);
        print_str(":\n");
    }

    size_t count_entrys = 0;
    while (it)
    {
        if (!it->is_hidden || show_all_option)
        {
            print_str(it->name);
            print_str(" ");
            count_entrys++;
        }
        it = it->next;
    }
    if (count_entrys)
        print_str_literal("\n");
}