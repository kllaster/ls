#include "utils.h"

size_t kl_strlen(const char *str)
{
    size_t size = 0;
    while (str[size] != '\0')
        size++;
    return size;
}

inline void print_str(char *str)
{
    write(STDIN_FILENO, str, kl_strlen(str));
}

inline void print_char(char c)
{
    write(STDIN_FILENO, &c, 1);
}

int kl_numlen(long long n)
{
    int i;

    if (n == 0)
        return (1);
    i = 0;
    while (n)
    {
        n /= 10;
        ++i;
    }
    return (i);
}

static char *kl_getres(char *res, unsigned long long num, int len, int flag)
{
    if (!num)
        res[0] = '0';
    else
    {
        while (num)
        {
            res[--len] = (num % 10) + '0';
            num /= 10;
        }
        if (flag)
            res[0] = '-';
    }
    return (res);
}

char *kl_itoa(long long n)
{
    char *res;
    int flag;
    int len;
    unsigned long long num;

    len = kl_numlen(n);
    flag = 0;
    if (n < 0)
    {
        if (n < 0)
            flag = 1;
        len += 1;
        n *= -1;
    }
    num = (unsigned int)n;
    res = malloc((len + 1) * sizeof(char));
    if (res == NULL)
        return (NULL);
    res[len] = '\0';
    return (kl_getres(res, num, len, flag));
}

void print_num(long long num)
{
    char *str = kl_itoa(num);
    print_str(str);
    free(str);
}

int kl_strncmp(const char *str1, const char *str2, size_t n)
{
    size_t i;

    i = 0;
    while ((str1[i] || str2[i]) && n > i)
    {
        if ((unsigned char)str1[i] != (unsigned char)str2[i])
            return ((unsigned char)str1[i] - (unsigned char)str2[i]);
        ++i;
    }
    return (0);
}

int kl_strcmp(const char *str1, const char *str2)
{
    size_t i;

    i = 0;
    while ((str1[i] || str2[i]))
    {
        if ((unsigned char)str1[i] != (unsigned char)str2[i])
            return ((unsigned char)str1[i] - (unsigned char)str2[i]);
        ++i;
    }
    return (0);
}

bool kl_strequal(char *str1, char *str2)
{
    size_t len1;

    len1 = kl_strlen(str1);
    if (len1 == kl_strlen(str2) && kl_strncmp(str1, str2, len1) == 0)
        return (true);
    return (false);
}

char *kl_strdup(const char *str)
{
    char *res;
    char *p;
    size_t size;

    size = (size_t)kl_strlen((char *)str) + 1;
    res = malloc(size * sizeof(char));
    if (res == NULL)
        return (0);
    p = res;
    while (*str)
    {
        *res = *str;
        str++;
        res++;
    }
    *res = '\0';
    return (p);
}

void *kl_memmove(void *dest, const void *source, size_t count)
{
    unsigned char *p1;
    unsigned char *p2;

    p1 = (unsigned char *)dest;
    p2 = (unsigned char *)source;
    if (dest == source)
        return (dest);
    if (dest > source)
    {
        while (count--)
            p1[count] = p2[count];
    }
    else
    {
        size_t i = -1;
        while (++i != count)
            p1[i] = p2[i];
    }
    return (dest);
}

char *kl_strjoin(const char *s1, const char *s2)
{
    size_t size;
    size_t len1;
    size_t len2;
    char *res;

    if (!s1)
        return (0);
    len1 = kl_strlen((char *)s1);
    len2 = kl_strlen((char *)s2);
    size = len1 + len2 + 1;
    res = malloc(size * sizeof(char));
    if (res == NULL)
        return (NULL);
    kl_memmove((void *)res, (const void *)s1, len1);
    kl_memmove((void *)(res + len1), (const void *)s2, len2);
    *(res + size - 1) = 0;
    return (res);
}

void print_repeat_char(char c, size_t len)
{
    for (size_t i = 0; i < len; i++)
        print_char(c);
}