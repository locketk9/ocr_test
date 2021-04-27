/// @file	bmp_func.h
/// @brief	bmp relative functions
/// @author	locketk9@gmail.com
//#if defined(WIN32)
 #include "stdafx.h"
//#endif

#include "bmp_func.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <utility>
#include <iterator>
#include <array>



/////////////////////////////////
// bmp header
#if !defined(WIN32)
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;
typedef char BYTE;
#endif

typedef unsigned short ushort;
typedef unsigned int uint;
//typedef int int;
//typedef char BYTE;

typedef struct tagBITMAPFILEHEADER_ {
  ushort bfType;
  uint bfSize;
  ushort bfReserved1;
  ushort bfReserved2;
  uint bfOffBits;
} BITMAPFILEHEADER_;

typedef struct tagBITMAPINFOHEADER_ {
  int biSize;
  int biWidth;
  int biHeight;
  ushort biPlanes;
  ushort biBitCount;
  uint biCompression;
  uint biSizeImage;
  int biXPelsPerMeter;
  int biYPelsPerMeter;
  uint biClrUsed;
  uint biClrImportant;
} BITMAPINFOHEADER_;
//
#if !defined(WIN32)
typedef struct RGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD;
#endif
/////////////////////////////////



/// @brief load bmp file
/// @param const char *
/// @param ch_vec &
/// @param int &
/// @param int &
int loadBMP(const char *filename, ch_vec &bmp, int &width, int &height)
{
    int infosize;
    int i;
    uint bitsize;
    char *bits;

    FILE *fp = 0;

    //fp=fopen(filename, "rb");
#if defined(WIN32)
	fopen_s(&fp, filename, "rb");
#else
	fp = fopen(filename, "rb");
#endif
    if (fp == NULL)
        return (-1);

    BITMAPFILEHEADER_ header;
    BITMAPINFOHEADER_ bmpinfo;

    memset(&header, 0x00, sizeof(header));
    memset(&bmpinfo, 0x00, sizeof(bmpinfo));

//    cout << "bmp header/bmpinfo start = " << sizeof(WORD) << " / " << sizeof(bmpinfo) << std::endl << std::flush;

    fread(&header, 1, 14, fp);
    // read header
//    fread(&(header.bfType), 1, 2, fp);
//    fread(&(header.bfSize), 1, 4, fp);
//    fread(&(header.bfReserved1), 1, 2, fp);
//    fread(&(header.bfReserved2), 1, 2, fp);
//    fread(&(header.bfOffBits), 1, 4, fp);

    infosize=header.bfOffBits-sizeof(header);

    fread(&bmpinfo, 1, 40, fp);
    // write bmpinfo
//    fread(&(bmpinfo.biSize), 1, 2, fp);
//    fread(&(bmpinfo.biWidth), 1, 4, fp);
//    fread(&(bmpinfo.biHeight), 1, 4, fp);
//    fread(&(bmpinfo.biPlanes), 1, 2, fp);
//    fread(&(bmpinfo.biBitCount), 1, 2, fp);
//    fread(&(bmpinfo.biCompression), 1, 4, fp);
//    fread(&(bmpinfo.biSizeImage), 1, 4, fp);
//    fread(&(bmpinfo.biXPelsPerMeter), 1, 4, fp);
//    fread(&(bmpinfo.biYPelsPerMeter), 1, 4, fp);
//    fread(&(bmpinfo.biClrUsed), 1, 4, fp);
//    fread(&(bmpinfo.biClrImportant), 1, 4, fp);

    //bitsize=bmpinfo.biHeight*bmpinfo.biWidth*3;
	bitsize = bmpinfo.biWidth * abs(bmpinfo.biHeight) * (bmpinfo.biBitCount / 8); // bmpinfo.biSizeImage;
    //bits=(char*)malloc(bitsize);

	if ((bmpinfo.biBitCount / 8) == 1) {
		RGBQUAD quad[256] = { 0, };
		fread(quad, 4, 256, fp);
		//    cout << "bmp data read start = " << bitsize  << std::endl << std::flush;
	}

    bmp.clear();
    bmp.reserve(bitsize);
	bmp.resize(bitsize, 0x00);
//    fread( reinterpret_cast<void*>(const_cast<uchar*>(bmp.data())), 1, bitsize, fp);
    //char *data = new char[bitsize+1];
    //memset(data, 0x00, bitsize+1);
//    fread( const_cast<char*>(bmp.data()), 1, bitsize, fp);
    size_t rt = 0;
    rt = fread( bmp.data(), 1, bitsize,fp);
    //bmp.insert(bmp.begin(), data, data+bitsize );
//    memcpy(const_cast<char*>(bmp.data()), data, bitsize);
    //delete[] data;

    width = bmpinfo.biWidth;
    height = bmpinfo.biHeight;
//    cout << "bmp read ok = " << bmp.size() << "," << width << "," << height << "," << rt << std::endl << std::flush;

    fclose(fp);

    // bmp 2 raw
	if (bmpinfo.biHeight > 0) {
		ch_vec raw(bitsize);
		bmp2raw(bmp.data(), raw.data(), 
			bmpinfo.biWidth, bmpinfo.biHeight, (bmpinfo.biBitCount / 8));
		bmp = raw;
	}

#if 0
   fp=fopen("../image/out.bmp","wb");
   // write header
   fwrite(&(header), 1, 14, fp);
//   fwrite(&(header.bfType), 1, 2, fp);
//   fwrite(&(header.bfSize), 1, 4, fp);
//   fwrite(&(header.bfReserved1), 1, 2, fp);
//   fwrite(&(header.bfReserved2), 1, 2, fp);
//   fwrite(&(header.bfOffBits), 1, 4, fp);
   // write bmpinfo
   fwrite(&(bmpinfo), 1, 38, fp);
//   fwrite(&(bmpinfo.biSize), 1, 2, fp);
//   fwrite(&(bmpinfo.biWidth), 1, 4, fp);
//   fwrite(&(bmpinfo.biHeight), 1, 4, fp);
//   fwrite(&(bmpinfo.biPlanes), 1, 2, fp);
//   fwrite(&(bmpinfo.biBitCount), 1, 2, fp);
//   fwrite(&(bmpinfo.biCompression), 1, 4, fp);
//   fwrite(&(bmpinfo.biSizeImage), 1, 4, fp);
//   fwrite(&(bmpinfo.biXPelsPerMeter), 1, 4, fp);
//   fwrite(&(bmpinfo.biYPelsPerMeter), 1, 4, fp);
//   fwrite(&(bmpinfo.biClrUsed), 1, 4, fp);
//   fwrite(&(bmpinfo.biClrImportant), 1, 4, fp);
   // write quad
   fwrite(quad, 4, 155, fp);
   // write data
   fwrite(bmp.data(), 1, bitsize, fp);
   fclose(fp);
#endif

    return (0);
}

