#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>                     /* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>
#include <math.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *padimg, *grayimg, *outimg;
    int x, y, z, imageSize;
	int r,g,b,gray;

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
    int size = bmpInfoHeader.biWidth*elemSize;
    imageSize = size * bmpInfoHeader.biHeight; 

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    grayimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
    fread(inimg, sizeof(ubyte), imageSize, fp); 
    
    fclose(fp);

    int padSize = (bmpInfoHeader.biWidth + 2) * elemSize;
	int addSize = (padSize + bmpInfoHeader.biHeight) * 2;
    padimg = (ubyte*)malloc(sizeof(ubyte)*(imageSize + addSize));
    memset(padimg, 0, (sizeof(ubyte)*imageSize + addSize));
    memset(grayimg, 0, (sizeof(ubyte)*imageSize));

	for(y = 0; y < bmpInfoHeader.biHeight; y++) {
		for(x = 0; x < size; x+=elemSize) {
			b = inimg[x+y*size+0];
			g = inimg[x+y*size+1];
			r = inimg[x+y*size+2];
			//gray=(r*0.3F)+(g*0.59F)+(b*0.11F);
			grayimg[x+y*size+0] = grayimg[x+y*size+1] = grayimg[x+y*size+2] = ((66*r+129*g+25*b+128)>>8)+16;
		}
		
	}

    for(int y = 0; y < bmpInfoHeader.biHeight; y++) {
		for(int x = 0; x < size; x+=elemSize) {
			for(z = 0; z < elemSize; z++) {
				padimg[(x+elemSize)+(y+1)*padSize+z] = grayimg[x+y*size+z];
			}
		}
	}		

	for(y = 0; y < bmpInfoHeader.biHeight; y++) {
		for(z = 0; z < elemSize; z++) {
			padimg[0+(y+1)*padSize+z] = grayimg[0+y*size+z];
			padimg[padSize-elemSize+(y+1)*padSize+z] = grayimg[size-elemSize+y*size+z];
		}
	}

	for(x = 0; x < bmpInfoHeader.biWidth*elemSize; x++) {
		padimg[x+elemSize] = grayimg[x]; 
		padimg[(x+elemSize)+(bmpInfoHeader.biHeight+1)*padSize] = grayimg[x+(bmpInfoHeader.biHeight-1)*size];
	}

	for(z = 0; z < elemSize; z++) {
		padimg[z] = grayimg[z];
		padimg[padSize-elemSize+z] = grayimg[size-elemSize+z];
		padimg[0+(bmpInfoHeader.biHeight+1)*padSize+z] = grayimg[0+(bmpInfoHeader.biHeight-1)*size+z];
		padimg[padSize-elemSize+(bmpInfoHeader.biHeight+1)*padSize+z] = grayimg[size-elemSize+(bmpInfoHeader.biHeight-1)*size+z];	
	}

	
    // define the kernel
   	float xmask[3][3] = { {-1, 0, 1},
                          {-2, 0, 2},
                          {-1, 0, 1} };
	float ymask[3][3] = { {-1,-2,-1},
                          { 0, 0, 0},
                          { 1, 2, 1} };

    memset(outimg, 0, sizeof(ubyte)*imageSize);
    for(y = 1; y < bmpInfoHeader.biHeight+1; y++) { 
        for(x = 1*elemSize; x < padSize; x+=elemSize) {
            for(z = 0; z < elemSize; z++) {
                float xval = 0.0, yval = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        xval += xmask[i+1][j+1]*padimg[(x+j*elemSize)+(y+i)*padSize+z];
                        yval += ymask[i+1][j+1]*padimg[(x+j*elemSize)+(y+i)*padSize+z];
                    }
                }
                outimg[(x-elemSize)+(y-1)*size+z] = LIMIT_UBYTE(sqrt(xval*xval+yval+yval));
            }
        }
    }         

    palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
    for(x = 0; x < 256; x++) {
		palrgb[x].rgbBlue = palrgb[x].rgbGreen = palrgb[x].rgbRed = x;
		palrgb[x].rgbReserved = 0;
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

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 
    
    free(inimg); 
    free(outimg);
    free(padimg);
    
    return 0;
}
