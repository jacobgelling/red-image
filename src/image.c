/*
 * Red Image
 * MIT License
 * Copyright (c) 2020 Jacob Gelling
 */

#include "image.h"

// Set compile-time constants
#define COL_SIZE 768
#define TM_SIZE 49152
#define RAW_SIZE 64768
#define MPH_SIZE 65536

static size_t get_file_size(FILE *file_pointer) {
    fseek(file_pointer, 0, SEEK_END);
    const size_t file_size = ftell(file_pointer);
    fseek(file_pointer, 0, SEEK_SET);
    return file_size;
}

static int write_gif(const char *gif_path, const uint8_t *image_data, const uint16_t image_width, const uint16_t image_height, uint8_t *palette) {
    ge_GIF *gif = ge_new_gif(gif_path, image_width, image_height, palette, 8, -1);
    if(gif == NULL) {
        return 0;
    }
    memcpy(gif->frame, image_data, image_width * image_height);
    ge_add_frame(gif, 0);
    ge_close_gif(gif);
    return 1;
}

int col_to_gif(FILE *file_pointer, const char *gif_path) {
    // Read embedded colour palette
    uint8_t *palette = malloc(COL_SIZE);
    if (read_palette(palette, file_pointer) != 1) {
        fclose(file_pointer);
        free(palette);
        return 0;
    }
    fclose(file_pointer);

    // Create image data
    uint8_t *image_data = malloc(256);
    for (int i = 0; i < 256; i++) {
        image_data[i] = i;
    }

    // Create GIF
    if (write_gif(gif_path, image_data, 16, 16, palette) != 1) {
        free(palette);
        free(image_data);
        fprintf(stderr, "Could not create gif\n");
        return 0;
    }
    free(palette);
    free(image_data);

    return 1;
}

int mph_to_gif(FILE *file_pointer, const char *gif_path) {
    // Create greyscale colour palette
    uint8_t *palette = malloc(COL_SIZE);
    for (int i = 0; i < 256; i++) {
        const int j = i * 3;
        palette[j] = i;
        palette[j + 1] = i + 1;
        palette[j + 2] = i + 2;
    }

    // Read image data
    uint8_t *image_data = malloc(MPH_SIZE);
    if (fread(image_data, MPH_SIZE, 1, file_pointer) != 1) {
        fclose(file_pointer);
        free(palette);
        free(image_data);
        fprintf(stderr, "Could not read image\n");
        return 0;
    }
    fclose(file_pointer);

    // Write GIF
    if (write_gif(gif_path, image_data, 256, 256, palette) != 1) {
        free(palette);
        free(image_data);
        fprintf(stderr, "Could not create gif\n");
        return 0;
    }
    free(palette);
    free(image_data);

    return 1;
}

int raw_to_gif(FILE *file_pointer, const char *gif_path) {
    // Read embedded colour palette
    uint8_t *palette = malloc(COL_SIZE);
    if (read_palette(palette, file_pointer) != 1) {
        fclose(file_pointer);
        free(palette);
        return 0;
    }

    // Read image data
    uint8_t *image_data = malloc(RAW_SIZE - COL_SIZE);
    if (fread(image_data, RAW_SIZE - COL_SIZE, 1, file_pointer) != 1) {
        fclose(file_pointer);
        free(palette);
        free(image_data);
        fprintf(stderr, "Could not read image\n");
        return 0;
    }
    fclose(file_pointer);

    // Write GIF
    if (write_gif(gif_path, image_data, 320, 200, palette) != 1) {
        free(palette);
        free(image_data);
        fprintf(stderr, "Could not create gif\n");
        return 0;
    }
    free(palette);
    free(image_data);

    return 1;
}

int tm_to_gif(FILE *file_pointer, const char *palette_path, const char *gif_path) {
    // Read external colour palette
    uint8_t *palette = malloc(COL_SIZE);
    if (read_palette_from_file(palette, palette_path) != 1) {
        free(palette);
        return 0;
    }

    // Read image data
    uint8_t *image_data = malloc(TM_SIZE);
    if (fread(image_data, TM_SIZE, 1, file_pointer) != 1) {
        fclose(file_pointer);
        free(palette);
        free(image_data);
        fprintf(stderr, "Could not read image\n");
        return 0;
    }
    fclose(file_pointer);

    // Write GIF
    if (write_gif(gif_path, image_data, 256, 192, palette) != 1) {
        free(palette);
        free(image_data);
        fprintf(stderr, "Could not create gif\n");
        return 0;
    }
    free(palette);
    free(image_data);

    return 1;
}

