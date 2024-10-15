#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    char *filters = "b:e:grysi";
    int filter;

    // Ensure proper usage
    if (argc < 4){
        printf("Usage: ./filter [flag] infile outfile\n");
        printf("Available filters:\n");
        printf("    -y:  force symmetry (beta)\n");
        printf("    -g:  grayscale\n");
        printf("    -r:  reflect\n");
        printf("    -s:  sepia\n");
        printf("    -i:  invert\n");
        printf("    -b [surround]:  blur\n");
        printf("    -e [threshold]:  edge detection\n");
        return 3;
    }

    // Remember filenames
    char *infile = argv[argc - 2];
    char *outfile = argv[argc - 1];

    // Open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 4;
    }

    // Open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 5;
    }

    // Read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 6;
    }

    // Get image's dimensions
    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    // Allocate memory for image
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }

    while ((filter = getopt(argc, argv, filters)) != -1){
        switch (filter){
            case 'b':
                blur(height, width, image, optarg);
                break;
            case 'e':
                edges(height, width, image, optarg);
                break;
            case 'g':
                grayscale(height, width, image);
                break;
            case 'r':
                reflect(height, width, image);
                break;
            case 'y':
                symmetry(height, width, image);
                break;
            case 's':
                sepia(height, width, image);
                break;
            case 'i':
                invert(height, width, image);
                break;
            default:
                printf("invalid filter\n");
                return 1;
                break;
        }
    }

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // Write row to outfile
        fwrite(image[i], sizeof(RGBTRIPLE), width, outptr);

        // Write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // Free memory for image
    free(image);

    // Close files
    fclose(inptr);
    fclose(outptr);
    return 0;
}
