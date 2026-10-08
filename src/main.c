#include <stdio.h>

#include "options.h"
#include "directory.h"
#include "fileinfo.h"

int main(int argc, char *argv[])
{
    Options options;
    int i;
    int operand_count = 0;
    int had_error = 0;

    init_options(&options);

    if (parse_options(argc, argv, &options) != 0)
    {
        return 1;
    }

    /* Count path arguments so directory headings can match ls-style output. */
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            operand_count++;
        }
    }

    if (operand_count == 0)
    {
        if (options.directory)
        {
            if (list_file(".", &options) != 0)
            {
                had_error = 1;
            }
        }
        else
        {
            if (options.recursive)
            {
                printf(".:\n");
            }

            if (list_directory(".", &options) != 0)
            {
                had_error = 1;
            }
        }

        return had_error;
    }

    {
        int printed_directory = 0;

        for (i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                continue;
            }

            if (options.directory)
            {
                if (list_file(argv[i], &options) != 0)
                {
                    had_error = 1;
                }
            }
            else if (is_directory(argv[i]))
            {
                /* Add a heading when multiple operands are being listed. */
                if (options.recursive || operand_count > 1)
                {
                    if (printed_directory)
                    {
                        printf("\n");
                    }

                    printf("%s:\n", argv[i]);
                }

                if (list_directory(argv[i], &options) != 0)
                {
                    had_error = 1;
                }

                printed_directory = 1;
            }
            else
            {
                if (list_file(argv[i], &options) != 0)
                {
                    had_error = 1;
                }
            }
        }
    }

    return had_error;
}