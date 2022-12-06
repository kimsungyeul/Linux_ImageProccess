#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>
#include <time.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

#define widthbytes(bits) (((bits)+31)/32*4)

int main(int argc, char** argv) {
	FILE* fp; 
	RGBQUAD *palrgb;
	
	BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */

	char input[128], output[128];
	
	int i, j, x, y, z; 
	float srcX, srcY;
	int index; 
	float r,g,b;
	int imagesize;
	int index2;
	srand((unsigned int)time(NULL));
	
	unsigned char *inimg, *outimg;
	
	/* usage a.out in.bmp out.bmp */
	strcpy(input, argv[1]); 
	strcpy(output, argv[2]);
	
	
	if((fp=fopen(input, "rb")) == NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	/* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
		
	int elemSize = bmpInfoHeader.biBitCount/8;
    int size = bmpInfoHeader.biWidth*elemSize;
    imagesize = size * bmpInfoHeader.biHeight;

	if(!imagesize) 
		imagesize=bmpInfoHeader.biHeight*size;
	
	/* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     
    /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imagesize);	

	inimg=(ubyte*)malloc(sizeof(ubyte)*imagesize); 
	outimg=(ubyte*)malloc(sizeof(ubyte)*imagesize); 
	fread(inimg, sizeof(ubyte), imagesize, fp); 
	//fseek(fp, -1*sizeof(ubyte)*imageSize, SEEK_CUR);
	//fread(outimg, sizeof(ubyte), imagesize, fp); 
	
	fclose(fp);
	
	for(y=0; y<bmpInfoHeader.biHeight; y++) { 
		for(x=0; x<size; x+=elemSize) {
			for(z=0; z<elemSize; z++) {
				outimg[x+y*size+z] = inimg[x+y*size+z];
			};
		};
	};	   

	for(i = 0; i < 10000; i++) {
		int pos = rand() % (bmpInfoHeader.biWidth * bmpInfoHeader.biHeight);
		int value = rand() & 0b11111111;
		for(z = 0; z < elemSize; z++) {
			int tmp = inimg[pos*elemSize+z] + value;
			outimg[pos*elemSize+z] = LIMIT_UBYTE(tmp);
		}
	}
	 
	bmpHeader.bfOffBits += 256*sizeof(RGBQUAD);

	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
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
