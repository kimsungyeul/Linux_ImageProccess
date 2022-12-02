#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bmpHeader.h"

#define BYTE	unsigned char
#define widthbytes(bits)   (((bits)+31)/32*4)

#ifndef M_PI
#define M_PI	3.141592654
#endif

int main(int argc, char** argv)
{
	FILE *fp;
	RGBQUAD palrgb[256];
	
	unsigned int imagesize;
	char input[128], output[128];
	
	int i, j, size, index;
	double radius, cos_value, sin_value;
	int centerX, centerY;
	int degree = 45;
	int elemSize;
	
	unsigned char *inimg, *outimg;
	
	strcpy(input, argv[1]);
	strcpy(output, argv[2]);
	
	if((fp = fopen(input, "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		return -1;
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
	
	radius = degree*(M_PI/180.0f);
	sin_value = sin(radius);
	cos_value = cos(radius);
	centerX = bmpInfoHeader.biHeight/2;
	centerY = bmpInfoHeader.biWidth/2;
	
	elemSize = bmpInfoHeader.biBitCount/8;
	for(i = 0 ; i < bmpInfoHeader.biHeight; i++) { 
		index = (bmpInfoHeader.biHeight-i-1) * size; 
		for(j = 0 ; j < bmpInfoHeader.biWidth; j++) { 
			double new_x;
			double new_y;
			new_x = (i-centerX)*cos_value - (j-centerY)*sin_value + centerX;
			new_y = (i-centerX)*sin_value + (j-centerY)*cos_value + centerY; 
			
			if(new_x <0 || new_x > bmpInfoHeader.biHeight) {
				outimg[index+elemSize*j+0] = 0;
				outimg[index+elemSize*j+1] = 0xff;
				outimg[index+elemSize*j+2] = 0;

			} else if (new_y <0 || new_y > bmpInfoHeader.biWidth) {
				outimg[index+elemSize*j+0] = 0x55;
				outimg[index+elemSize*j+1] = 0;
				outimg[index+elemSize*j+2] = 0;
			} else {
				outimg[index+elemSize*j+0] = inimg[(int)(bmpInfoHeader.biHeight-new_x-1)*size+(int)new_y*3+0];
				outimg[index+elemSize*j+1] = inimg[(int)(bmpInfoHeader.biHeight-new_x-1)*size+(int)new_y*3+1];
				outimg[index+elemSize*j+2] = inimg[(int)(bmpInfoHeader.biHeight-new_x-1)*size+(int)new_y*3+2];
			}
		};
	};
	
	bmpHeader.bfOffBits += 256*sizeof(RGBQUAD); 
	
	if((fp = fopen(output, "wb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		return -1;
	}

	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);	

	fwrite(palrgb, sizeof(unsigned int), 256, fp);
	fwrite(outimg, sizeof(unsigned char), imagesize, fp);
	
	free(inimg);
	fclose(fp);
	
	return -1;
}
