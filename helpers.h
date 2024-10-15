#include "bmp.h"

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width]);

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width]);

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width], char *thresholdstr);

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width], char *surroundstr);

//accidentally made it but it looked cool
void symmetry(int height, int width, RGBTRIPLE image[height][width]);

//sepia (from the less comfortable version of filter)
void sepia(int height, int width, RGBTRIPLE image[height][width]);

//invert the colors of the image
void invert(int height, int width, RGBTRIPLE image[height][width]);
