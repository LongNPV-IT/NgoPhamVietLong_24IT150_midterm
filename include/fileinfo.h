#ifndef FILEINFO_H
#define FILEINFO_H

#include <sys/types.h>
#include <time.h>

#include "options.h"

typedef struct
{
    mode_t mode;
    nlink_t links;
    uid_t uid;
    gid_t gid;
    off_t size;
    time_t mtime;
    time_t ctime;
    time_t atime;
    ino_t inode;
    blkcnt_t blocks;
} FileInfo;

int get_file_info(const char *path, FileInfo *info);

int get_link_info(const char *path, FileInfo *info);

int is_directory(const char *path);

int list_file(const char *path, const Options *options);

char get_file_type_suffix(const char *path);

void format_permissions(mode_t mode, char *buffer);

void print_long_format(const char *path, const char *display_name,
                       const Options *options);

long long get_display_blocks(
    const FileInfo *info,
    int kilobytes,
    int human_readable
);

void format_size(off_t size, int human_readable,
                 char *buffer, size_t buffer_size);

void format_filename(
    const char *name,
    const Options *options,
    char *buffer,
    size_t buffer_size
);

#endif
