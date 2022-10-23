#include "ls.h"

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

static inline void add_entry_in_directory(struct directory *dir, struct entry_info *entry_info)
{
    if (dir->entrys)
    {
        struct entry_info *it = dir->entrys;
        struct entry_info *prev = NULL;
        char *name = entry_info->name;

        while (it)
        {
            if (kl_strcmp(it->name, name) > 0)
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

void add_entry(struct directory *dir, char *entry_name, char *entry_path)
{
    struct entry_info *entry_info = malloc(sizeof(struct entry_info));

    if (stat(entry_path, &entry_info->stat) < 0)
    {
        // TODO: Print error?
        free(entry_info);
        return;
    }

    entry_info->next = NULL;
    entry_info->name = kl_strdup(entry_name);
    entry_info->owner_name = NULL;
    entry_info->group_name = NULL;

    add_max_fields_len(dir, entry_info);

    dir->total_blocks += entry_info->stat.st_blocks;

    add_entry_in_directory(dir, entry_info);
}