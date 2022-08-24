#ifndef _NTR_STDEXT_H_
#define _NTR_STDEXT_H_ 1
#include <stdio.h>


#ifndef HAVE_STRNLEN
# include <stddef.h>
// Safe strlen because it's not standard
	extern size_t strnlen(const char *string, size_t maxlen);
#endif

// reverse string
void strrev(char *arr, int start, int end);
// int to string
char *itoa(int number, char *arr, int base);
// get name of pid, linux only
const char *pidname(int pid);

// Asynchronous file input/output
struct aiocb *async_read(int file, void *buff, size_t mem_sz, size_t n);
struct aiocb *async_read_str(int file, char *buff, size_t n);
struct aiocb *async_write(int file, void *, size_t mem_sz, size_t n);
struct aiocb *async_write_str(int file, char *str, size_t n);

#endif /* ifndef _NTR_STDEXT_H_ */

