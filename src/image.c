/*
 * Red Image
 * MIT License
 * Copyright (c) 2020 Jacob Gelling
 */

#include "image.h"

static size_t get_file_size(FILE *file_pointer) {
    fseek(file_pointer, 0, SEEK_END);
    const size_t file_size = ftell(file_pointer);
    fseek(file_pointer, 0, SEEK_SET);
    return file_size;
}

int image_to_gif(const char *image_path, const char *palette_path, const char *gif_path) {
    // Open image file
    FILE *image_pointer = fopen(image_path, "rb");
    if (image_pointer == NULL) {
        fprintf(stderr, "Error opening image\n");
        return EXIT_FAILURE;
    }

    // Get image size
    const size_t image_size = get_file_size(image_pointer);
    uint16_t image_width = 0;
    uint16_t image_height = 0;
    switch (image_size) {
        // .TM image
        case 49152:
            image_width = 256;
            image_height = 192;
            break;

        // .RAW image (check width and height are correct)
        // case 64768:
        //     image_width = 320;
        //     image_height = 200;
        //     break;

        // .MPH heightmap
        // case 65536:
        //     image_width = 256;
        //     image_height = 256;
        //     break;

        // .COL colour palette
        // case 768:
        //     image_width = 16;
        //     image_height = 16;
        //     break;

        default:
            fclose(image_pointer);
            fprintf(stderr, "Unsupported image size\n");
            return EXIT_FAILURE;
    }

    // Read image data file
    uint8_t *image_data = malloc(image_size);
    if (fread(image_data, image_size, 1, image_pointer) != 1) {
        fclose(image_pointer);
        free(image_data);
        fprintf(stderr, "Could not read image\n");
        return EXIT_FAILURE;
    }
    fclose(image_pointer);

    // Open palette file
    FILE *palette_pointer = fopen(palette_path, "rb");
    if (palette_pointer == NULL) {
        free(image_data);
        fprintf(stderr, "Error opening palette\n");
        return EXIT_FAILURE;
    }

    // Check palette size
    const size_t palette_size = get_file_size(palette_pointer);
    if(palette_size != 768) {
        fclose(palette_pointer);
        free(image_data);
        fprintf(stderr, "Unsupported palette size\n");
        return EXIT_FAILURE;
    }

    // Read palette file
    uint8_t palette[768];
    if (fread(&palette, 768, 1, palette_pointer) != 1) {
        fclose(palette_pointer);
        free(image_data);
        fprintf(stderr, "Could not read colour palette\n");
        return EXIT_FAILURE;
    }
    fclose(palette_pointer);

    // Scale palette colour values
    for(int i = 0; i < 768; i++) {
        // Check colour is valid
        if(palette[i] >= 64) {
            free(image_data);
            fprintf(stderr, "Invalid colour in colour palette\n");
            return EXIT_FAILURE;
        }

        // Multiply by 4 to scale to 256 colours
        palette[i] *= 4;
    }

    // Create GIF
    ge_GIF *gif = ge_new_gif(gif_path, image_width, image_height, palette, 8, -1);
    if(gif == NULL) {
        free(image_data);
        fprintf(stderr, "Could not create gif\n");
        return EXIT_FAILURE;
    }
    memcpy(gif->frame, image_data, image_width * image_height);
    free(image_data);
    ge_add_frame(gif, 0);
    ge_close_gif(gif);

    return EXIT_SUCCESS;
}

int gif_to_image(const char *gif_path, const char *palette_path, const char *image_path) {
    // Open gif
    gd_GIF *gif = gd_open_gif(gif_path);
    if(gif == NULL) {
        fprintf(stderr, "Error opening gif\n");
        return EXIT_FAILURE;
    }

    // Check gif
    // todo this better
    switch (gif->width * gif->height) {
        // .TM image
        case 49152:
            break;

        default:
            gd_close_gif(gif);
            fprintf(stderr, "Unsupported gif size\n");
            return EXIT_FAILURE;
    }

    // Check gif frame
    gd_get_frame(gif);
    if(gif->frame == NULL) {
        gd_close_gif(gif);
        fprintf(stderr, "Unsupported gif frame\n");
        return EXIT_FAILURE;
    }

    // Get gif colour palette

    // Convert gif frame to image
    uint8_t *image_data = malloc(gif->width * gif->height);
    memcpy(image_data, gif->frame, gif->width * gif->height);

    // Open image
    FILE *image_pointer = NULL;
    if ((image_pointer = fopen(image_path, "wb")) == NULL) {
        fprintf(stderr, "Error creating image file\n");
        return EXIT_FAILURE;
    }

    // Write image data to file
    int write_status = fwrite(image_data, gif->width * gif->height, 1, image_pointer);
    fclose(image_pointer);
    free(image_data);
    if (write_status != 1) {
        fprintf(stderr, "Error writing image data to file\n");
        return EXIT_FAILURE;
    }

    // Close and return
    gd_close_gif(gif);
    return EXIT_SUCCESS;
}
