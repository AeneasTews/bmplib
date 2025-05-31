#include <stdio.h>
#include <stdlib.h>

typedef struct Pixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} Pixel;

#pragma pack(push, 1)
// default BMP header
typedef struct BMPHeader {
    short type;
    unsigned int size;
    unsigned int reserved;
    unsigned int offset;
} BMPHeader;

// windows bitmap info header
typedef struct DIBHeader {
    unsigned int headerSize;
    signed int width;
    signed int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int imageSize;
    signed int xPixelsPerMeter;
    signed int yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int importantColors;
} DIBHeader;

typedef struct BMPFile {
    BMPHeader bmpHeader;
    DIBHeader dibHeader;
    Pixel** pixels;
} BMPFile;
#pragma pack(pop)

// calculate the number of bytes per row used for padding
int calcPaddingBytes(int width) {
    char padding = 4 - (width * sizeof(Pixel) % 4);
    return (padding != 4) ? padding : 0;
}

// source: wikipedia (bmp file requires multiple of 4 calculated using this formula)
// calculates number of bytes not pixel
int calcRowSizeBytes(int width) {
    return width * sizeof(Pixel) + calcPaddingBytes(width);
}

// simple function to allocate required space for pixel array
Pixel** createPixelArray(int width, int height) {
    Pixel** arr = malloc(height * sizeof(Pixel*));
    for (int i = 0; i < height; i++) {
        arr[i] = malloc (width * sizeof(Pixel));
    }

    return arr;
}

// free pixel array
void freePixelArray(Pixel** arr, int height) {
    for (int i = 0; i < height; i++) {
        free(arr[i]);
    }

    free(arr);
}

int writePixelArray(Pixel** pixels, int width, int height, FILE* file) {
    if (file == NULL) {
        fprintf(stderr, "Error opening file whilst trying to write pixel data");
        return 1;
    }

    unsigned char padding[calcPaddingBytes(width)];
    for (int i = 0; i < calcPaddingBytes(width); i++) {
        padding[i] = 0x0;
    }

    printf("Debug: size of padding: %d\n", sizeof(padding));
    for (int i = 0; i < height; i++) {
        fwrite(pixels[i], sizeof(Pixel), width, file);
        fwrite(padding, sizeof(padding), 1, file);
    }

    return 0;
}

// create default values for bmp header
BMPHeader* createBMPHeader(int width, int height) {
    BMPHeader* bmpHeader = malloc(sizeof(BMPHeader));
    if (!bmpHeader) {
        fprintf(stderr, "Memory allocation failed for BMPHeader\n");
        return NULL;
    }

    bmpHeader->type = 0x4D42; // magic number
    //printf("Debug: total file size in bmp header: %d\n", sizeof(BMPHeader) + sizeof(DIBHeader) + (calcRowSizeBytes(width) * height));
    bmpHeader->size = sizeof(BMPHeader) + sizeof(DIBHeader) + (calcRowSizeBytes(width) * height); // total file size
    bmpHeader->reserved = 0; // nothing
    bmpHeader->offset = sizeof(BMPHeader) + sizeof(DIBHeader); // starting offset of pixel array
    return bmpHeader;
}

int writeBMPHeader(BMPHeader* header, FILE* file) {
    if (file == NULL) {
        fprintf(stderr, "Error opening file whilst trying to write BMP Header");
        return 1;
    }

    fwrite(header, sizeof(*header), 1, file);

    return 0;
}

// create bmp file header according to old windows standard
DIBHeader* createDIBHeader(int width, int height) {
    DIBHeader* dibHeader = malloc(sizeof(DIBHeader));

    dibHeader->headerSize = 40;
    dibHeader->width = width;
    dibHeader->height = height;
    dibHeader->planes = 1; // default 1
    dibHeader->bitsPerPixel = sizeof(Pixel) * 8;
    dibHeader->compression = 0; // none
    dibHeader->imageSize = 0; // dummy zero for BI_RGB (no compression)
    dibHeader->xPixelsPerMeter = 100;
    dibHeader->yPixelsPerMeter = 100;
    dibHeader->colorsUsed = 0; // default to 2**n
    dibHeader->importantColors = 0; // default (every color)
    return dibHeader;
}

int writeDIBHeader(DIBHeader* header, FILE* file) {
    if (file == NULL) {
        fprintf(stderr, "Error opening file whilst trying to write DIB Header");
        return 1;
    }

    fwrite(header, sizeof(*header), 1, file);

    return 0;
}

int writeBMP(const char* filename, int width, int height, Pixel** pixels) {
    printf("Writing BMP file: %s\n", filename);
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        return 1;
    }

    puts("Creating BMP Header...");
    BMPHeader* bmpHeader = createBMPHeader(width, height);
    //puts("Writing BMP Header...");
    //printf("Debug: BMP Header size: %d\n", sizeof(*bmpHeader));
    if (writeBMPHeader(bmpHeader, file) != 0) {
        fclose(file);
        return 1;
    }
    //puts("Complete!");
    
    puts("Creating DIB Header...");
    DIBHeader* dibHeader = createDIBHeader(width, height);
    //puts("Writing DIB Header...");
    if (writeDIBHeader(dibHeader, file) != 0) {
        fclose(file);
        return 1;
    }
    //puts("Complete!");

    //printf("Debug: sizeof(bmpHeader): %d, sizeof(dibHeader): %d, sizeof(outputPixels): %d\n", sizeof(*bmpHeader), sizeof(*dibHeader), sizeof(pixels));
    
    puts("Writing pixel data...");
    if (writePixelArray(pixels, width, height, file) != 0) {
        fclose(file);
        return 1;
    }
    puts("Complete!");
    
    fclose(file);
    return 0;
}

void printPixel(Pixel p) {
    printf("(%d %d %d)", p.red, p.green, p.blue);
}

// values are blue, green, red
void setPixelToPixel(Pixel** pixelArray, Pixel pixel, int x, int y) {
    pixelArray[y][x] = pixel;
}

void setPixelToRGB(Pixel** pixelArray, unsigned char red, unsigned char green, unsigned char blue, int x, int y) {
    pixelArray[y][x] = (Pixel){blue, green, red};
}

int main() {
    const char* filename = "output.bmp";
    int width = 500;
    int height = 500;

    Pixel** pixels = createPixelArray(width, height);

    // write test values
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double redFraction = ((double) i / (width - 1));
            double greenFraction = ((double) j / (height - 1));
            double blueFraction = ((double) ((width + height) - (i + j)) / (width + height - 2));
            setPixelToRGB(pixels, (unsigned char) 255 * redFraction, 255 * greenFraction, 255 * blueFraction, i, j);
        }
    }

    if (writeBMP(filename, width, height, pixels) != 0) {
        fprintf(stderr, "Error writing BMP file\n");
        freePixelArray(pixels, height);
        return 1;
    }

    freePixelArray(pixels, height);
    return 0;
}