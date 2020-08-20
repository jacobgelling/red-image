/*
 * Red Image
 * MIT License
 * Copyright (c) 2020 Jacob Gelling
 */

#ifndef REDIMAGE_ARCHIVE_H
#define REDIMAGE_ARCHIVE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "gifenc.h"
#include "gifdec.h"

int col_to_gif(FILE *file_pointer, const char *gif_path);
int mph_to_gif(FILE *file_pointer, const char *gif_path);
int raw_to_gif(FILE *file_pointer, const char *gif_path);
int tm_to_gif(FILE *file_pointer, const char *palette_path, const char *gif_path);

int image_to_gif(const char *image_path, const char *palette_path, const char *gif_path);
int embedded_image_to_gif(const char *image_path, const char *gif_path);

int gif_to_col(gd_GIF *gif, const char *image_path);
int gif_to_mph(gd_GIF *gif, const char *image_path);
int gif_to_raw(gd_GIF *gif, const char *image_path);
int gif_to_tm(gd_GIF *gif, const char *palette_path, const char *image_path);

int gif_to_image(const char *gif_path, const char *palette_path, const char *image_path);
int gif_to_embedded_image(const char *gif_path, const char *image_path);

int read_palette(uint8_t *palette, FILE *palette_pointer);
int read_palette_from_file(uint8_t *palette, const char *palette_path);

#endif
