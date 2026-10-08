#ifndef SORTING_H
#define SORTING_H

#include "options.h"

typedef struct
{
    char *name;
    off_t size;
    time_t mtime;
} SortEntry;

void sort_entries(SortEntry entries[], int count, const Options *options);

#endif
