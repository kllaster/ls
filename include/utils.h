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
int kl_strncmp(const char *str1, const char *str2, size_t n);
int kl_numlen(long long n);
void print_num(long long num);
void print_str(char *str);
void print_char(char c);
void print_repeat_char(char c, size_t len);

#endif