/// @brief save bmp
/// @param const char*
/// @param const ch_vec &
/// @param int
/// @param int
int saveBMP(const char *filename, const byte *img, int width, int height, int bp)
{
    int infosize;
    int i;
    DWORD bitsize;
    char *bits;

    FILE *fp = 0;


    BITMAPFILEHEADER_ header;
    BITMAPINFOHEADER_ bmpinfo;
	RGBQUAD quad[256] = { 0x00, }; // { { 0xFF,0xFF, 0xFF, 0xFF }, };

    memset((void*)&header, 0x00, sizeof(header));
    memset((void*)&bmpinfo, 0x00, sizeof(bmpinfo));

#if 0
//  fp=fopen("../sample_image/lena256.bmp", "rb");
    fopen_s(&fp, "lena256.bmp", "rb");
    if (fp == NULL)
        return (-1);
		
    // read header
    fread(&header, 1, 14, fp);
//    fread(&header, 1, sizeof(header), fp);
    infosize=header.bfOffBits-sizeof(header);
    // read bmpinfo
    fread(&bmpinfo, 1, 38, fp);
//    fread(&bmpinfo, 1, sizeof(bmpinfo), fp);
    // read quad
	//RGBQUAD quad[256]={0,};
	fread( quad,4,256,fp);

	//size_t size = bmpinfo.biWidth*bmpinfo.biHeight;
	//byte *data = new byte[size];
	//fread( (void*)data, 1, size, fp);

    fclose(fp);
#endif

	int new_w = width + (width % 4 ? 4 - (width % 4) : 0);
	int new_h = height; // +(height % 4 ? 4 - (height % 4) : 0);
	
	// header
	char type[] = "BM";
	memcpy((void*)&header.bfType, &type, 2);
	//header.bfSize = sizeof(header) + sizeof(bmpinfo) + (width * height * bp);
	header.bfSize = 14 + 40 + (new_w * new_h * bp) + 2;
	header.bfOffBits = 0; // header.bfSize - 14;

	// bmpinfo
	bmpinfo.biSize = 40; // sizeof(bmpinfo);
	bmpinfo.biPlanes = 1;
    bmpinfo.biBitCount = 8*bp;
	bmpinfo.biWidth = new_w;
	bmpinfo.biHeight = -new_h;
	bmpinfo.biSizeImage = new_w * new_h * bp; // (((new_w * bmpinfo.biBitCount + 31) & ~31) >> 3) * new_h;;
	bitsize = new_w * new_h * bp;

	// quad
	for (int i = 0; i < 256; ++i) {
		quad[i].rgbRed = static_cast<byte>(i);
		quad[i].rgbGreen = static_cast<byte>(i);
		quad[i].rgbBlue = static_cast<byte>(i);
		quad[i].rgbReserved = 0;
	}

    // bmp write
//    fp=fopen(filename,"wb");
#if defined(WIN32)
    fopen_s(&fp, filename,"w+b");
#else
	fp = fopen(filename, "w+b");
#endif
	if (fp == NULL)
        return (-1);
	
	//// y-axis conv
	//byte *conv = new byte[bitsize];
	//bmp2raw(img, conv, width, height, 1);

#if 1
	// write header
    fwrite(&(header), 1, 14, fp);
    //fwrite(&(header), 1, sizeof(header), fp);
    // write bmpinfo
    fwrite(&(bmpinfo), 1, 40, fp);
    //fwrite(&(bmpinfo), 1, sizeof(bmpinfo), fp);
    // write quad
	if (bp == 1)
 		fwrite(quad, 4, 256, fp);
    // write data
//	fwrite(img, bp, bitsize, fp);
	std::vector<unsigned char> bmp(new_w * bp, 0);
//	std::copy_n(img, width*height*bp, std::begin(bmp));
//	memcpy(bmp.data(), img, width*height*bp);
	for (int i = 0; i < new_h; ++i)
	{
		memcpy(bmp.data(), img + i * width * bp, width * bp);
		fwrite(bmp.data(), 1, new_w * bp, fp);
		//fwrite(img + i * width * bp, 1, new_w * bp, fp);
	}

	// write padding
	int padd = 2;// bitsize + (bitsize % bp ? bp - (bitsize % bp) : 0);
    std::vector<char> vp(padd);
	fwrite(vp.data(), 1, padd , fp);
#endif
#if 0
   // write header
//   fwrite(&(header), 1, 14, fp);
   fwrite(&(header.bfType), 1, 2, fp);
   fwrite(&(header.bfSize), 1, 4, fp);
   fwrite(&(header.bfReserved1), 1, 2, fp);
   fwrite(&(header.bfReserved2), 1, 2, fp);
   fwrite(&(header.bfOffBits), 1, 4, fp);
   // write bmpinfo
//   fwrite(&(bmpinfo), 1, 38, fp);
   fwrite(&(bmpinfo.biSize), 1, 2, fp);
   fwrite(&(bmpinfo.biWidth), 1, 4, fp);
   fwrite(&(bmpinfo.biHeight), 1, 4, fp);
   fwrite(&(bmpinfo.biPlanes), 1, 2, fp);
   fwrite(&(bmpinfo.biBitCount), 1, 2, fp);
   fwrite(&(bmpinfo.biCompression), 1, 4, fp);
   fwrite(&(bmpinfo.biSizeImage), 1, 4, fp);
   fwrite(&(bmpinfo.biXPelsPerMeter), 1, 4, fp);
   fwrite(&(bmpinfo.biYPelsPerMeter), 1, 4, fp);
   fwrite(&(bmpinfo.biClrUsed), 1, 4, fp);
   fwrite(&(bmpinfo.biClrImportant), 1, 4, fp);
   // write quad
   fwrite(quad, 4, 256, fp);
   // write data
   fwrite(img, 1, bitsize, fp);
#endif // 0

//	delete[] conv;
    fclose(fp);

    return (0);
}


/// @brief bmp 2 raw
byte* bmp2raw(const byte *bmp, byte *conv, int imgWidth, int imgHeight, int bp)
{
    if (1 > imgWidth*imgHeight*bp) return conv;

//    cout << "bmp2raw = " << bmp.size() << "," << imgWidth << "," << imgHeight << std::endl << std::flush;

    int size = imgWidth*imgHeight*bp;
#if 1
    // convert y-axis - it's woring
	memset(conv, 0x00, size);
	
	int stripe = imgWidth*bp;
	for(int i=0, pos=0, rpos=imgWidth*imgHeight*bp
		; i<imgHeight
		; ++i, pos += (stripe), rpos-=(stripe))
    {
		memcpy(conv+pos, bmp+rpos-(stripe), (stripe));
    }
 
#endif

    return (conv);
}

