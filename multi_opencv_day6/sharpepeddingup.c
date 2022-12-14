#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     	/* BMP IMAGE INFO */
    //RGBQUAD *palrgb;
    ubyte *inimg, *padimg, *outimg;
    int x, y, z, imageSize;

    if(argc != 3) {
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    int elemSize = bmpInfoHeader.biBitCount/8;
    int originWidth = bmpInfoHeader.biWidth*elemSize;
    imageSize = originWidth * bmpInfoHeader.biHeight; 

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
    fread(inimg, sizeof(ubyte), imageSize, fp); 
    
    fclose(fp);

    int padWidthSize = (bmpInfoHeader.biWidth + 2) * elemSize;
    int padSize = (padWidthSize + bmpInfoHeader.biHeight * elemSize) * 2;
    padimg = (ubyte*)malloc(sizeof(ubyte)*(imageSize + padSize)); 

    memset(padimg, 0, (sizeof(ubyte)*(imageSize + padSize)));

    for(int y = 0; y < bmpInfoHeader.biHeight; y++) {
        for(int x = 0; x < originWidth; x+=elemSize) {
            for(z = 0; z < elemSize; z++) {
                padimg[(x+elemSize)+(y+1)*padWidthSize+z] = inimg[x+y*originWidth+z];
            }
        }	
    }		

    for(y = 0; y < bmpInfoHeader.biHeight; y++) {
        for(z = 0; z < elemSize; z++) {
            padimg[0+(y+1)*padWidthSize+z] = inimg[0+y*originWidth+z];
            padimg[padWidthSize-elemSize+(y+1)*padWidthSize+z] = inimg[originWidth-elemSize+y*originWidth+z];
        }
    }

    for(x = 0; x < bmpInfoHeader.biWidth*elemSize; x++) {
        padimg[x+elemSize] = inimg[x]; 
        padimg[(x+elemSize)+(bmpInfoHeader.biHeight+1)*padWidthSize] = inimg[x+(bmpInfoHeader.biHeight-1)*originWidth];
    }

    for(z = 0; z < elemSize; z++) {
        padimg[z] = inimg[z];
        padimg[padWidthSize-elemSize+z] = inimg[originWidth-elemSize+z];
        padimg[0+(bmpInfoHeader.biHeight+1)*padWidthSize+z] = inimg[0+(bmpInfoHeader.biHeight-1)*originWidth+z];
        padimg[padWidthSize-elemSize+(bmpInfoHeader.biHeight+1)*padWidthSize+z] = inimg[originWidth-elemSize+(bmpInfoHeader.biHeight-1)*originWidth+z];	
    }

    // define the kernel
    float kernel[3][3] = { {-1, -1, -1},
                           {-1,  9, -1},
                           {-1, -1, -1} };

    memset(outimg, 0, sizeof(ubyte)*imageSize);

    for(y = 1; y < bmpInfoHeader.biHeight+1; y++) { 
        for(x = 1*elemSize; x < padWidthSize; x+=elemSize) {
            for(z = 0; z < elemSize; z++) {
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*padimg[(x+j*elemSize)+(y+i)*padWidthSize+z];
                    }
                }
                outimg[(x-elemSize)+(y-1)*originWidth+z] = LIMIT_UBYTE(sum);
            }
        }
    }         

    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 

    free(inimg); 
    free(outimg);
    free(padimg);

    return 0;
}
