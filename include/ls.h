#ifndef KL_LS_H
# define KL_LS_H

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
    size_t total_blocks;
    struct entry_info *entrys;
    t_directory *next;
};

typedef struct entry_info t_file_info;

struct entry_info
{
    char *name;
    bool is_hidden;
    struct stat stat;
    t_file_info *next;
};

//---

void execute_ls(char *dir_path, t_options options);
void execute_multiple_ls(char **dir_paths, size_t count_dirs, t_options options);

void print_simple(struct directory *dir, t_options options, bool show_path);
void print_long_format(struct directory *dir, t_options options);

#endif