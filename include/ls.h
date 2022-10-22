#ifndef KL_LS_H
# define KL_LS_H

# include <pwd.h>
# include <grp.h>
# include <time.h>
# include <stdbool.h>
# include <stdlib.h>
# include <dirent.h>
# include <sys/stat.h>
# include "utils.h"

# define OPTION_NONE            0b00000000
# define OPTION_ALL             0b00000001
# define OPTION_RECURSIVE       0b00000010
# define OPTION_LONG_FORMAT     0b00000100
# define OPTION_SORT_BY_TIME    0b00001000

typedef int t_options;

t_options parse_options(int argc, const char **argv);

//---

typedef struct directory t_directory;

struct directory
{
    DIR *dir_stream;
    char *name;
    char *path;
    struct entry_info *entrys;
    t_directory *next;
    size_t max_hard_links_len;
    size_t max_owner_name_len;
    size_t max_group_name_len;
    size_t max_size_len;
    size_t total_blocks;
};

typedef struct entry_info t_file_info;

struct entry_info
{
    char *name;
    char *owner_name;
    char *group_name;
    t_file_info *next;
    struct stat stat;
    size_t owner_name_len;
    size_t group_name_len;
};

//---

void execute_ls(char *dir_path, t_options options);
void execute_multiple_ls(char **dir_paths, size_t count_dirs, t_options options);

void add_entry(struct directory *dir, char *entry_name, char *entry_path);

void print_simple(struct directory *dir, bool show_path);
void print_long_format(struct directory *dir, bool show_path);

#endif