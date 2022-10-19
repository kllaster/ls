#ifndef KL_LS_UTILS_H
# define KL_LS_UTILS_H

# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>

# define print_str_literal(str) \
        write(STDIN_FILENO, (str), sizeof(str) - 1); \

size_t kl_strlen(const char *str);
char *kl_strjoin(const char *s1, const char *s2);
char *kl_strdup(const char *str);
bool kl_strequal(char *str1, char *str2);
void print_str(char *str);

#endif