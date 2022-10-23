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

static inline void print_date(time_t time)
{
    char *date = ctime(&time);

    date = (char *)date + 4;
    date[12] = '\0';
    print_str(date);
}

static inline void print_long_format_entry(struct directory *dir, struct entry_info *entry)
{
    struct stat *stat = &entry->stat;

    print_char(get_letter_mode(stat->st_mode));
    print_user_permission(stat->st_mode);
    print_group_permission(stat->st_mode);
    print_other_permission(stat->st_mode);

    print_repeat_char(' ', (dir->max_hard_links_len - kl_numlen(stat->st_nlink)) + 2);

    print_num(stat->st_nlink);

    print_repeat_char(' ', (dir->max_owner_name_len - entry->owner_name_len) + 1);

    if (entry->owner_name != NULL)
        print_str(entry->owner_name);
    else
        print_num((int)stat->st_uid);

    print_repeat_char(' ', (dir->max_group_name_len - entry->group_name_len) + 2);

    if (entry->group_name != NULL)
        print_str(entry->group_name);
    else
        print_num((int)stat->st_gid);

    print_repeat_char(' ', (dir->max_size_len - kl_numlen(stat->st_size)) + 2);

    print_num(stat->st_size);

    print_str_literal(" ");

    print_date(stat->st_mtime);

    print_str_literal(" ");

    print_str(entry->name);

    if (entry->linked_name)
    {
        print_str_literal(" -> ");
        print_str(entry->linked_name);
    }

    print_str_literal("\n");
}

void print_long_format(struct directory *dir, bool show_path)
{
    if (show_path)
    {
        print_str(dir->path);
        print_str(":\n");
    }

    print_str_literal("total ");
    print_num((int)dir->total_blocks);
    print_str_literal("\n");

    struct entry_info *it = dir->entrys;

    while (it)
    {
        print_long_format_entry(dir, it);
        it = it->next;
    }
}

void print_simple(struct directory *dir, bool show_path)
{
    struct entry_info *it = dir->entrys;

    if (show_path)
    {
        print_str(dir->path);
        print_str(":\n");
    }

    size_t count_entrys = 0;
    while (it)
    {
        print_str(it->name);
        print_repeat_char(' ', (dir->max_name_len - kl_strlen(it->name)) + 1);
        count_entrys++;
        it = it->next;
    }
    if (count_entrys)
        print_str_literal("\n");
}