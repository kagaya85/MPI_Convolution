#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <iostream>
#include <mpi.h>

#pragma pack(1)

typedef struct BITMAPFILEHEADER {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct BITMAPINFOHEADER {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFODEADER;

using namespace std;

#define BMP_FILE_NAME "timg.bmp"

/*******************************************************************************/

void showBmpHead(BITMAPFILEHEADER &pBmpHead) {
    cout << "==========位图文件头==========" << endl;
    cout << "文件头类型:" << pBmpHead.bfType << endl;
    cout << "文件大小:" << pBmpHead.bfSize << endl;
    cout << "保留字_1:" << pBmpHead.bfReserved1 << endl;
    cout << "保留字_2:" << pBmpHead.bfReserved2 << endl;
    cout << "实际位图数据的偏移字节数:" << pBmpHead.bfOffBits << endl << endl;
}

void showBmpInforHead(BITMAPINFODEADER &pBmpInforHead) {
    cout << "==========位图信息头==========" << endl;
    cout << "结构体的长度:" << pBmpInforHead.biSize << endl;
    cout << "位图宽:" << pBmpInforHead.biWidth << endl;
    cout << "位图高:" << pBmpInforHead.biHeight << endl;
    cout << "biPlanes平面数:" << pBmpInforHead.biPlanes << endl;
    cout << "biBitCount采用颜色位数:" << pBmpInforHead.biBitCount << endl;
    cout << "压缩方式:" << pBmpInforHead.biCompression << endl;
    cout << "biSizeImage实际位图数据占用的字节数:" << pBmpInforHead.biSizeImage
         << endl;
    cout << "X方向分辨率:" << pBmpInforHead.biXPelsPerMeter << endl;
    cout << "Y方向分辨率:" << pBmpInforHead.biYPelsPerMeter << endl;
    cout << "使用的颜色数:" << pBmpInforHead.biClrUsed << endl;
    cout << "重要颜色数:" << pBmpInforHead.biClrImportant << endl;
}

void readBmp(FILE* fp, unsigned char *pBmpBuf, int BmpWidth, int BmpHeight, int BiBitCount) {

    /**
     * 灰度图像有颜色表，且颜色表表项为256
     * (可以理解为lineByte是对bmpWidth的以4为步长的向上取整)
     */ 
    int lineByte = (BmpWidth * BiBitCount / 8 + 3) / 4 * 4;  

    //申请位图数据所需要的空间，读位图数据进内存
    pBmpBuf = new(nothrow) unsigned char[lineByte * BmpHeight];
    if(pBmpBuf == NULL) {
        cerr << "Mem alloc failed." << endl;
        exit(-1);
    }

    fread(pBmpBuf, 1, lineByte * BmpHeight, fp);

    return;  
}

//给定一个图像位图数据、宽、高、颜色表指针及每像素所占的位数等信息,将其写到指定文件中
bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height,
             int biBitCount) {
    //如果位图数据指针为0，则没有数据传入，函数返回
    if (!imgBuf) return 0;

    //颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0
    int colorTablesize = 0;

    if (biBitCount == 8) colorTablesize = 1024;  // 8*128

    //待存储图像数据每行字节数为4的倍数
    int lineByte = (width * biBitCount / 8 + 3) / 4 * 4;

    //以二进制写的方式打开文件
    FILE *fp = fopen(bmpName, "wb");

    if (fp == 0) {
        cerr << "Open file error." << endl;
        return 0;
    }

    //申请位图文件头结构变量，填写文件头信息
    BITMAPFILEHEADER fileHead;

    fileHead.bfType = 0x4D42;  // bmp类型

    // bfSize是图像文件4个组成部分之和
    fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                      colorTablesize + lineByte * height;
    fileHead.bfReserved1 = 0;
    fileHead.bfReserved2 = 0;

    // bfOffBits是图像文件前3个部分所需空间之和
    fileHead.bfOffBits = 54 + colorTablesize;

    //写文件头进文件
    fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

    //申请位图信息头结构变量，填写信息头信息
    BITMAPINFOHEADER head;

    head.biBitCount = biBitCount;
    head.biClrImportant = 0;
    head.biClrUsed = 0;
    head.biCompression = 0;
    head.biHeight = height;
    head.biPlanes = 1;
    head.biSize = 40;
    head.biSizeImage = lineByte * height;
    head.biWidth = width;
    head.biXPelsPerMeter = 0;
    head.biYPelsPerMeter = 0;

    //写位图信息头进内存
    fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);

    //写位图数据进文件
    fwrite(imgBuf, height * lineByte, 1, fp);

    //关闭文件
    fclose(fp);

    return 1;
}

int main(int argc, char *argv[]) {
    BITMAPFILEHEADER BmpHead;
    BITMAPINFODEADER BmpInfo;
    
    int BmpWidth;//图像的宽
    int BmpHeight;//图像的高
    int BiBitCount;//图像类型，每像素位数 8-灰度图 24-彩色图
    unsigned char *pBmpBuf;  //读入图像数据的指针
    
    FILE *fp = fopen(BMP_FILE_NAME, "rb");  //二进制读方式打开指定的图像文件
    if (fp == 0) {
        cerr << "Can not open " << BMP_FILE_NAME << endl;
        return 0;
    }
    //获取位图文件头结构BITMAPFILEHEADER
    fread(&BmpHead, sizeof(BITMAPFILEHEADER), 1, fp);

    //获取图像宽、高、每像素所占位数等信息
    fread(&BmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);  
    
    // 打印一下文件信息
    showBmpHead(BmpHead);
    showBmpInforHead(BmpInfo);
    
    BmpWidth = BmpInfo.biWidth;  //宽度用来计算每行像素的字节数
    BmpHeight = BmpInfo.biHeight;  // 像素的行数
    BiBitCount = BmpInfo.biBitCount;  //定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
    
    // 将图片读取到内存中
    readBmp(fp, pBmpBuf, BmpWidth, BmpHeight, BiBitCount);

    // MPI 并行计算部分
    MPI_Init(&argc, &argv);

    
    MPI_Finalize();
    // MPI end

    if (pBmpBuf)
        delete pBmpBuf;

    fclose(fp);
    return 0;
}