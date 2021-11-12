#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

unsigned char *file_to_image_buffer(char* filepath, int *width, int *height);
FILE *image_buffer_to_file(char *filepath, unsigned char *image, int width, int height);
bool file_has_extension(char *filename, char *extension);

int main(int argc, char **argv) {

    // We need 2 arguments
    if (argc != 3) {
        printf("Usage: imagify <inputfile> <outputfile>\nIf <inputfile> ends with '.png', it is decoded, otherwise encoded.\n");
        exit(1);
    }

    // File must exist
    char *outputpath = argv[2];
    char *filepath = argv[1];
    if (access(filepath, F_OK) != 0) {
        printf("ERROR - %s: File can not be accessed. Check whether it exists and you have permissions.\n", filepath);
        exit(1);
    }

    bool encode = !file_has_extension(filepath, ".png");
    if (encode) {

        // Read file into image buffer
        int width, height;
        unsigned char *image = file_to_image_buffer(filepath, &width, &height);
        // Write image buffer as png
        stbi_write_png(outputpath, width, height, 4, image, width * 4);
        free(image);

    } else {

        int width, height, channels;
        unsigned char *image = stbi_load(filepath, &width, &height, &channels, 0);
        if(image == NULL) {
            printf("ERROR - %s: Unable to parse image.\n", filepath);
        }
        if (channels != 4) {
            printf("ERROR - %s: Image does not have the required 4 channels (RGBA), it instead seems to have %d channels.\n", filepath, channels);
            exit(1);
        }
        FILE *ptr = image_buffer_to_file(outputpath, image, width, height);

        stbi_image_free(image);

    }

    return 0;
}

FILE *image_buffer_to_file(char *filepath, unsigned char *image, int width, int height) {
    FILE *file;
    file = fopen(filepath, "wb+");
    for (int i = 0; i < width * height; i++) {
        unsigned char a = image[4*i + 3];
        if (a == 0) break;
        if (a == 255) a = 3;
        for (int j = 0; j < a; j++) {
            fwrite(image + 4*i + j, 1, 1, file);
        }
    }
    fclose(file);
}

unsigned char *file_to_image_buffer(char* filepath, int *width, int *height) {
        // Open file
        FILE *ptr;
        ptr = fopen(filepath, "rb");
        // Determine file size
        fseek(ptr, 0, SEEK_END);
        int file_size = ftell(ptr);
        fseek(ptr, 0, SEEK_SET);
        // Make appropriate image dimension
        int image_dimension = (int) ceil(sqrt(file_size));
        // 4 bytes for each pixel: RGBA
        size_t buffer_size = image_dimension * image_dimension * 4;
        unsigned char *image = calloc(buffer_size, 1);
        // Read through the file
        unsigned char read_byte;
        long index = 0;
        while (!feof(ptr)) {
            // Skip alpha channel
            if (index % 4 == 3) image[index++] = (unsigned char) 255;
            // Turn each byte into a R, G or B value
            fread(&read_byte, sizeof(read_byte), 1, ptr);
            image[index++] = read_byte;
        }
        // First non 255 alpha channel denotes how many bytes in the last pixel
        int empty_bytes = 0;
        while (index % 4 != 3) index++; empty_bytes++;
        image[index] = 3 - empty_bytes;
        fclose(ptr);
        *width = image_dimension;
        *height = (int) ceil(((double) index / 4.0) / (double) image_dimension);
        return image;
}

bool file_has_extension(char *filename, char *extension) {
    char *dot = strrchr(filename, '.');
    if( dot != NULL ) {
        return strcmp(dot, extension) == 0;
    }
    return false;
}