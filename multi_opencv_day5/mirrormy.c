#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <math.h>

#include "bmpHeader.h"

#define ubyte	unsigned char
#define BASE	15

#define widthbytes(bits)   (((bits)+31)/32*4)

void main(int argc, char** argv)
{
	FILE *fp;
	RGBQUAD *palrgb;

	unsigned int imagesize;
	char input[128], output[128];
 
	int i, j, size, index;

	unsigned char *inimg;
	unsigned char *outimg;
 
	strcpy(input, argv[1]); //argv[1]);
	strcpy(output, argv[2]); //argv[2]);
 
	if((fp = fopen(input, "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		exit(EXIT_FAILURE);
	}

	BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfoHeader;

    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	int elemsize = bmpInfoHeader.biBitCount/8;
	size= bmpInfoHeader.biWidth*elemsize;
	imagesize = size * bmpInfoHeader.biHeight;

	if(!imagesize) 
		imagesize=bmpInfoHeader.biHeight*size;

	inimg = (ubyte*)malloc(sizeof(ubyte)*imagesize);
	outimg = (ubyte*)malloc(sizeof(ubyte)*bmpInfoHeader.biWidth*bmpInfoHeader.biHeight);
	fread(inimg, sizeof(ubyte), imagesize, fp);

	fclose(fp);

/*
	// height convert
	for(y = 0; y < bmpInfoHeader.biHeight; y++) {
		index = (bmpInfoHeader.biHeight-y-1) * size; 
		for(x = 0 ; x < bmpInfoHeader.biWidth; x++) {
			outimg[index+3*x+0] = inimg[size*y+3*x+0];
			outimg[index+3*x+1] = inimg[size*y+3*x+1];
			outimg[index+3*x+2] = inimg[size*y+3*x+2];
		};
	};
*/	
	// gray 
	
	for(int y = 0; y < bmpInfoHeader.biHeight; y++) {
		for(int x = 0 ; x < size; x+=elemsize) {
			ubyte b = inimg[size*y+x];
			ubyte g = inimg[size*y+x+1];
			ubyte r = inimg[size*y+x+2];
			ubyte gray = (r*0.299F)+(g*0.587F)+(b*0.114F);
			outimg[bmpInfoHeader.biWidth*y+x/elemsize] = gray; 
		};
	};

	if((fp = fopen(argv[2], "wb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		exit(EXIT_FAILURE);
	}

	palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
	for(int x=0;x<256;x++){
		palrgb[x].rgbBlue = palrgb[x].rgbGreen = palrgb[x].rgbRed = x;
		palrgb[x].rgbReserved = 0;	
	}

	bmpInfoHeader.biBitCount = 8;
    bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth*bmpInfoHeader.biHeight;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biClrImportant = 0;
    bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
    bmpHeader.bfSize = bmpHeader.bfOffBits + bmpInfoHeader.SizeImage;
	
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
 
	fwrite(palrgb, sizeof(unsigned int), 256, fp);
	fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);

	free(inimg);
	free(outimg);

	fclose(fp);
}
