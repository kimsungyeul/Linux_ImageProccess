#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include "bmpHeader.h"

#define BYTE unsigned char 
#define widthbytes(bits)   (((bits)+31)/32*4) 

int main(int argc, char** argv) 
{ 
	FILE *fp; 
	RGBQUAD palrgb[256]; 
	
	unsigned int imagesize;
	char input[128], output[128]; 
	
	int i, j, size; 
	int xFactor = 4, yFactor = 4;  
	float srcX, srcY; 
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

	size = widthbytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth); 

	imagesize = bmpInfoHeader.SizeImage;
	
	if(!imagesize) imagesize = bmpInfoHeader.biHeight * size; 
	inimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
	outimg = (BYTE*)malloc(sizeof(BYTE)*imagesize); 
	fread(inimg, sizeof(BYTE), imagesize, fp); 
	fclose(fp); 
	
	elemSize = bmpInfoHeader.biBitCount/8;
	for(i = 0, srcY = 0; i < bmpInfoHeader.biHeight*elemSize/yFactor; srcY += yFactor, i++) {  
		for(j = 0, srcX = 0; j < bmpInfoHeader.biWidth*elemSize/xFactor; srcX += xFactor, j++) {  
			outimg[j+(i*bmpInfoHeader.biWidth/4)] = inimg[(long)(j*xFactor)+(long)(i*bmpInfoHeader.biWidth*yFactor)]; 
		};  
	};  
	
	bmpInfoHeader.biWidth /= xFactor, bmpInfoHeader.biHeight /= yFactor;
	size = widthbytes(bmpInfoHeader.biBitCount*bmpInfoHeader.biWidth); 
	imagesize = bmpInfoHeader.biHeight * size; 
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
	free(outimg); 
	
	fclose(fp); 
	
	return 0;
}

