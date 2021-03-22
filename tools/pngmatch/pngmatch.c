// SPDX-License-Identifier: MIT
//
// Copyright (c) 2011-2015, 2019-2020, Antonio Niño Díaz

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

#if !defined(PNG_SIMPLIFIED_READ_SUPPORTED)
# error "This code needs libpng 1.6"
#endif

// Load a PNG file into a RGBA buffer
int Read_PNG(const char *filename, unsigned char **_buffer,
             int *_width, int *_height)
{
    png_image image;

    // Only the image structure version number needs to be set
    memset(&image, 0, sizeof image);
    image.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_file(&image, filename))
    {
        printf("%s(): png_image_begin_read_from_file(): %s",
               __func__, image.message);
        return 1;
    }

    image.format = PNG_FORMAT_RGBA;

    png_bytep buffer;
    buffer = malloc(PNG_IMAGE_SIZE(image));

    if (buffer == NULL)
    {
        png_image_free(&image);
        printf("%s(): Not enough memory", __func__);
        return 1;
    }

    if (!png_image_finish_read(&image, NULL, buffer, 0, NULL))
    {
        printf("%s(): png_image_finish_read(): %s", __func__, image.message);
        free(buffer);
        return 1;
    }

    *_buffer = buffer;
    *_width = image.width;
    *_height = image.height;

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s image1.png image2.png\n"
               "Return: 0 if the images match, 1 if they don't.\n",
               argv[0]);

        return 2;
    }

    int ret = 1;

    unsigned char *buffer1 = NULL;
    unsigned char *buffer2 = NULL;
    int width1, width2;
    int height1, height2;

    if (Read_PNG(argv[1], &buffer1, &width1, &height1) != 0)
    {
        printf("Failed to read %s\n", argv[1]);
        goto cleanup;
    }

    if (Read_PNG(argv[2], &buffer2, &width2, &height2) != 0)
    {
        printf("Failed to read %s\n", argv[1]);
        goto cleanup;
    }

    if ((width1 != width2) || (height1 != height2))
    {
        // Images are different sizes, which means that the user probably made a
        // mistake when passing the paths.
        printf("Sizes are different\n");
        goto cleanup;
    }

    size_t size = width1 * height1 * 4;

    if (memcmp(buffer1, buffer2, size) != 0)
    {
        // The content of the images is different
        goto cleanup;
    }

    // Success
    ret = 0;

cleanup:

    free(buffer1);
    free(buffer2);

    return ret;
}
