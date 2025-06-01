#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef BMPOUT_H
#define BMPOUT_H
typedef union {
    struct {
        uint8_t blue, green, red;
    };
    struct {
        uint8_t b, g, r;
    };
    uint8_t data[3];
} Pixel;

#define Pixel(args...) (Pixel) { .data = { args, }}

#pragma pack(push, 1)
typedef struct {
    int16_t type;
    uint32_t size;
    uint32_t reserved;
    uint32_t offset;
} BMPHeader;

typedef struct {
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t importantColors;
} DIBHeader;
#pragma pack(pop)

// calculate number of bytes per row needed for multiple of 4 padding
extern int calcPaddingBytes(int width);

// calculate total number of bytes per row (not just requested pixels, but including padding)
extern int calcRowSizeBytes(int width);

// creating and writing pixel array
extern Pixel* createPixelArray(int width, int height);

extern int writePixelArray(Pixel* pixels, int width, int height, FILE* file);

// creating and writing Bmp header with adequate and default values
extern void initBMPHeader(BMPHeader* header, int width, int height);

extern int writeBMPHeader(const BMPHeader* header, FILE* file);

// ceating and writing DIB header with adequate and default values
extern void initDIBHeader(DIBHeader* header, int width, int height);

extern int writeDIBHeader(const DIBHeader* header, FILE* file);

// writing BMP file
extern int writeBMP(const char* filename, int width, int height, Pixel* pixels);

// utility functions
extern void printPixel(Pixel* p);

extern void setPixelToRGB(Pixel* pixels, uint8_t red, uint8_t green, uint8_t blue, int x, int y, int width);
#endif // BMPOUT_H