int image_to_gif(const char *image_path, const char *palette_path, const char *gif_path) {
    // Open image file
    FILE *image_pointer = fopen(image_path, "rb");
    if (image_pointer == NULL) {
        fprintf(stderr, "Error opening image\n");
        return 0;
    }

    // Get file size to determine file type
    const size_t file_size = get_file_size(image_pointer);
    switch (file_size) {
        // .TM image
        case TM_SIZE:
            return tm_to_gif(image_pointer, palette_path, gif_path);

        default:
            fclose(image_pointer);
            fprintf(stderr, "Unsupported image type or size\n");
            return 0;
    }

    return 1;
}

int embedded_image_to_gif(const char *image_path, const char *gif_path) {
    // Open image file
    FILE *image_pointer = fopen(image_path, "rb");
    if (image_pointer == NULL) {
        fprintf(stderr, "Error opening image\n");
        return 0;
    }

    // Get file size to determine file type
    const size_t file_size = get_file_size(image_pointer);
    switch (file_size) {
        // .COL colour palette
        case COL_SIZE:
            return col_to_gif(image_pointer, gif_path);

        // .MPH heightmap
        case MPH_SIZE:
            return mph_to_gif(image_pointer, gif_path);

        // .RAW image
        case RAW_SIZE:
            return raw_to_gif(image_pointer, gif_path);

        default:
            fclose(image_pointer);
            fprintf(stderr, "Unsupported image type or size\n");
            return 0;
    }

    return 1;
}

int gif_to_col(gd_GIF *gif, const char *image_path) {
    // Check palette size
    if (gif->palette->size != 256) {
        fprintf(stderr, "Unsupported colour palette size\n");
        return 0;
    }

    // Read colour palette
    uint8_t *palette = malloc(COL_SIZE);
    memcpy(palette, gif->palette->colors, COL_SIZE);
    gd_close_gif(gif);

    // Scale palette colour values
    for (int i = 0; i < COL_SIZE; i++) {
        // Divide by 4 to scale to 64 colours
        palette[i] /= 4;
    }

    // Open image
    FILE *image_pointer = NULL;
    if ((image_pointer = fopen(image_path, "wb")) == NULL) {
        free(palette);
        fprintf(stderr, "Error creating image file\n");
        return 0;
    }

    // Write image data to file
    int write_status = fwrite(palette, COL_SIZE, 1, image_pointer);
    fclose(image_pointer);
    free(palette);
    if (write_status != 1) {
        fprintf(stderr, "Error writing image data to file\n");
        return 0;
    }

    return 1;
}

int gif_to_mph(gd_GIF *gif, const char *image_path) {
    // Check palette size
    if (gif->palette->size != 256) {
        fprintf(stderr, "Unsupported colour palette size\n");
        return 0;
    }

    // Convert gif frame to image
    uint8_t *image_data = malloc(MPH_SIZE);
    memcpy(image_data, gif->frame, MPH_SIZE);
    gd_close_gif(gif);

    // Open image
    FILE *image_pointer = NULL;
    if ((image_pointer = fopen(image_path, "wb")) == NULL) {
        free(image_data);
        fprintf(stderr, "Error creating image file\n");
        return 0;
    }

    // Write image data to file
    int write_status = fwrite(image_data, MPH_SIZE, 1, image_pointer);
    fclose(image_pointer);
    free(image_data);
    if (write_status != 1) {
        fprintf(stderr, "Error writing image data to file\n");
        return 0;
    }

    return 1;
}

int gif_to_raw(gd_GIF *gif, const char *image_path) {
    // Check palette size
    if (gif->palette->size != 256) {
        fprintf(stderr, "Unsupported colour palette size\n");
        return 0;
    }

    // Read colour palette
    uint8_t *palette = malloc(COL_SIZE);
    memcpy(palette, gif->palette->colors, COL_SIZE);

    // Scale palette colour values
    for (int i = 0; i < COL_SIZE; i++) {
        // Divide by 4 to scale to 64 colours
        palette[i] /= 4;
    }

    // Convert gif frame to image
    uint8_t *image_data = malloc(RAW_SIZE - COL_SIZE);
    memcpy(image_data, gif->frame, RAW_SIZE - COL_SIZE);
    gd_close_gif(gif);

    // Open image
    FILE *image_pointer = NULL;
    if ((image_pointer = fopen(image_path, "wb")) == NULL) {
        free(image_data);
        fprintf(stderr, "Error creating image file\n");
        return 0;
    }

    // Write colour palette to file
    int write_status = fwrite(palette, COL_SIZE, 1, image_pointer);
    free(palette);
    if (write_status != 1) {
        fclose(image_pointer);
        fprintf(stderr, "Error writing image data to file\n");
        return 0;
    }

    // Write image data to file
    write_status = fwrite(image_data, RAW_SIZE - COL_SIZE, 1, image_pointer);
    fclose(image_pointer);
    free(image_data);
    if (write_status != 1) {
        fprintf(stderr, "Error writing image data to file\n");
        return 0;
    }

    return 1;
}

