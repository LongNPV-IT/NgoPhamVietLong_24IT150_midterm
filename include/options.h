#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct
{
    /* Visibility and output format. */
    int all;
    int almost_all;
    int directory;
    int classify;
    int inode;
    int long_format;
    int numeric;
    int human_readable;
    int kilobytes;
    int blocks;

    /* Sorting and timestamp selection. */
    int no_sort;
    int reverse;
    int sort_size;
    int sort_time;

    int use_ctime;
    int use_atime;
    int recursive;

    /* Filename rendering. */
    int quote;
    int raw;
} Options;

void init_options(Options *options);
int parse_options(int argc, char *argv[], Options *options);

#endif
