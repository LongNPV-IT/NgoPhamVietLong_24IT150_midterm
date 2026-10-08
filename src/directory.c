#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fileinfo.h"
#include "options.h"
#include "sorting.h"

#define MAX_ENTRIES 1024

static void print_directory_header(const char *path)
{
    printf("%s:\n", path);
}

static void print_total_blocks(
    const char *path,
    SortEntry entries[],
    int count,
    const Options *options
)
{
    long long total_blocks = 0;
    int i;

    if (!options->blocks)
    {
        return;
    }

    if (!isatty(STDOUT_FILENO))
    {
        return;
    }

    for (i = 0; i < count; i++)
    {
        FileInfo info;
        char full_path[PATH_MAX];

        if (strcmp(path, ".") == 0)
        {
            snprintf(
                full_path,
                sizeof(full_path),
                "./%s",
                entries[i].name
            );
        }
        else
        {
            snprintf(
                full_path,
                sizeof(full_path),
                "%s/%s",
                path,
                entries[i].name
            );
        }

        if (get_link_info(full_path, &info) == 0)
        {
            total_blocks += info.blocks;
        }
    }

    if (options->human_readable)
    {
        char total_buffer[32];

        format_size(
            total_blocks * 512LL,
            1,
            total_buffer,
            sizeof(total_buffer)
        );

        printf("total %s\n", total_buffer);
    }
    else if (options->kilobytes)
    {
        printf(
            "total %lld\n",
            (total_blocks * 512LL + 1023) / 1024
        );
    }
    else
    {
        printf("total %lld\n", total_blocks);
    }
}

int list_directory(const char *path, const Options *options)
{
    DIR *dir;
    struct dirent *entry;

    SortEntry entries[MAX_ENTRIES];
    int count = 0;
    int i;

    dir = opendir(path);

    if (dir == NULL)
    {
        perror(path);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        FileInfo info;
        char full_path[PATH_MAX];

        /* -a includes all dotfiles; -A excludes only "." and "..". */
        if (entry->d_name[0] == '.' && !options->all)
        {
            if (!options->almost_all ||
                strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
        }

        if (count >= MAX_ENTRIES)
        {
            fprintf(
                stderr,
                "Too many entries in directory\n"
            );
            break;
        }

        if (strcmp(path, ".") == 0)
        {
            snprintf(
                full_path,
                sizeof(full_path),
                "./%s",
                entry->d_name
            );
        }
        else
        {
            snprintf(
                full_path,
                sizeof(full_path),
                "%s/%s",
                path,
                entry->d_name
            );
        }

        if (get_link_info(full_path, &info) != 0)
        {
            fprintf(
                stderr,
                "%s: cannot get file information\n",
                full_path
            );
            continue;
        }

        entries[count].name =
            malloc(strlen(entry->d_name) + 1);

        if (entries[count].name == NULL)
        {
            perror("malloc");
            break;
        }

        strcpy(
            entries[count].name,
            entry->d_name
        );

        entries[count].size = info.size;

        /* Sort by the timestamp selected by -c/-u, or by mtime by default. */
        if (options->use_ctime)
        {
            entries[count].mtime = info.ctime;
        }
        else if (options->use_atime)
        {
            entries[count].mtime = info.atime;
        }
        else
        {
            entries[count].mtime = info.mtime;
        }

        count++;
    }

    closedir(dir);

    sort_entries(entries, count, options);

    print_total_blocks(
        path,
        entries,
        count,
        options
    );

    for (i = 0; i < count; i++)
    {
        char full_path[PATH_MAX];
        char display_name[PATH_MAX];
        FileInfo info;

        format_filename(
            entries[i].name,
            options,
            display_name,
            sizeof(display_name)
        );

        if (strcmp(path, ".") == 0)
        {
            snprintf(
                full_path,
                sizeof(full_path),
                "./%s",
                entries[i].name
            );
        }
        else
        {
            snprintf(
                full_path,
                sizeof(full_path),
                "%s/%s",
                path,
                entries[i].name
            );
        }

        if (get_link_info(full_path, &info) != 0)
        {
            fprintf(
                stderr,
                "%s: cannot get file information\n",
                full_path
            );

            free(entries[i].name);
            continue;
        }

        if (options->long_format)
        {
            print_long_format(
                full_path,
                display_name,
                options
            );
        }
        else
        {
            char suffix;

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
                printf(
                    "%lu ",
                    (unsigned long)info.inode
                );
            }

            printf(
                "%s",
                display_name
            );

            if (options->classify)
            {
                suffix =
                    get_file_type_suffix(full_path);

                if (suffix != '\0')
                {
                    printf("%c", suffix);
                }
            }

            printf("\n");
        }
    }

    if (options->recursive)
    {
        for (i = 0; i < count; i++)
        {
            char full_path[PATH_MAX];
            FileInfo info;

            if (strcmp(path, ".") == 0)
            {
                snprintf(
                    full_path,
                    sizeof(full_path),
                    "./%s",
                    entries[i].name
                );
            }
            else
            {
                snprintf(
                    full_path,
                    sizeof(full_path),
                    "%s/%s",
                    path,
                    entries[i].name
                );
            }

            if (get_link_info(full_path, &info) != 0)
            {
                continue;
            }

            if (S_ISDIR(info.mode))
            {
                /* These entries may be displayed, but must never be traversed. */
                if (strcmp(entries[i].name, ".") == 0 ||
                    strcmp(entries[i].name, "..") == 0)
                {
                    continue;
                }

                printf("\n");

                print_directory_header(full_path);

                list_directory(full_path, options);
            }
        }
    }

    for (i = 0; i < count; i++)
    {
        free(entries[i].name);
    }

    return 0;
}
