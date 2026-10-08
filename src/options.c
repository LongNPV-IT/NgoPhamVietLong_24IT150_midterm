#include <stdio.h>
#include <string.h>

#include "options.h"

void init_options(Options *options)
{
    options->all = 0;
    options->almost_all = 0;
    options->directory = 0;
    options->classify = 0;
    options->inode = 0;
    options->long_format = 0;
    options->numeric = 0;
    options->human_readable = 0;
    options->kilobytes = 0;
    options->blocks = 0;
    options->no_sort = 0;
    options->reverse = 0;
    options->sort_size = 0;
    options->sort_time = 0;
    options->use_ctime = 0;
    options->use_atime = 0;
    options->recursive = 0;
    options->quote = 0;
    options->raw = 0;
}

int parse_options(int argc, char *argv[], Options *options)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            continue;
        }

        if (strcmp(argv[i], "--") == 0)
        {
            break;
        }

        if (strcmp(argv[i], "-a") == 0)
        {
            options->all = 1;
            options->almost_all = 0;
        }
        else if (strcmp(argv[i], "-A") == 0)
        {
            options->almost_all = 1;
            options->all = 0;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            options->directory = 1;
            options->recursive = 0;
        }
        else if (strcmp(argv[i], "-F") == 0)
        {
            options->classify = 1;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            options->inode = 1;
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            options->long_format = 1;
            options->numeric = 0;
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            options->long_format = 1;
            options->numeric = 1;
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            options->human_readable = 1;
            options->kilobytes = 0;
        }
        else if (strcmp(argv[i], "-k") == 0)
        {
            options->kilobytes = 1;
            options->human_readable = 0;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            options->blocks = 1;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            options->no_sort = 1;
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
            options->reverse = 1;
        }
        else if (strcmp(argv[i], "-S") == 0)
        {
            options->sort_size = 1;
            options->sort_time = 0;
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            options->sort_time = 1;
            options->sort_size = 0;
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            options->use_ctime = 1;
            options->use_atime = 0;
        }
        else if (strcmp(argv[i], "-u") == 0)
        {
            options->use_atime = 1;
            options->use_ctime = 0;
        }
        else if (strcmp(argv[i], "-R") == 0)
        {
            options->recursive = 1;
            options->directory = 0;
        }
        else if (strcmp(argv[i], "-q") == 0)
        {
            options->quote = 1;
            options->raw = 0;
        }
        else if (strcmp(argv[i], "-w") == 0)
        {
            options->raw = 1;
            options->quote = 0;
        }
        else
        {
            printf("Unknown option: %s\n", argv[i]);
            return -1;
        }
    }

    return 0;
}
