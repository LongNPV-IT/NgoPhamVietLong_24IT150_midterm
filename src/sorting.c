#include <stdlib.h>
#include <string.h>

#include "sorting.h"

static int compare_name(
    const SortEntry *a,
    const SortEntry *b
)
{
    return strcmp(a->name, b->name);
}

static int compare_size(
    const SortEntry *a,
    const SortEntry *b
)
{
    if (a->size < b->size)
    {
        return 1;
    }

    if (a->size > b->size)
    {
        return -1;
    }

    return strcmp(a->name, b->name);
}

static int compare_time(
    const SortEntry *a,
    const SortEntry *b
)
{
    if (a->mtime < b->mtime)
    {
        return 1;
    }

    if (a->mtime > b->mtime)
    {
        return -1;
    }

    return strcmp(a->name, b->name);
}

static int compare_entries(
    const void *a,
    const void *b,
    const Options *options
)
{
    const SortEntry *entry_a;
    const SortEntry *entry_b;
    int result;

    entry_a = (const SortEntry *)a;
    entry_b = (const SortEntry *)b;

    if (options->sort_size)
    {
        result = compare_size(entry_a, entry_b);
    }
    else if (options->sort_time)
    {
        result = compare_time(entry_a, entry_b);
    }
    else
    {
        result = compare_name(entry_a, entry_b);
    }

    if (options->reverse)
    {
        result = -result;
    }

    return result;
}

static const Options *current_options;

static int qsort_compare(
    const void *a,
    const void *b
)
{
    return compare_entries(a, b, current_options);
}

void sort_entries(
    SortEntry entries[],
    int count,
    const Options *options
)
{
    if (options->no_sort || count <= 1)
    {
        return;
    }

    current_options = options;

    qsort(
        entries,
        count,
        sizeof(SortEntry),
        qsort_compare
    );

    current_options = NULL;
}
