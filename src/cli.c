/*
 * Red Image
 * MIT License
 * Copyright (c) 2020 Jacob Gelling
 */

#include "cli.h"

int main(const int argc, char *argv[]) {
    // Set return code to success
    int return_code = EXIT_SUCCESS;

    switch (argc) {
        // No arguments provided
        case 1:
            printf("Red Image %d.%d\n", REDIMAGE_VERSION_MAJOR, REDIMAGE_VERSION_MINOR);
            printf("MIT License\n");
            printf("Copyright (c) 2020 Jacob Gelling\n\n");
            printf("  To decode a image into a GIF:\n");
            printf("  %s -d image gif palette\n\n", argv[0]);
            printf("  To encode a GIF into a image:\n");
            printf("  %s -e gif image palette\n", argv[0]);
            break;

        // Correct number of arguments provided
        case 5:
            if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--decode") == 0) {
                return_code = EXIT_FAILURE;
            } else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--encode") == 0) {
                return_code = EXIT_FAILURE;
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

    return return_code;
}
