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

int image_to_gif(const char *image_path, const char *palette_path, const char *gif_path);
int gif_to_image(const char *gif_path, const char *palette_path, const char *image_path);

#endif
