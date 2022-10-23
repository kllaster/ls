#include "ls.h"

typedef long (t_entry_cmp)(struct entry_info *, struct entry_info *);

long entry_name_cmp(struct entry_info *entry1, struct entry_info *entry2)
{
    return kl_strcmp(entry1->name, entry2->name);
}

long entry_time_cmp(struct entry_info *entry1, struct entry_info *entry2)
{
    long res;

    res = entry2->stat.st_mtimespec.tv_sec - entry1->stat.st_mtimespec.tv_sec;
    if (res != 0)
        return res;

    res = entry2->stat.st_mtimespec.tv_nsec - entry1->stat.st_mtimespec.tv_nsec;
    if (res != 0)
        return res;
    return entry_name_cmp(entry1, entry2);
}

static inline void add_max_fields_len(struct directory *dir, struct entry_info *entry_info)
{
    {
        size_t len = kl_numlen(entry_info->stat.st_nlink);
        if (len > dir->max_hard_links_len)
            dir->max_hard_links_len = len;
    }

    struct passwd *pw = getpwuid(entry_info->stat.st_uid);
    if (pw != NULL)
    {
        entry_info->owner_name = pw->pw_name;

        size_t len = kl_strlen(pw->pw_name);
        if (len > dir->max_owner_name_len)
            dir->max_owner_name_len = len;
        entry_info->owner_name_len = len;
    }

    struct group *gr = getgrgid(entry_info->stat.st_gid);
    if (gr != NULL)
    {
        entry_info->group_name = gr->gr_name;

        size_t len = kl_strlen(gr->gr_name);
        if (len > dir->max_group_name_len)
            dir->max_group_name_len = len;
        entry_info->group_name_len = len;
    }

    {
        size_t len = kl_numlen(entry_info->stat.st_size);
        if (len > dir->max_size_len)
            dir->max_size_len = len;
    }
}

void add_entry_in_directory(struct directory *dir, struct entry_info *entry_info, t_options options)
{
    t_entry_cmp *entry_cmp;
    bool option_reverse_sort = options & OPTION_REVERSE_SORT;

    if (options & OPTION_SORT_BY_TIME)
        entry_cmp = entry_time_cmp;
    else
        entry_cmp = entry_name_cmp;

    if (dir->entrys)
    {
        struct entry_info *it = dir->entrys;
        struct entry_info *prev = NULL;

        while (it)
        {
            bool cmp_res = entry_cmp(it, entry_info) > 0;
            if ((cmp_res && !option_reverse_sort) || (!cmp_res && option_reverse_sort))
            {
                if (prev)
                    prev->next = entry_info;
                else
                    dir->entrys = entry_info;
                entry_info->next = it;
                break;
            }
            prev = it;
            it = it->next;
        }

        if (it == NULL)
            prev->next = entry_info;
    }
    else
        dir->entrys = entry_info;
}

void set_link_info(struct entry_info *entry_info, const char *path)
{
    ssize_t size;
    char linked_rel_path[PATH_MAX];

    if ((size = readlink(path, linked_rel_path, PATH_MAX)) == -1)
        return;
    linked_rel_path[size] = '\0';

    entry_info->linked_name = kl_strdup(linked_rel_path);
}

struct entry_info *create_entry_info(struct directory *dir, char *entry_name, struct stat *s_stat)
{
    struct entry_info *entry_info = malloc(sizeof(struct entry_info));

    entry_info->next = NULL;
    entry_info->name = kl_strdup(entry_name);
    entry_info->linked_name = NULL;
    entry_info->owner_name = NULL;
    entry_info->group_name = NULL;
    entry_info->stat = *s_stat;

    add_max_fields_len(dir, entry_info);

    dir->total_blocks += entry_info->stat.st_blocks;

    return entry_info;
}
