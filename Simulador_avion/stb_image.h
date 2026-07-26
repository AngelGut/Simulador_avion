/* stb_image - v2.27 - public domain image loader
   Simplified version for texture loading in this project
   Supports BMP and basic image formats
*/

#ifndef STB_IMAGE_H
#define STB_IMAGE_H

#define _CRT_SECURE_NO_WARNINGS

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;

stbi_uc* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp);
void stbi_image_free(void* retval_from_stbi_load);

#ifdef __cplusplus
}
#endif

#ifdef STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    unsigned short type;
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offset;
} BMPHeader;

typedef struct {
    unsigned int headerSize;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int imageSize;
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    unsigned int numColors;
    unsigned int importantColors;
} BMPInfoHeader;
#pragma pack(pop)

static unsigned char* stbi_load_bmp(const char* filename, int* x, int* y, int* comp, int req_comp) {
    FILE* f = NULL;
    fopen_s(&f, filename, "rb");
    if (!f) return NULL;

    BMPHeader header;
    if (fread(&header, sizeof(BMPHeader), 1, f) != 1) {
        fclose(f);
        return NULL;
    }

    if (header.type != 0x4D42) { // 'BM'
        fclose(f);
        return NULL;
    }

    BMPInfoHeader infoHeader;
    if (fread(&infoHeader, sizeof(BMPInfoHeader), 1, f) != 1) {
        fclose(f);
        return NULL;
    }

    int width = infoHeader.width;
    int height = infoHeader.height;
    if (height < 0) height = -height;
    int bpp = infoHeader.bitsPerPixel / 8;

    if (bpp < 3) {
        fclose(f);
        return NULL;
    }

    int dataSize = width * height * 4;
    unsigned char* data = (unsigned char*)malloc(dataSize);
    if (!data) {
        fclose(f);
        return NULL;
    }

    fseek(f, header.offset, SEEK_SET);

    for (int row = height - 1; row >= 0; row--) {
        for (int col = 0; col < width; col++) {
            unsigned char bgra[4] = {255, 255, 255, 255};
            if (fread(bgra, bpp, 1, f) != 1) {
                free(data);
                fclose(f);
                return NULL;
            }

            int idx = (row * width + col) * 4;
            data[idx + 0] = bgra[2];
            data[idx + 1] = bgra[1];
            data[idx + 2] = bgra[0];
            data[idx + 3] = (bpp == 4) ? bgra[3] : 255;
        }
    }

    fclose(f);

    *x = width;
    *y = height;
    *comp = 4;

    return data;
}

stbi_uc* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp) {
    if (!filename || !x || !y || !comp) {
        return NULL;
    }

    const char* ext = filename;
    while (*ext) ext++;
    while (ext > filename && ext[-1] != '.') ext--;

    if (strcmp(ext, "bmp") == 0 || strcmp(ext, "BMP") == 0) {
        return stbi_load_bmp(filename, x, y, comp, req_comp);
    }

    FILE* f = NULL;
    fopen_s(&f, filename, "rb");
    if (!f) return NULL;

    unsigned char magic[4];
    if (fread(magic, 1, 4, f) != 4) {
        fclose(f);
        return NULL;
    }
    fclose(f);

    // PNG magic bytes: 89 50 4E 47
    if (magic[0] == 0x89 && magic[1] == 0x50 && magic[2] == 0x4E && magic[3] == 0x47) {
        fprintf(stderr, "PNG support not compiled in. Use BMP files or compile with libpng.\n");
        return NULL;
    }

    // JPEG magic bytes: FF D8
    if (magic[0] == 0xFF && magic[1] == 0xD8) {
        fprintf(stderr, "JPEG support not compiled in. Use BMP files or compile with libjpeg.\n");
        return NULL;
    }

    // BMP magic bytes: 42 4D
    if (magic[0] == 0x42 && magic[1] == 0x4D) {
        return stbi_load_bmp(filename, x, y, comp, req_comp);
    }

    fprintf(stderr, "Unknown image format: %s\n", filename);
    return NULL;
}

void stbi_image_free(void* retval_from_stbi_load) {
    free(retval_from_stbi_load);
}

#endif // STB_IMAGE_IMPLEMENTATION

#endif // STB_IMAGE_H
