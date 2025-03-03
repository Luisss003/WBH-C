#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cairo/cairo.h>

#define MAX_PATH 260
#define BMP_WIDTH 516
#define BMP_HEIGHT 532
#define HIST_WIDTH 512
#define HIST_HEIGHT 512
#define MAX_BMP_SIZE (1024 * 1024 * 1024) // 1GB limit

// Type definitions
typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef uint64_t QWORD;
typedef struct { BYTE blu, grn, red; } RGB;

typedef struct __attribute__((packed)) {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct __attribute__((packed)) {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

// Global variables
BYTE gFileReadBuffer[32 * 1024 * 1024];
RGB gPalette[256];
BITMAPFILEHEADER gBM_FileHeader;
BITMAPINFOHEADER gBM_InfoHeader;
QWORD gHistogram[256] = {0};

void generate_bar_graph(const char *filename) {
    int width = 800, height = 600;
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);

    QWORD max_count = 0;
    for (int i = 0; i < 256; i++) {
        if (gHistogram[i] > max_count) {
            max_count = gHistogram[i];
        }
    }

    for (int i = 0; i < 256; i++) {
        double bar_height = (double)gHistogram[i] / max_count * (height - 50);
        cairo_rectangle(cr, i * (width / 256.0), height - bar_height, width / 256.0 - 1, bar_height);
        cairo_fill(cr);
    }

    cairo_surface_write_to_png(surface, filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

void initBM_Header() {
    gBM_FileHeader.bfType = 0x4D42;
    gBM_FileHeader.bfSize = 0;
    gBM_FileHeader.bfReserved1 = 0;
    gBM_FileHeader.bfReserved2 = 0;
    gBM_FileHeader.bfOffBits = 0x436;

    gBM_InfoHeader.biSize = 40;
    gBM_InfoHeader.biWidth = 0;
    gBM_InfoHeader.biHeight = 0;
    gBM_InfoHeader.biPlanes = 1;
    gBM_InfoHeader.biBitCount = 8;
    gBM_InfoHeader.biCompression = 0;
    gBM_InfoHeader.biSizeImage = 0;
    gBM_InfoHeader.biXPelsPerMeter = 0;
    gBM_InfoHeader.biYPelsPerMeter = 0;
    gBM_InfoHeader.biClrUsed = 256;
    gBM_InfoHeader.biClrImportant = 0;
}

void analyze_file(char *binFile) {
    FILE *fptr = fopen(binFile, "rb");
    if (!fptr) {
        printf("Error - Could not open target file %s for reading in analyze_file.\n\n", binFile);
        return;
    }

    int ch;
    while ((ch = fgetc(fptr)) != EOF) {
        gHistogram[ch]++;
    }
    fclose(fptr);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    initBM_Header();
    analyze_file(argv[1]);
    generate_bar_graph("histogram.png");
    return 0;
}

