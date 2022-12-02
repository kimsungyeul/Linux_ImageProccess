#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <math.h>

#include "bmpHeader.h"

#define BYTE	unsigned char
#define M_PI	3.141592654
#define BASE	15

#define widthbytes(bits)   (((bits)+31)/32*4)

void main(int argc, char** argv)
{
	FILE *fp;
	RGBQUAD palrgb[256];

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
	
	size=widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth);

	imagesize = bmpInfoHeader.SizeImage;

	if(!imagesize) 
		imagesize=bmpInfoHeader.biHeight*size;

	inimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
	outimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
	fread(inimg, sizeof(BYTE), imagesize, fp);
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
	// width convert

	int elemsize = bmpInfoHeader.biBitCount/8;
	for(int y = 0; y < bmpInfoHeader.biHeight; y++) {
		for(int x = 0 ; x < size; x+=elemsize) {
			for(int z = 0; z < elemsize; z++) {
				
			}
			outimg[size*y+3*(bmpInfoHeader.biWidth-x-1)+0] = inimg[size*y+3*x+0];
			outimg[size*y+3*(bmpInfoHeader.biWidth-x-1)+1] = inimg[size*y+3*x+1];
			outimg[size*y+3*(bmpInfoHeader.biWidth-x-1)+2] = inimg[size*y+3*x+2];
		};
	};
	bmpHeader.bfOffBits += 256*sizeof(RGBQUAD);

	if((fp = fopen(output, "wb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		exit(EXIT_FAILURE);
	}

	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
 
	fwrite(palrgb, sizeof(unsigned int), 256, fp);
	fwrite(outimg, sizeof(unsigned char), imagesize, fp);

	free(inimg);
	free(outimg);

	fclose(fp);
}
