#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include "bmpHeader.h"

#define widthBytes(bits) (((bits)+31)/32*4)

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *outimg;
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
    if(bmpInfoHeader.biBitCount == 24) {
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    if(imageSize != 0)
        imageSize = widthBytes(bmpInfoHeader.biBitCount * bmpInfoHeader.biWidth) * 
                               bmpInfoHeader.biHeight; 

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);
    printf("Used Color : %d\n", bmpInfoHeader.biClrUsed);
    printf("Important Color : %d\n", bmpInfoHeader.biClrImportant);

    if(bmpInfoHeader.biBitCount == 8 && bmpInfoHeader.biClrUsed == 0)
        bmpInfoHeader.biClrUsed = 256;

    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*bmpInfoHeader.biClrUsed);
    fread(palrgb, sizeof(RGBQUAD), bmpInfoHeader.biClrUsed, fp); 

    for(int i = 0; i < bmpInfoHeader.biClrUsed; i++) 
        printf("%d : %x %x %x %x\n", i, palrgb[i].rgbRed, palrgb[i].rgbGreen, 
                              palrgb[i].rgbBlue, palrgb[i].rgbReserved);

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    outimg = (ubyte*)malloc(sizeof(ubyte)*(bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*3));
    fread(inimg, sizeof(ubyte), imageSize, fp); 
    
    fclose(fp);
    
    ubyte mask = 0; 
    for(x = 0; x < bmpInfoHeader.biBitCount; x++) 
        mask |= 0b1 << x; 

    unsigned int pos = 0; 
    float elemSize = bmpInfoHeader.biBitCount/8.;
    for(x = 0; x < bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*elemSize; x++) { 
        ubyte num = inimg[x]; 
        for(int i = (8-bmpInfoHeader.biBitCount); i >= 0; i-=bmpInfoHeader.biBitCount) { //8자리 숫자까지 나타냄
             ubyte res = num >> i & mask;
             outimg[pos++] = palrgb[res].rgbBlue;
             outimg[pos++] = palrgb[res].rgbGreen;
             outimg[pos++] = palrgb[res].rgbRed;
        }
    }         
     
    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    bmpInfoHeader.biBitCount = 24;
	  bmpInfoHeader.SizeImage = bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*bmpInfoHeader.biBitCount/8;
    bmpInfoHeader.biCompression = 0;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biClrImportant = 0;
    bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpHeader.bfSize = bmpHeader.bfOffBits + bmpInfoHeader.SizeImage;
    
    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    fwrite(outimg, sizeof(unsigned char), bmpInfoHeader.SizeImage, fp);

    fclose(fp); 
    
    free(inimg); 
    free(outimg);
    
    return 0;
}
