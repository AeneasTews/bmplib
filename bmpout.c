#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "bmpout.h"

// calculate number of bytes per row needed for multiple of 4 padding
int calcPaddingBytes(int width) {
    char padding = 4 - (width * sizeof(Pixel) % 4);
    return (padding != 4) ? padding : 0;
}

// calculate total number of bytes (not just requested pixels, but including padding)
int calcRowSizeBytes(int width) {
    return width * sizeof(Pixel) + calcPaddingBytes(width);
}

Pixel* createPixelArray(int width, int height) {
    return malloc(width * height * sizeof(Pixel));
}

int writePixelArray(Pixel* pixels, int width, int height, FILE* file) {
    if (file == NULL) {
        fprintf(stderr, "Error opening file whilst trying to write pixel data");
        return 1;
    }

    const uint8_t numPaddingBytes = calcPaddingBytes(width);
    uint8_t padding[numPaddingBytes];
    for (int i = 0; i < numPaddingBytes; i++) {
        padding[i] = 0;
    }

    printf("Debug: size of padding: %ld\n", sizeof(padding));
    for (int i = 0; i < height; i++) {
        fwrite(&pixels[i * width], sizeof(Pixel), width, file);
        fwrite(padding, sizeof(padding), 1, file);
    }

    return 0;
}

void initBMPHeader(BMPHeader* header, int width, int height) {
    header->type     = 0x4D42;
    header->size     = sizeof(BMPHeader) + sizeof(DIBHeader) + (calcRowSizeBytes(width) * height);
    header->reserved = 0;
    header->offset   = sizeof(BMPHeader) + sizeof(DIBHeader);
}

int writeBMPHeader(const BMPHeader* header, FILE* file) {
    if (file == NULL) {
        fprintf(stderr, "Error opening file whilst trying to write BMP Header");
        return 1;
    }

    fwrite(header, sizeof(BMPHeader), 1, file);

    return 0;
}

void initDIBHeader(DIBHeader* header, int width, int height) {
    header->headerSize       = sizeof(DIBHeader);
    header->width            = width;
    header->height           = height;
    header->planes           = 1;
    header->bitsPerPixel     = sizeof(Pixel) * 8;
    header->compression      = 0; // None
    header->imageSize        = 0; // dummy size (can be used with no compression)
    header->xPixelsPerMeter  = 100;
    header->yPixelsPerMeter  = 100;
    header->colorsUsed       = 0; // default to 2**n
    header->importantColors  = 0; // default every color
}

int writeDIBHeader(const DIBHeader* header, FILE* file) {
    if (file == NULL) {
        fprintf(stderr, "Error opening file whilst trying to write DIB Header");
        return 1;
    }

    fwrite(header, sizeof(DIBHeader), 1, file);

    return 0;
}

int writeBMP(const char* filename, int width, int height, Pixel* pixels) {
    printf("Writing BMP file: %s\n", filename);
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        return 1;
    }

    puts("Creating BMP Header...");
    BMPHeader bmpHeader;
    initBMPHeader(&bmpHeader, width, height);
    if (writeBMPHeader(&bmpHeader, file) != 0) {
        fclose(file);
        return 1;
    }

    puts("Creating DIB Header...");
    DIBHeader dibHeader;
    initDIBHeader(&dibHeader, width, height);
    if (writeDIBHeader(&dibHeader, file) != 0) {
        fclose(file);
        return 1;
    }

    puts("Writing pixel data...");
    if(writePixelArray(pixels, width, height, file) != 0) {
        return 1;
    }
    
    puts("Complete!");
    fclose(file);
    return 0;
}

void printPixel(Pixel* p) {
    printf("(%d %d %d)", p->red, p->green, p->blue);
}

void setPixelToRGB(Pixel* pixels, uint8_t red, uint8_t green, uint8_t blue, int x, int y, int width) {
    pixels[y * width + x] = Pixel(blue, green, red);
}