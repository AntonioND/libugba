// SPDX-License-Identifier: MIT
//
// Copyright (c) 2014, 2019, 2020 Antonio Niño Díaz

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *search_string =
    "__attribute__((aligned(4))) __attribute__((visibility(\"hidden\")))";

#define MAX_PATH_LEN    2048

char temp_path[MAX_PATH_LEN];

void file_foad(const char *path, void **buffer, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        printf("%s couldn't be opened!", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    *size = ftell(f);

    if (*size == 0)
    {
        printf("Size of %s is 0!", path);
        fclose(f);
        exit(1);
    }

    rewind(f);
    *buffer = malloc(*size);
    if (*buffer == NULL)
    {
        printf("Not enought memory to load %s!", path);
        fclose(f);
        exit(1);
    }

    if (fread(*buffer, *size, 1, f) != 1)
    {
        printf("Error while reading.");
        fclose(f);
        exit(1);
    }

    fclose(f);
}

void generate_temp_file(char *file, size_t size, const char *path)
{
    FILE *ftemp = fopen(path, "w");
    if (ftemp == NULL)
    {
        printf("Can't open %s\n", temp_path);
        exit(1);
    }

    const char *c_header =
        "// File exported by Grit and modified by toncfix. Do not edit.\n"
        "\n"
        "#if defined(_MSC_VER)\n"
        "# define ALIGNED(x) __declspec(align(x))\n"
        "#else\n"
        "# define ALIGNED(x) __attribute__((aligned(x)))\n"
        "#endif\n"
        "\n";

    // First, save the cross-platform header
    fprintf(ftemp, "%s", c_header);

    while (size > 0)
    {
        if (strncmp(file, search_string, strlen(search_string)) == 0)
        {
            fprintf(ftemp, "ALIGNED(4) ");

            size_t len = strlen(search_string);
            file += len;
            size -= len;
        }

        char c = *file;
        fputc(c, ftemp);

        file++;
        size --;
    }

    fclose(ftemp);
}

int main(int argc, char **argv)
{
    void *file = NULL;
    size_t size;

    if (argc < 2)
    {
        printf("Invalid arguments.\n"
               "Usage: %s [file_in]\n", argv[0]);
        return 1;
    }

    const char *path_in = argv[1];

    // Generate temporary file path
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", path_in);

    file_foad(path_in, &file, &size);

    generate_temp_file(file, size, temp_path);

    free(file);

    remove(path_in);
    rename(temp_path, path_in);

    return 0;
}
