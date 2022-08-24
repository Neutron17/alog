#include "stdext.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aio.h>

#include "log.h"

#ifndef HAVE_STRNLEN
inline size_t strnlen(const char *str, size_t n) {
	const char * stop = (char *)memchr(str, '\0', n);
	return stop ? stop - str : n;
}
#endif

void strrev(char *arr, int start, int end) {
	char temp;

	if (start >= end)
		return;

	temp = *(arr + start);
	*(arr + start) = *(arr + end);
	*(arr + end) = temp;

	start++;
	end--;
	strrev(arr, start, end);
}

char *itoa(int number, char *arr, int base) {
	int i = 0, r, negative = 0;

	if (number == 0) {
		arr[i] = '0';
		arr[i + 1] = '\0';
		return arr;
	}
	if (number < 0 && base == 10) {
		number *= -1;
		negative = 1;
	}
	while (number != 0) {
		r = number % base;
		arr[i] = (r > 9) ? (r - 10) + 'a' : r + '0';
		i++;
		number /= base;
	}
	if (negative) {
		arr[i] = '-';
		i++;
	}
	strrev(arr, 0, i - 1);
	arr[i] = '\0';

	return arr;
}

const char *pidname(int pid) {
	char *name = calloc(1024, sizeof(char));
	if(!name)
		return "unknown";
	sprintf(name, "/proc/%d/cmdline",pid);
	FILE *file = fopen(name, "r");
	if (!file) {
		free(name);
		return "unknown";
	}
	size_t size;
		size = fread(name, sizeof(char), 1024, file);
	if(size>0) {
		if(name[size-1] == '\n')
			name[size-1] = '\0';
	}
	fclose(file);
	return name;
}

struct aiocb *async_read(int file, void *buff, size_t mem_sz, size_t n) {
	struct aiocb *aio = calloc(sizeof(*aio), 1);
	if(!aio) {
		LOG(L_ERR, "Couldn't allocate for async_read/aiocb");
		return NULL;
	}
	aio->aio_buf = buff;
	aio->aio_fildes = file;
	aio->aio_nbytes = mem_sz*n;
	aio->aio_offset = 0;
	if(aio_read(aio) < 0) {
		LOG(L_ERR, "aoi_read failed");
		free(aio);
		return NULL;
	}
	return aio;
}

struct aiocb *async_read_str(int file, char *buff, size_t n) {
	struct aiocb *aio = calloc(sizeof(*aio), 1);
	if(!aio) {
		LOG(L_ERR, "Couldn't allocate for async_read/aiocb");
		return NULL;
	}
	aio->aio_buf = buff;
	aio->aio_fildes = file;
	aio->aio_nbytes = n;
	aio->aio_offset = 0;
	if(aio_read(aio) < 0) {
		LOG(L_ERR, "aoi_read failed");
		free(aio);
		return NULL;
	}
	return aio;
}

struct aiocb *async_write(int file, void *buff, size_t mem_sz, size_t n) {
	struct aiocb *aio = calloc(sizeof(*aio), 1);
	aio->aio_buf = buff;
	aio->aio_fildes = file;
	aio->aio_nbytes = n;
	if(aio_write(aio) < 0) {
		LOG(L_ERR, "aoi_write failed");
		return NULL;
	}
	return aio;
}
struct aiocb *async_write_str(int file, char *str, size_t n) {
	struct aiocb *aio = calloc(sizeof(*aio), 1);
	if(!aio) {
		LOG(L_ERR, "Couldn't allocate for async_write/aiocb");
		return NULL;
	}
	aio->aio_buf = str;
	aio->aio_fildes = file;
	aio->aio_nbytes = strlen(str);
	if(aio_write(aio) < 0) {
		LOG(L_ERR, "aoi_write failed");
		return NULL;
	}
	return aio;
}

