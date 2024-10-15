#include "helpers.h"#include <math.h>#include <stdio.h>#include <stdlib.h>// Convert image to grayscalevoid grayscale(int height, int width, RGBTRIPLE image[height][width]){    int average;    for (int column = 0; column < height; column++){        for (int row = 0; row < width; row++){            average = round((image[column][row].rgbtBlue + image[column][row].rgbtGreen + image[column][row].rgbtRed) / 3.0);            image[column][row].rgbtBlue = average;            image[column][row].rgbtGreen = average;            image[column][row].rgbtRed = average;        }    }    return;}//force it to be symetric :Dvoid symmetry(int height, int width, RGBTRIPLE image[height][width]){    for (int row = 0; row < height; row++){        for (int column = 0; column < width; column++){            image[row][column] = image[row][width - column - 1];        }    }    return;}// Reflect image horizontallyvoid reflect(int height, int width, RGBTRIPLE image[height][width]){    for (int row = 0; row < height; row++){        for (int column = 0; column < width/2; column++){            //swap the pixels            RGBTRIPLE reflection = image[row][width - column - 1];            image[row][width - column - 1] = image[row][column];            image[row][column] = reflection;        }    }    return;}// Blur imagevoid blur(int height, int width, RGBTRIPLE image[height][width], char *surroundstr){    RGBTRIPLE blurred[height][width];    RGBTRIPLE average;    int surround = atoi(surroundstr);    if (surround < 1){        printf("surround value must be a positive integer\n");        return;    }    int sumBlue;    int sumGreen;    int sumRed;    float count;    for (int row = 0; row < height; row++){        for (int column = 0; column < width; column++){            count = pow(surround, 2);            sumBlue = 0;            sumGreen = 0;            sumRed = 0;            for (int rowOff = -1; rowOff < (surround - 1); rowOff++){                for (int colOff = -1; colOff < (surround - 1); colOff++){                    if ((row + rowOff) < 0){                        count--;                        continue;                    }                    if ((row + rowOff) > (height - 1)){                        count--;                        continue;                    }                    if ((column + colOff) < 0){                        count--;                        continue;                    }                    if ((column + colOff) > (width - 1)){                        sumBlue += 0;                        sumGreen += 0;                        sumRed += 0;                        count --;                        continue;                    }                    sumBlue += image[row + rowOff][column + colOff].rgbtBlue;                    sumGreen += image[row + rowOff][column + colOff].rgbtGreen;                    sumRed += image[row + rowOff][column + colOff].rgbtRed;                }            }            average.rgbtBlue = round((sumBlue / count > 255)?255:(sumBlue / count));            average.rgbtGreen = round((sumGreen / count > 255)?255:(sumGreen / count));            average.rgbtRed = round((sumRed / count > 255)?255:(sumRed / count));            blurred[row][column] = average;        }    }    for (int row = 0; row < height; row ++){        for (int column = 0; column < width; column++){            image[row][column] = blurred[row][column];        }    }    return;}// Detect edgesvoid edges(int height, int width, RGBTRIPLE image[height][width], char *thresholdstr){    int threshold = atoi(thresholdstr);    RGBTRIPLE edged[height][width];    int kernel[2][3][3] = {{{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}, {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}}}; // [x, y][row][column]    int sums[2][3]; //SUMS OF GX AND GY: [gx, gy][blue, greed, red]    for (int row = 0; row < height; row++){        for (int col = 0; col < width; col++){            for (int i = 0; i < 2; i++){                for (int j = 0; j < 3; j++){                        sums[i][j] = 0;                }            }            for (int direction = 0; direction < 2; direction++){                for (int rowoff = -1; rowoff < 2; rowoff++){                    for (int coloff = -1; coloff < 2; coloff++){                        if ((row + rowoff) < 0 || (row + rowoff) > (height - 1) || (col + coloff) < 0 || (col + coloff) > (width - 1)){                            for (int color = 0; color < 3; color++){                                sums[direction][color] += 0;                            }                            continue;                        }                        sums[direction][0] += (kernel[direction][rowoff + 1][coloff + 1] * image[row + rowoff][col + coloff].rgbtBlue);                        sums[direction][1] += (kernel[direction][rowoff + 1][coloff + 1] * image[row + rowoff][col + coloff].rgbtGreen);                        sums[direction][2] += (kernel[direction][rowoff + 1][coloff + 1] * image[row + rowoff][col + coloff].rgbtRed);                    }                }            }            edged[row][col].rgbtBlue = (round(sqrt(pow(sums[0][0], 2) + pow(sums[1][0], 2))) > 255)?255:round(sqrt(pow(sums[0][0], 2) + pow(sums[1][0], 2)));            edged[row][col].rgbtGreen = (round(sqrt(pow(sums[0][1], 2) + pow(sums[1][1], 2))) > 255)?255:round(sqrt(pow(sums[0][1], 2) + pow(sums[1][1], 2)));            edged[row][col].rgbtRed = (round(sqrt(pow(sums[0][2], 2) + pow(sums[1][2], 2))) > 255)?255:round(sqrt(pow(sums[0][2], 2) + pow(sums[1][2], 2)));        }    }    for (int row = 0; row < height; row ++){        for (int column = 0; column < width; column++){            image[row][column] = edged[row][column];        }    }    if (threshold > 0 && threshold < 256){        for (int row = 0; row < height; row++){            for (int col = 0; col < width; col++){                if ((image[row][col].rgbtBlue >= threshold) || (image[row][col].rgbtGreen >= threshold) || (image[row][col].rgbtRed >= threshold)){                    image[row][col].rgbtBlue = 255;                    image[row][col].rgbtGreen = 255;                    image[row][col].rgbtRed = 255;                }                else{                    image[row][col].rgbtBlue = 0;                    image[row][col].rgbtGreen = 0;                    image[row][col].rgbtRed = 0;                }            }        }    }    return;}void sepia(int height, int width, RGBTRIPLE image[height][width]){    for (int row = 0; row < height; row++){        for (int col = 0; col < width; col++){            image[row][col].rgbtBlue = (round((.272 * image[row][col].rgbtRed) + (.534 * image[row][col].rgbtGreen) + (.131 * image[row][col].rgbtBlue)) > 255)?                                        255:                                        round((.272 * image[row][col].rgbtRed) + (.534 * image[row][col].rgbtGreen) + (.131 * image[row][col].rgbtBlue));            image[row][col].rgbtGreen = (round((.349 * image[row][col].rgbtRed) + (.686 * image[row][col].rgbtGreen) + (.168 * image[row][col].rgbtBlue)) > 255)?                                         255:                                         round((.349 * image[row][col].rgbtRed) + (.686 * image[row][col].rgbtGreen) + (.168 * image[row][col].rgbtBlue));            image[row][col].rgbtRed = (round((.393 * image[row][col].rgbtRed) + (.769 * image[row][col].rgbtGreen) + (.189 * image[row][col].rgbtBlue)) > 255)?                                       255:                                       round((.393 * image[row][col].rgbtRed) + (.769 * image[row][col].rgbtGreen) + (.189 * image[row][col].rgbtBlue));        }    }    return;}void invert(int height, int width, RGBTRIPLE image[height][width]){    for (int row = 0; row < height; row++){        for (int col = 0; col < width; col++){            image[row][col].rgbtBlue = (255 - image[row][col].rgbtBlue);            image[row][col].rgbtGreen = (255 - image[row][col].rgbtGreen);            image[row][col].rgbtRed = (255 - image[row][col].rgbtRed);        }    }    return;}