int gif_to_tm(gd_GIF *gif, const char *palette_path, const char *image_path) {
    // Check palette size
    if (gif->palette->size != 256) {
        fprintf(stderr, "Unsupported colour palette size\n");
        return 0;
    }

    // Convert gif frame to image
    uint8_t *image_data = malloc(TM_SIZE);
    memcpy(image_data, gif->frame, TM_SIZE);
    gd_close_gif(gif);

    // Open image
    FILE *image_pointer = NULL;
    if ((image_pointer = fopen(image_path, "wb")) == NULL) {
        free(image_data);
        fprintf(stderr, "Error creating image file\n");
        return 0;
    }

    // Write image data to file
    int write_status = fwrite(image_data, TM_SIZE, 1, image_pointer);
    fclose(image_pointer);
    free(image_data);
    if (write_status != 1) {
        fprintf(stderr, "Error writing image data to file\n");
        return 0;
    }

    return 1;
}

int gif_to_image(const char *gif_path, const char *palette_path, const char *image_path) {
    // Open gif file
    gd_GIF *gif = gd_open_gif(gif_path);
    if(gif == NULL) {
        fprintf(stderr, "Error opening gif\n");
        return 0;
    }

    // Check gif frame
    gd_get_frame(gif);
    if(gif->frame == NULL) {
        gd_close_gif(gif);
        fprintf(stderr, "Unsupported gif frame\n");
        return 0;
    }

    // Get file size to determine file type
    switch (gif->width * gif->height) {
        // .TM image
        case TM_SIZE:
            return gif_to_tm(gif, palette_path, image_path);

        default:
            gd_close_gif(gif);
            fprintf(stderr, "Unsupported gif size\n");
            return 0;
    }

    return 1;
}

int gif_to_embedded_image(const char *gif_path, const char *image_path) {
    // Open gif file
    gd_GIF *gif = gd_open_gif(gif_path);
    if(gif == NULL) {
        fprintf(stderr, "Error opening gif\n");
        return 0;
    }

    // Check gif frame
    gd_get_frame(gif);
    if(gif->frame == NULL) {
        gd_close_gif(gif);
        fprintf(stderr, "Unsupported gif frame\n");
        return 0;
    }

    // Get file size to determine file type
    switch (gif->width * gif->height) {
        // .COL colour palette
        case 256:
            return gif_to_col(gif, image_path);

        // .MPH heightmap
        case MPH_SIZE:
            return gif_to_mph(gif, image_path);

        // .RAW image
        case RAW_SIZE - COL_SIZE:
            return gif_to_raw(gif, image_path);

        default:
            gd_close_gif(gif);
            fprintf(stderr, "Unsupported gif size\n");
            return 0;
    }

    return 1;
}

int read_palette(uint8_t *palette, FILE *palette_pointer) {
    // Read palette file
    if (fread(palette, COL_SIZE, 1, palette_pointer) != 1) {
        fprintf(stderr, "Could not read colour palette\n");
        return 0;
    }

    // Scale palette colour values
    for(int i = 0; i < COL_SIZE; i++) {
        // Check colour is valid
        if(palette[i] >= 64) {
            fprintf(stderr, "Unsupported colour palette value\n");
            return 0;
        }

        // Multiply by 4 to scale to 256 colours
        palette[i] *= 4;
    }

    return 1;
}

int read_palette_from_file(uint8_t *palette, const char *palette_path) {
    // Open palette file
    FILE *palette_pointer = fopen(palette_path, "rb");
    if (palette_pointer == NULL) {
        fprintf(stderr, "Error opening colour palette\n");
        return 0;
    }

    // Check palette size
    if (get_file_size(palette_pointer) != COL_SIZE) {
        fclose(palette_pointer);
        fprintf(stderr, "Unsupported colour palette size\n");
        return 0;
    }

    // Read palette file
    if (read_palette(palette, palette_pointer) != 1) {
        fclose(palette_pointer);
        return 0;
    }
    fclose(palette_pointer);

    return 1;
}
