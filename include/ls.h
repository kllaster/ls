#ifndef KL_LS_H
# define KL_LS_H

# include <pwd.h>
# include <grp.h>
# include <time.h>
# include <stdlib.h>
# include <dirent.h>
# include <stdbool.h>
# include <sys/stat.h>
# include <sys/syslimits.h>
# include "utils.h"

# define OPTION_NONE            0b00000000
# define OPTION_ALL             0b00000001
# define OPTION_RECURSIVE       0b00000010
# define OPTION_LONG_FORMAT     0b00000100
# define OPTION_SORT_BY_TIME    0b00001000
# define OPTION_REVERSE_SORT    0b00010000

typedef int t_options;

t_options parse_options(int argc, const char **argv);

//---

typedef struct directory t_directory;

struct directory
{
    struct stat *stat;
    DIR *dir_stream;
    char *name;
    char *path;
    struct entry_info *entrys;
    t_directory *next;
    size_t max_name_len;
    size_t max_hard_links_len;
    size_t max_owner_name_len;
    size_t max_group_name_len;
    size_t max_size_len;
    size_t total_blocks;
};

typedef struct entry_info t_file_info;

struct entry_info
{
    struct stat stat;
    char *name;
    char *linked_name;
    char *owner_name;
    char *group_name;
    t_file_info *next;
    size_t owner_name_len;
    size_t group_name_len;
};

//---

void dir_browsing(struct directory *dirs, t_options options, bool print_dir_path);

struct stat *get_entry_stat(const char *path);

struct directory *create_directory_info(char *name, char *dir_path, struct stat *stat);
struct directory *add_dir_in_list(
    struct directory *dirs, struct directory *new_dir, t_options options
);
void remove_directory(struct directory *dir);

struct entry_info *create_entry_info(struct directory *dir, char *entry_name, struct stat *s_stat);
void add_entry_in_directory(
    struct directory *dir, struct entry_info *entry_info, t_options options
);
void set_link_info(struct entry_info *entry_info, const char *path);

void print_simple(struct directory *dir, bool show_path);
void print_long_format(struct directory *dir, bool show_path);

#endif