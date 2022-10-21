#include "ls.h"

static char get_letter_mode(unsigned long st_mode)
{
    if (S_ISREG(st_mode))
        return ('-');
    else if (S_ISDIR(st_mode))
        return ('d');
    else if (S_ISCHR(st_mode))
        return ('c');
    else if (S_ISBLK(st_mode))
        return ('b');
    else if (S_ISFIFO(st_mode))
        return ('p');
    else if (S_ISLNK(st_mode))
        return ('l');
    else if (S_ISSOCK(st_mode))
        return ('s');
    return ('?');
}

static void print_user_permission(mode_t st_mode)
{
    print_char(st_mode & S_IRUSR ? 'r' : '-');
    print_char(st_mode & S_IWUSR ? 'w' : '-');
    if (st_mode & S_IXUSR)
    {
        if (st_mode & S_ISUID)
            print_char('s');
        else
            print_char('x');
    }
    else if (st_mode & S_ISUID)
        print_char('S');
    else
        print_char('-');
}

static void print_group_permission(mode_t st_mode)
{
    print_char(st_mode & S_IRGRP ? 'r' : '-');
    print_char(st_mode & S_IWGRP ? 'w' : '-');
    if (st_mode & S_IXGRP)
    {
        if (st_mode & S_ISGID)
            print_char('s');
        else
            print_char('x');
    }
    else if (st_mode & S_ISGID)
        print_char('S');
    else
        print_char('-');
}

static void print_other_permission(mode_t st_mode)
{
    print_char(st_mode & S_IROTH ? 'r' : '-');
    print_char(st_mode & S_IWOTH ? 'w' : '-');
    if ((st_mode & S_ISVTX) && (st_mode & S_IXUSR) && (st_mode & S_IXGRP))
        print_char('t');
    else if ((st_mode & S_ISVTX) && !(st_mode & S_IXUSR) && !(st_mode & S_IXGRP))
        print_char('T');
    else if (st_mode & S_IXOTH)
        print_char('x');
    else
        print_char('-');
}

void print_long_format(struct directory *dir, t_options options, bool show_path)
{
    if (show_path)
    {
        print_str_literal("\n");
        print_str(dir->path);
        print_str(":\n");
    }

    print_str_literal("total ");
    print_num((int)dir->total_blocks);
    print_str_literal("\n");

    struct entry_info *it = dir->entrys;
    bool show_all_option = options & OPTION_ALL;

    while (it)
    {
        if (!it->is_hidden || show_all_option)
        {
            struct stat *stat = &it->stat;

            print_char(get_letter_mode(stat->st_mode));
            print_user_permission(stat->st_mode);
            print_group_permission(stat->st_mode);
            print_other_permission(stat->st_mode);

            print_str_literal("\n");
        }
        it = it->next;
    }
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