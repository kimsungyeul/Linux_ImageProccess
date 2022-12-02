#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include "bmpHeader.h"

#define BYTE unsigned char

#define widthbytes(bits) (((bits)+31)/32*4)

int main(int argc, char** argv) {
	FILE* fp; 
	RGBQUAD *palrgb;
	
	int imagesize;
	char input[128], output[128];
	
	int i, j, size; 
	float r,g,b;
	int x,pos=0;
	int mask=0;

	unsigned char *grayimg, *inimg, *outimg;
	
	/* usage a.out in.bmp out.bmp */
	strcpy(input, argv[1]); 
	strcpy(output, argv[2]);
	
	
	if((fp=fopen(input, "rb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	if(bmpInfoHeader.biBitCount == 24){
		perror("This image file doesn't supports 24bit color\n");
		fclose(fp);
		return -1;
	}

	if(bmpInfoHeader.SizeImage != 0) 
		imagesize=widthbytes(bmpInfoHeader.biBitCount*bmpInfoHeader.biWidth)*
					bmpInfoHeader.biHeight;

	printf("offset : %d\n", bmpHeader.bfOffBits);
	printf("bitcount : %d\n", bmpInfoHeader.biBitCount);
	printf("imagesize : %d\n", bmpInfoHeader.SizeImage);
	printf("true used color : %d\n", bmpInfoHeader.biClrUsed);

	if(bmpInfoHeader.biBitCount==8 && bmpInfoHeader.biClrUsed==0){
		bmpInfoHeader.biClrUsed = 256;
	}

	printf("changed true used color : %d\n", bmpInfoHeader.biClrUsed);

	palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*bmpInfoHeader.biClrUsed);
	fread(palrgb, sizeof(RGBQUAD), bmpInfoHeader.biClrUsed, fp);

	
	for(int i=0;i<bmpInfoHeader.biClrUsed; i++)
	{
		printf("%d : %x %x %x %x\n", i, palrgb[i].rgbRed, palrgb[i].rgbGreen,
				palrgb[i].rgbBlue, palrgb[i].rgbReserved);
	}

	inimg=(BYTE*)malloc(sizeof(BYTE)*bmpInfoHeader.SizeImage); 
	outimg=(BYTE*)malloc(sizeof(BYTE)*(bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*3)); 

	fread(inimg, sizeof(BYTE), imagesize, fp); 
	
	fclose(fp);

	printf("%x : %u\n",0b1, 0b1);
	for(int x=0; x<bmpInfoHeader.biBitCount; x++){
		mask |= 0b1 << x;	
	}

	printf("mask : %x : %u : %d\n",mask,mask,mask);
	float elemSize = bmpInfoHeader.biBitCount/8.;	
	printf("elemsize : %f : float : %f\n",elemSize, elemSize);
	for(x=0; x<(bmpInfoHeader.biHeight*bmpInfoHeader.biWidth)*elemSize; x++) { 
		int num = inimg[x];
		for(int i = 8-bmpInfoHeader.biBitCount; i>=0; i-=bmpInfoHeader.biBitCount) {
			int res = num >> i & mask;
			outimg[pos++]=palrgb[res].rgbBlue;
			outimg[pos++]=palrgb[res].rgbGreen;
			outimg[pos++]=palrgb[res].rgbRed;
		}
	 };	  
		
	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	bmpInfoHeader.biBitCount = 24;
	bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*3;
	bmpInfoHeader.biClrUsed = 0;
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfoHeader.SizeImage;
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);

	free(inimg); 
	free(outimg);
	
	fclose(fp); 
	
	return 0;
}
