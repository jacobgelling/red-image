/*
 * Red Image
 * MIT License
 * Copyright (c) 2020 Jacob Gelling
 */

#include "cli.h"

int main(const int argc, char *argv[]) {
    switch (argc) {
        // No arguments provided
        case 1:
            printf("Red Image %d.%d\n", REDIMAGE_VERSION_MAJOR, REDIMAGE_VERSION_MINOR);
            printf("MIT License\n");
            printf("Copyright (c) 2020 Jacob Gelling\n\n");
            printf("  To decode a image into a GIF:\n");
            printf("  %s -d image palette gif\n\n", argv[0]);
            printf("  To encode a GIF into a image:\n");
            printf("  %s -e gif palette image\n", argv[0]);
            break;

        // Correct number of arguments provided for external palette
        case 5:
            if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--decode") == 0) {
                if (image_to_gif(argv[2], argv[3], argv[4]) != 1) {
                    return EXIT_FAILURE;
                }
            } else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--encode") == 0) {
                if (gif_to_image(argv[2], argv[3], argv[4]) != 1) {
                    return EXIT_FAILURE;
                }
            } else {
                fprintf(stderr, "Unknown option %s\n", argv[1]);
                return EXIT_FAILURE;
            }
            break;

        // Correct number of arguments provided for embedded palette
        case 4:
            if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--decode") == 0) {
                if (embedded_image_to_gif(argv[2], argv[3]) != 1) {
                    return EXIT_FAILURE;
                }
            } else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--encode") == 0) {
                if (gif_to_embedded_image(argv[2], argv[3]) != 1) {
                    return EXIT_FAILURE;
                }
            } else {
                fprintf(stderr, "Unknown option %s\n", argv[1]);
                return EXIT_FAILURE;
            }
            break;

        // Incorrect number of arguments provided
        default:
            fprintf(stderr, "Incorrect number of arguments\n");
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
