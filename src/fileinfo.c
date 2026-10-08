#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>

#include "fileinfo.h"

int get_file_info(const char *path, FileInfo *info)
{
    struct stat st;

    if (stat(path, &st) != 0)
    {
        return -1;
    }

    info->mode = st.st_mode;
    info->links = st.st_nlink;
    info->uid = st.st_uid;
    info->gid = st.st_gid;
    info->size = st.st_size;
    info->mtime = st.st_mtime;
    info->ctime = st.st_ctime;
    info->atime = st.st_atime;
    info->inode = st.st_ino;
    info->blocks = st.st_blocks;

    return 0;
}

int get_link_info(const char *path, FileInfo *info)
{
    struct stat st;

    if (lstat(path, &st) != 0)
    {
        return -1;
    }

    info->mode = st.st_mode;
    info->links = st.st_nlink;
    info->uid = st.st_uid;
    info->gid = st.st_gid;
    info->size = st.st_size;
    info->mtime = st.st_mtime;
    info->ctime = st.st_ctime;
    info->atime = st.st_atime;
    info->inode = st.st_ino;
    info->blocks = st.st_blocks;

    return 0;
}

int is_directory(const char *path)
{
    FileInfo info;

    if (get_file_info(path, &info) != 0)
    {
        return 0;
    }

    return S_ISDIR(info.mode);
}

int list_file(const char *path, const Options *options)
{
    FileInfo info;
    char display_name[PATH_MAX];
    char suffix;

    if (get_link_info(path, &info) != 0)
    {
        perror(path);
        return -1;
    }

    format_filename(
        path,
        options,
        display_name,
        sizeof(display_name)
    );

    if (options->blocks)
    {
        if (options->human_readable)
        {
            char block_size[32];

            format_size(
                get_display_blocks(
                    &info,
                    options->kilobytes,
                    options->human_readable
                ),
                1,
                block_size,
                sizeof(block_size)
            );

            printf("%7s ", block_size);
        }
        else
        {
            printf(
                "%4lld ",
                get_display_blocks(
                    &info,
                    options->kilobytes,
                    options->human_readable
                )
            );
        }
    }

    if (options->inode)
    {
        printf("%lu ", (unsigned long)info.inode);
    }

    if (options->long_format)
    {
        print_long_format(path, display_name, options);
    }
    else
    {
        printf("%s", display_name);

        if (options->classify)
        {
            suffix = get_file_type_suffix(path);

            if (suffix != '\0')
            {
                printf("%c", suffix);
            }
        }

        printf("\n");
    }

    return 0;
}

char get_file_type_suffix(const char *path)
{
    FileInfo info;

    if (get_link_info(path, &info) != 0)
    {
        return '\0';
    }

    if (S_ISLNK(info.mode))
    {
        return '@';
    }

    if (S_ISDIR(info.mode))
    {
        return '/';
    }

    if (S_ISFIFO(info.mode))
    {
        return '|';
    }

    if (S_ISSOCK(info.mode))
    {
        return '=';
    }

    if (info.mode & (S_IXUSR | S_IXGRP | S_IXOTH))
    {
        return '*';
    }

    return '\0';
}

void format_permissions(mode_t mode, char *buffer)
{
    /* Encode the file type followed by owner, group, and other permissions. */
    if (S_ISDIR(mode))
    {
        buffer[0] = 'd';
    }
    else if (S_ISLNK(mode))
    {
        buffer[0] = 'l';
    }
    else if (S_ISREG(mode))
    {
        buffer[0] = '-';
    }
    else if (S_ISCHR(mode))
    {
        buffer[0] = 'c';
    }
    else if (S_ISBLK(mode))
    {
        buffer[0] = 'b';
    }
    else if (S_ISFIFO(mode))
    {
        buffer[0] = 'p';
    }
    else if (S_ISSOCK(mode))
    {
        buffer[0] = 's';
    }
    else
    {
        buffer[0] = '?';
    }

    buffer[1] = (mode & S_IRUSR) ? 'r' : '-';
    buffer[2] = (mode & S_IWUSR) ? 'w' : '-';

    if (mode & S_ISUID)
    {
        buffer[3] = (mode & S_IXUSR) ? 's' : 'S';
    }
    else
    {
        buffer[3] = (mode & S_IXUSR) ? 'x' : '-';
    }

    buffer[4] = (mode & S_IRGRP) ? 'r' : '-';
    buffer[5] = (mode & S_IWGRP) ? 'w' : '-';

    if (mode & S_ISGID)
    {
        buffer[6] = (mode & S_IXGRP) ? 's' : 'S';
    }
    else
    {
        buffer[6] = (mode & S_IXGRP) ? 'x' : '-';
    }

    buffer[7] = (mode & S_IROTH) ? 'r' : '-';
    buffer[8] = (mode & S_IWOTH) ? 'w' : '-';

    if (mode & S_ISVTX)
    {
        buffer[9] = (mode & S_IXOTH) ? 't' : 'T';
    }
    else
    {
        buffer[9] = (mode & S_IXOTH) ? 'x' : '-';
    }

    buffer[10] = '\0';
}

