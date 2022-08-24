#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include "log.h"
#include "stdext.h"

static const char *logprefix[4] = {
	"ALL: ", "INFO: ", "WARINING: ", "ERROR: "
};

pthread_mutex_t logmutex;

static struct aiocb defAIO = {0};
static struct aiocb *logAio(struct aiocb *aio);
static void logtofile(const char *msg);
// appends runtime info to dest
static void appendRTimeInfo(char *dest, const char *file,
#ifndef NO_FUNC
		const char *func,
#endif
		int line);

static enum LogLevel stdoutMask, fileMask;
static FILE *logfile;

void loginit(enum LogLevel _stdoutMask, enum LogLevel _fileMask) {
	stdoutMask = _stdoutMask;
	fileMask = _fileMask;
	logfile = fopen(LOGFILE_NAME, "a");
	if(!logfile) {
		fprintf(stderr, "ERROR: Couldn't open logfile, %s, %s\n", LOGFILE_NAME, strerror(errno));
	}
	pthread_mutex_init(&logmutex, 0);
}

void logdestroy() {
	if(pthread_mutex_trylock(&logmutex) == -1){// && errno == EBUSY) {
		while(aio_error(logAio(NULL)) == EINPROGRESS);
		free(logAio(NULL));
		logAio(NULL+1);
		goto cl;
	}
	struct aiocb *aio = logAio(NULL);
	if(aio) {
		//unsigned depth;
		//while(aio_error(aio) == EINPROGRESS && depth < MAX_AIO_WAIT)
		//	depth++;
		const struct aiocb *aios[1];
		aios[0] = aio;
		if(aio_suspend(aios, 1, NULL) == 0)
			free(aio);
		//if(depth != 500)
		//	free(aio);
	}
cl:
	if(logfile) {
		fclose(logfile);
		logfile = NULL;
	}
	pthread_mutex_unlock(&logmutex);
	pthread_mutex_destroy(&logmutex);
}

void __logf(enum LogLevel level, const char *file,
#ifndef NO_FUNC
		const char *func,
#endif
		int line, const char *format, ...) {
	if(level == L_NONE)
		return;
	va_list ap;
	va_start(ap, format);

	char message[512];
	strncpy(message, logprefix[level], 9);
	{
		char tmp[256];
		vsnprintf(tmp, 256, format, ap);
		strcat(message, tmp);
	}
	va_end(ap);
	appendRTimeInfo(message, file,
#ifndef NO_FUNC
			func,
#endif
			line);
	if(stdoutMask != L_NONE && level >= stdoutMask)
		printf("%s\n", message);
	if(fileMask != L_NONE && level >= fileMask)
		logtofile(message);
}

void __log(enum LogLevel level, const char *file,
#ifndef NO_FUNC
		const char *func,
#endif
		int line, const char *msg) {
	if(level == L_NONE)
		return;
	char message[512];
	strncpy(message, logprefix[level], 9);
	strncat(message, msg, 256);

	appendRTimeInfo(message, file,
#ifndef NO_FUNC
			func,
#endif
			line);

	// outputing
	if(stdoutMask != L_NONE && level >= stdoutMask)
		printf("%s\n", message);
	if(fileMask != L_NONE && level >= fileMask)
		logtofile(message);
}

static void appendRTimeInfo(char *dest, const char *file,
#ifndef NO_FUNC
		const char *func,
#endif
		int line) {
	// not ASSERT because it would call log
#ifdef _NTR_ASSERT_H_
	__assert_m(dest != NULL, "Dest was NULL, while appending runtime info", __FILE__,
# ifndef NO_FUNC
			__func__,
# endif
			__LINE__, false);
#endif

	// ignore warning because, it thinks that message can be 512 long, but it at most can be 265
	snprintf(dest+strnlen(dest, 265), 512, "\n\t in file: '%s', "
#ifndef NO_FUNC
			"in function: %s, "
#endif

			"at line: %d, errno: %s\n",
			file,
#ifndef NO_FUNC
			func,
#endif
			line, strerror(errno));
}

static struct aiocb *logAio(struct aiocb *aio) {
	static struct aiocb *val;
	if(aio == NULL)
		return val;
	if(val != NULL)
		free(val);
	val = aio;
	return NULL;
}

static void logtofile(const char *msg) {
	pthread_mutex_lock(&logmutex);
	if(!logfile)
		return;
	// Error already printed in loginit
	logAio(async_write_str(fileno(logfile), (char *)msg, strnlen(msg, 512)));
	pthread_mutex_unlock(&logmutex);
}

