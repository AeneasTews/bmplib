#include <stdio.h>

#include "bmpout.h"

int main() {
    const char* filename = "output.bmp";
    int width = 5000;
    int height = 5000;

    Pixel* pixels = createPixelArray(width, height);

    // write test values
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double redFraction = ((float) x / (width - 1));
            double greenFraction = ((float) y / (height - 1));
            double blueFraction = ((float) ((width + height) - (x + y)) / (width + height - 2));
            setPixelToRGB(pixels, 255.999f * redFraction, 255.999f * greenFraction, 255.999f * blueFraction, x, y, width);
            
            if (300 > y && y > 200) {
                setPixelToRGB(pixels, 0, 0, 0, x, y, width);
            }

            if (300 > x && x > 200) {
                setPixelToRGB(pixels, 0, 0, 0, x, y, width);
            }
        }
    }

    if (writeBMP(filename, width, height, pixels) != 0) {
        fprintf(stderr, "Error writing BMP file\n");
        //freePixelArray(pixels, height);
        return 1;
    }

    //freePixelArray(pixels, height);
    return 0;
}