void print_long_format(
    const char *path,
    const char *display_name,
    const Options *options
)
{
    FileInfo info;
    char permissions[11];
    struct passwd *user;
    struct group *group;
    char time_buffer[64];
    struct tm *time_info;
    char suffix;
    char owner[32];
    char group_name[32];
    char size_buffer[32];

    if (get_link_info(path, &info) != 0)
    {
        perror(path);
        return;
    }

    format_permissions(info.mode, permissions);

    user = getpwuid(info.uid);
    group = getgrgid(info.gid);

    if (options->use_ctime)
    {
        time_info = localtime(&info.ctime);
    }
    else if (options->use_atime)
    {
        time_info = localtime(&info.atime);
    }
    else
    {
        time_info = localtime(&info.mtime);
    }

    if (time_info != NULL)
    {
        strftime(
            time_buffer,
            sizeof(time_buffer),
            "%b %d %H:%M",
            time_info
        );
    }
    else
    {
        snprintf(
            time_buffer,
            sizeof(time_buffer),
            "??? ?? ??:??"
        );
    }

    if (options->inode)
    {
        printf("%lu ", (unsigned long)info.inode);
    }

    suffix = '\0';

    if (options->classify)
    {
        suffix = get_file_type_suffix(path);
    }

    if (options->numeric)
    {
        snprintf(
            owner,
            sizeof(owner),
            "%lu",
            (unsigned long)info.uid
        );

        snprintf(
            group_name,
            sizeof(group_name),
            "%lu",
            (unsigned long)info.gid
        );
    }
    else
    {
        snprintf(
            owner,
            sizeof(owner),
            "%s",
            user != NULL ? user->pw_name : "unknown"
        );

        snprintf(
            group_name,
            sizeof(group_name),
            "%s",
            group != NULL ? group->gr_name : "unknown"
        );
    }

    format_size(
        info.size,
        options->human_readable,
        size_buffer,
        sizeof(size_buffer)
    );

    if (S_ISLNK(info.mode))
    {
        char link_target[PATH_MAX];
        ssize_t link_length;

        link_length = readlink(
            path,
            link_target,
            sizeof(link_target) - 1
        );

        if (link_length >= 0)
        {
            link_target[link_length] = '\0';

            printf(
                "%s %2lu %s %s %8s %s %s%c -> %s\n",
                permissions,
                (unsigned long)info.links,
                owner,
                group_name,
                size_buffer,
                time_buffer,
                display_name,
                suffix,
                link_target
            );
        }
        else
        {
            printf(
                "%s %2lu %s %s %8s %s %s%c\n",
                permissions,
                (unsigned long)info.links,
                owner,
                group_name,
                size_buffer,
                time_buffer,
                display_name,
                suffix
            );
        }
    }
    else
    {
        printf(
            "%s %2lu %s %s %8s %s %s%c\n",
            permissions,
            (unsigned long)info.links,
            owner,
            group_name,
            size_buffer,
            time_buffer,
            display_name,
            suffix
        );
    }
}

long long get_display_blocks(
    const FileInfo *info,
    int kilobytes,
    int human_readable
)
{
    long long bytes;

    /* POSIX st_blocks counts allocated storage in 512-byte units. */
    bytes = (long long)info->blocks * 512LL;

    if (human_readable)
    {
        /* The caller formats these allocated bytes with format_size(). */
        return bytes;
    }

    if (kilobytes)
    {
        return (bytes + 1023) / 1024;
    }

    return (long long)info->blocks;
}

void format_size(
    off_t size,
    int human_readable,
    char *buffer,
    size_t buffer_size
)
{
    if (!human_readable)
    {
        snprintf(
            buffer,
            buffer_size,
            "%lld",
            (long long)size
        );

        return;
    }

    if (size < 1024)
    {
        snprintf(
            buffer,
            buffer_size,
            "%lld",
            (long long)size
        );
    }
    else if (size < 1024 * 1024)
    {
        snprintf(
            buffer,
            buffer_size,
            "%.1fK",
            (double)size / 1024.0
        );
    }
    else if (size < 1024LL * 1024LL * 1024LL)
    {
        snprintf(
            buffer,
            buffer_size,
            "%.1fM",
            (double)size / (1024.0 * 1024.0)
        );
    }
    else
    {
        snprintf(
            buffer,
            buffer_size,
            "%.1fG",
            (double)size / (1024.0 * 1024.0 * 1024.0)
        );
    }
}

void format_filename(
    const char *name,
    const Options *options,
    char *buffer,
    size_t buffer_size
)
{
    size_t i;
    size_t j;

    if (buffer_size == 0)
    {
        return;
    }

    /* -w preserves the filename without replacing non-printable characters. */
    if (options->raw)
    {
        snprintf(buffer, buffer_size, "%s", name);
        return;
    }

    /* -q replaces non-printable characters; otherwise copy the name unchanged. */
    j = 0;

    for (i = 0; name[i] != '\0' && j + 1 < buffer_size; i++)
    {
        unsigned char c;

        c = (unsigned char)name[i];

        if (options->quote && !isprint(c))
        {
            buffer[j++] = '?';
        }
        else
        {
            buffer[j++] = name[i];
        }
    }

    buffer[j] = '\0';
}
