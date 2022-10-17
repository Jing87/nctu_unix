#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdarg.h>

#define DLSYM(sym)                                 \
    void *handle = dlopen("libc.so.6", RTLD_LAZY); \
    if (handle != NULL)                            \
        ori_##sym = dlsym(handle, #sym);

#define log(format, ...)                            \
    if (output == NULL) {                           \
        char *log_output = getenv("LOGGER_OUTPUT"); \
        if (log_output) {                           \
            DLSYM(fopen);                           \
            output = ori_fopen(log_output, "w");    \
        } else {                                    \
            int fd = atoi(getenv("LOGGER_STDERR")); \
            output = fdopen(fd, "w");               \
        }                                           \
    }                                               \
    fprintf(output, "[logger] " format, ##__VA_ARGS__);

static FILE *output;

static int (*ori_chmod)(const char *pathname, mode_t mode);
static int (*ori_chown)(const char *pathname, uid_t owner, gid_t group);
static int (*ori_creat)(const char *path, mode_t mode);
static int (*ori_creat64)(const char *path, mode_t mode);
static FILE *(*ori_fopen)(const char *pathname, const char *mode);
static FILE *(*ori_fopen64)(const char *pathname, const char *mode);
static int (*ori_fclose)(FILE *stream);
static size_t (*ori_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
static size_t (*ori_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
static int (*ori_open)(const char *pathname, int flags, ...);
static int (*ori_open64)(const char *pathname, int flags, ...);
static int (*ori_close)(int fd);
static ssize_t (*ori_read)(int fd, void *buf, size_t count);
static ssize_t (*ori_write)(int fd, const void *buf, size_t count);
static int (*ori_remove)(const char *path);
static int (*ori_rename)(const char *oldpath, const char *newpath);
static FILE *(*ori_tmpfile)(void);
static FILE *(*ori_tmpfile64)(void);

void getFileNameByFd(int fd, char link_dest[]) {
    ssize_t link_dest_size;
    char fdpath[200];
    snprintf(fdpath, 200, "/proc/self/fd/%d", fd);

    if ((link_dest_size = readlink(fdpath, link_dest, PATH_MAX - 1)) < 0)
        printf("%s\n", "fail to read link");
    else
        link_dest[link_dest_size] = '\0';
}

void charBuf(char *ptr, char char_buf[]) {
    for (int i = 0; i < 31; i++) {
        if (isprint(ptr[i])) {
            char_buf[i] = ptr[i];
        } else {
            if (ptr[i] == '\0') {
                char_buf[i] = '\0';
                break;
            }
            char_buf[i] = '.';
        }
    }
    char_buf[31] = '\0';
}

int chmod(const char *pathname, mode_t mode) {
    DLSYM(chmod);
    char real_pathname[PATH_MAX];
    char *res = realpath(pathname, real_pathname);
    int ret = ori_chmod(pathname, mode);
    log("%s(\"%s\", %o) = %d\n", __func__, res ? real_pathname : pathname, mode, ret);
    return ret;
}

int chown(const char *pathname, uid_t owner, gid_t group) {
    DLSYM(chown);
    char real_pathname[PATH_MAX];
    char *res = realpath(pathname, real_pathname);
    int ret = ori_chown(pathname, owner, group);
    log("%s(\"%s\", %d, %d) = %d\n", __func__, res ? real_pathname : pathname, owner, group, ret);
    return ret;
}

int creat(const char *path, mode_t mode) {
    DLSYM(creat);
    char real_path[PATH_MAX];
    char *res = realpath(path, real_path);
    int ret = ori_creat(path, mode);
    log("%s(\"%s\", %o) = %d\n", __func__, res ? real_path : path, mode, ret);
    return ret;
}

int creat64(const char *path, mode_t mode) {
    DLSYM(creat64);
    char real_path[PATH_MAX];
    char *res = realpath(path, real_path);
    int ret = ori_creat64(path, mode);
    log("%s(\"%s\", %o) = %d\n", __func__, res ? real_path : path, mode, ret);
    return ret;
}

FILE *fopen(const char *pathname, const char *mode) {
    DLSYM(fopen);
    char real_pathname[PATH_MAX];
    char *res = realpath(pathname, real_pathname);
    FILE *ret = ori_fopen(pathname, mode);
    log("%s(\"%s\", \"%s\") = %p\n", __func__, res ? real_pathname : pathname, mode, ret);
    return ret;
}

FILE *fopen64(const char *pathname, const char *mode) {
    DLSYM(fopen64);
    char real_pathname[PATH_MAX];
    char *res = realpath(pathname, real_pathname);
    FILE *ret = ori_fopen64(pathname, mode);
    log("%s(\"%s\", \"%s\") = %p\n", __func__, res ? real_pathname : pathname, mode, ret);
    return ret;
}

int fclose(FILE *stream) {
    DLSYM(fclose);
    static char link_dest[PATH_MAX];
    getFileNameByFd(fileno(stream), link_dest);
    int ret = ori_fclose(stream);
    log("%s(\"%s\") = %d\n", __func__, link_dest, ret);
    return ret;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    DLSYM(fread);
    char char_buf[32];
    static char link_dest[PATH_MAX];
    charBuf((char *)ptr, char_buf);
    getFileNameByFd(fileno(stream), link_dest);
    size_t ret = ori_fread(ptr, size, nmemb, stream);
    log("%s(\"%s\", %zu, %zu, \"%s\") = %zu\n", __func__, char_buf, size, nmemb, link_dest, ret);
    return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    DLSYM(fwrite);
    char char_buf[32];
    static char link_dest[PATH_MAX];
    charBuf((char *)ptr, char_buf);
    getFileNameByFd(fileno(stream), link_dest);
    size_t ret = ori_fwrite(ptr, size, nmemb, stream);
    log("%s(\"%s\", %zu, %zu, \"%s\") = %zu\n", __func__, char_buf, size, nmemb, link_dest, ret);
    return ret;
}

int open(const char *pathname, int flags, ...) {
    DLSYM(open);
    char real_pathname[PATH_MAX];
    va_list args;
    va_start(args, flags);
    mode_t mode = va_arg(args, mode_t);
    int ret;
    char *res = realpath(pathname, real_pathname);
 
    if ((flags & O_CREAT) != O_CREAT && (flags & O_TMPFILE) != O_TMPFILE) {
        ret = ori_open(pathname, flags);
        log("%s(\"%s\", %o) = %d\n", __func__, res ? real_pathname : pathname, flags, ret);
    } else {
        ret = ori_open(pathname, flags, mode);
        log("%s(\"%s\", %o, %o) = %d\n", __func__, res ? real_pathname : pathname, flags, mode, ret);
    }
    va_end(args);
    return ret;
}

int open64(const char *pathname, int flags, ...) {
    DLSYM(open64);
    char real_pathname[PATH_MAX];
    va_list args;
    va_start(args, flags);
    mode_t mode = va_arg(args, mode_t);
    int ret;
    char *res = realpath(pathname, real_pathname);

    if ((flags & O_CREAT) != O_CREAT && (flags & O_TMPFILE) != O_TMPFILE) {
        ret = ori_open64(pathname, flags);
        log("%s(\"%s\", %o) = %d\n", __func__, res ? real_pathname : pathname, flags, ret);
    } else {
        ret = ori_open64(pathname, flags, mode);
        log("%s(\"%s\", %o, %o) = %d\n", __func__, res ? real_pathname : pathname, flags, mode, ret);
    }
    va_end(args);
    return ret;
}

int close(int fd) {
    DLSYM(close);
    static char link_dest[PATH_MAX];
    getFileNameByFd(fd, link_dest);
    int ret = ori_close(fd);
    log("%s(\"%s\") = %d\n", __func__, link_dest, ret);
    return ret;
}

ssize_t read(int fd, void *buf, size_t count) {
    DLSYM(read);
    char char_buf[32];
    static char link_dest[PATH_MAX];
    charBuf((char *)buf, char_buf);
    getFileNameByFd(fd, link_dest);
    ssize_t ret = ori_read(fd, buf, count);
    log("%s(\"%s\", \"%s\", %zu) = %zd\n", __func__, link_dest, char_buf, count, ret);
    return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
    DLSYM(write);
    char char_buf[32];
    static char link_dest[PATH_MAX];
    charBuf((char *)buf, char_buf);
    getFileNameByFd(fd, link_dest);
    ssize_t ret = ori_write(fd, buf, count);
    log("%s(\"%s\", \"%s\", %zu) = %zd\n", __func__, link_dest, char_buf, count, ret);
    return ret;
}

int remove(const char *path) {
    DLSYM(remove);
    char real_path[PATH_MAX];
    char *res = realpath(path, real_path);
    int ret = ori_remove(path);
    log("%s(\"%s\") = %d\n", __func__, res ? real_path : path, ret);
    return ret;
}

int rename(const char *oldpath, const char *newpath) {
    DLSYM(rename);
    char real_oldpath[PATH_MAX], real_newpath[PATH_MAX];
    char *res1 = realpath(oldpath, real_oldpath);
    char *res2 = realpath(newpath, real_newpath);
    int ret = ori_rename(oldpath, newpath);
    log("%s(\"%s\", \"%s\") = %d\n", __func__, res1 ? real_oldpath : oldpath, res2 ? real_newpath : newpath, ret);
    return ret;
}

FILE *tmpfile(void) {
    DLSYM(tmpfile);
    FILE *ret = ori_tmpfile();
    log("%s() = %p\n", __func__, ret);
    return ret;
}

FILE *tmpfile64(void) {
    DLSYM(tmpfile64);
    FILE *ret = ori_tmpfile64();
    log("%s() = %p\n", __func__, ret);
    return ret;
}