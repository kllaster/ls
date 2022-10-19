#include "ls.h"

static t_options get_options_by_str(const char *str)
{
    t_options options = OPTION_NONE;

    if (str[0] != '-')
        return OPTION_NONE;
    for (int i = 1; str[i] != '\0'; i++)
    {
        if (str[i] == 'a')
            options |= OPTION_ALL;
        else if (str[i] == 'R')
            options |= OPTION_RECURSIVE;
        else if (str[i] == 'l')
            options |= OPTION_LONG_FORMAT;
        else if (str[i] == 't')
            options |= OPTION_SORT_BY_TIME;
    }
    return options;
}

t_options parse_options(int argc, const char **argv)
{
    t_options options = OPTION_NONE;

    for (int i = 1; i < argc; i++)
        options |= get_options_by_str(argv[i]);
    return options;
}