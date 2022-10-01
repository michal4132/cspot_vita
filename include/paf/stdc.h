#pragma once

#include <stdarg.h>
#include <wchar.h>
#include <psp2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * std C functions
 */

void *sce_paf_bzero(void *ptr, SceSize num);
void *sce_paf_memchr(const void *ptr, int value, SceSize num);
int sce_paf_memcmp(const void *ptr1, const void *ptr2, SceSize num);
int sce_paf_bcmp(const void *ptr1, const void *ptr2, SceSize num);
void *sce_paf_memcpy(void *destination, const void *source, SceSize num);
void *sce_paf_memmove(void *destination, const void *source, SceSize num);
void *sce_paf_bcopy(void *destination, const void *source, SceSize num);
void *sce_paf_memset(void *ptr, int value, SceSize num);
int sce_paf_snprintf(char *s, SceSize n, const char *format, ...);
int sce_paf_strcasecmp(const char *str1, const char *str2);
char *sce_paf_strchr(const char *str, int character);
int sce_paf_strcmp(const char *str1, const char *str2);
size_t sce_paf_strlen(const char *str);
int sce_paf_strncasecmp(const char *str1, const char *str2, SceSize num);
int sce_paf_strncmp(const char *str1, const char *str2, SceSize num);
char *sce_paf_strncpy(char *destination, const char *source, SceSize num);
char *sce_paf_strrchr(const char *str, int character);

void *sce_paf_memalign(size_t align, size_t length);
void sce_paf_free(void *ptr);
void *sce_paf_malloc(size_t size);
void* sce_paf_calloc(size_t num, size_t size);

/**
 * wchar functions
 */

/* __attribute__((__format__(__wprintf__, 3, 4))) */
int sce_paf_swprintf(wchar_t *buffer, size_t bufsz, const wchar_t *format, ...);

int sce_paf_vswprintf(wchar_t *buffer, size_t bufsz, const wchar_t *format, va_list vlist);

wint_t sce_paf_towlower(wint_t wc);

wchar_t *sce_paf_wcscat(wchar_t *dest, const wchar_t *src);
wchar_t *sce_paf_wcsncat(wchar_t *dest, const wchar_t *src, size_t count);
wchar_t *sce_paf_wcscpy(wchar_t *dest, const wchar_t *src);
wchar_t *sce_paf_wcsncpy(wchar_t *dest, const wchar_t *src, size_t count);

int sce_paf_wcscasecmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *sce_paf_wcschr(const wchar_t *str, wchar_t ch);
int sce_paf_wcscmp(const wchar_t *lhs, const wchar_t *rhs);
size_t sce_paf_wcscspn(const wchar_t *dest, const wchar_t *src);
size_t sce_paf_wcslen(const wchar_t *str);
int sce_paf_wcsncasecmp(const wchar_t *s1, const wchar_t *s2, size_t n);
int sce_paf_wcsncmp(const wchar_t *lhs, const wchar_t *rhs, size_t count);
size_t sce_paf_wcsnlen(const wchar_t *s, size_t maxlen);
wchar_t *sce_paf_wcspbrk(const wchar_t *dest, const wchar_t *str);
wchar_t *sce_paf_wcsrchr(const wchar_t *str, wchar_t ch);
size_t sce_paf_wcsspn(const wchar_t *dest, const wchar_t *src);

wchar_t *sce_paf_wmemchr(const wchar_t *ptr, wchar_t ch, size_t count);
int sce_paf_wmemcmp(const wchar_t *lhs, const wchar_t *rhs, size_t count);
wchar_t *sce_paf_wmemcpy(wchar_t *dest, const wchar_t *src, size_t count);
wchar_t *sce_paf_wmemmove(wchar_t *dest, const wchar_t *src, size_t count);

#ifdef __cplusplus
}
#endif