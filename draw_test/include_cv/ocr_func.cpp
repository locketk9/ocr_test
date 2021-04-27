/// @file	ocr_func.h
/// @brief	ocr relative functions
/// @author	locketk9@gmail.com


#include "stdafx.h"

#include "ocr_def.h"
#include "ocr_func.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <utility>
#include <iterator>
#include <cassert>

#include "bmp_func.h"

/////////////////////////////////
// bmp header
//typedef unsigned short WORD;
//typedef unsigned int DWORD;
//typedef int LONG;
//typedef char BYTE;

typedef unsigned short ushort;
typedef unsigned int uint;
//typedef int int;
//typedef char BYTE;

#if 0
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
//typedef struct RGBQUAD {
//    BYTE rgbBlue;
//    BYTE rgbGreen;
//    BYTE rgbRed;
//    BYTE rgbReserved;
//} RGBQUAD;
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
    fopen_s(&fp, filename, "rb");
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

    fread(&bmpinfo, 1, 38, fp);
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

    bitsize=bmpinfo.biHeight*bmpinfo.biWidth*3;
    //bitsize=bmpinfo.biSizeImage;
    //bits=(char*)malloc(bitsize);

    RGBQUAD quad[155]={0,};
    fread( quad,4,155,fp);
//    cout << "bmp data read start = " << bitsize  << std::endl << std::flush;

    bmp.clear();
    bmp.reserve(bitsize+1);
//    fread( reinterpret_cast<void*>(const_cast<uchar*>(bmp.data())), 1, bitsize, fp);
    char *data = new char[bitsize+1];
    memset(data, 0x00, bitsize+1);
//    fread( const_cast<char*>(bmp.data()), 1, bitsize, fp);
    size_t rt = 0;
    rt = fread( data, 1, bitsize,fp);
    bmp.insert(bmp.begin(), data, data+bitsize );
//    memcpy(const_cast<char*>(bmp.data()), data, bitsize);
    delete[] data;

    width = bmpinfo.biWidth;
    height = bmpinfo.biHeight;
//    cout << "bmp read ok = " << bmp.size() << "," << width << "," << height << "," << rt << std::endl << std::flush;

    fclose(fp);

    // bmp 2 raw
//    bmp = bmp2raw(bmp, bmpinfo.biWidth, bmpinfo.biHeight);

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
#if 0
//    fp=fopen("../sample_image/lena256.bmp", "rb");
    fopen_s(&fp, "lena256.bmp", "rb");
    if (fp == NULL)
        return (-1);

    BITMAPFILEHEADER_ header;
    BITMAPINFOHEADER_ bmpinfo;

    memset(&header, 0x00, sizeof(header));
    memset(&bmpinfo, 0x00, sizeof(bmpinfo));

    // read header
    fread(&header, 1, 14, fp);
//    fread(&header, 1, sizeof(header), fp);
    infosize=header.bfOffBits-sizeof(header);
    // read bmpinfo
    fread(&bmpinfo, 1, 38, fp);
//    fread(&bmpinfo, 1, sizeof(bmpinfo), fp);
    // read quad
	RGBQUAD quad[256]={0,};
	fread( quad,4,256,fp);

	//size_t size = bmpinfo.biWidth*bmpinfo.biHeight;
	//byte *data = new byte[size];
	//fread( (void*)data, 1, size, fp);

    fclose(fp);
#endif
	BITMAPFILEHEADER_ header;
	BITMAPINFOHEADER_ bmpinfo;

	memset(&header, 0x00, sizeof(header));
	memset(&bmpinfo, 0x00, sizeof(bmpinfo));

	header.bfSize = bp;
	header.bfType = 19778;
	header.bfReserved2 = 1078;

	bmpinfo.biSize = 40;
	bmpinfo.biPlanes = 1;

    bmpinfo.biBitCount = 8*bp;
    bmpinfo.biWidth = width;
    bmpinfo.biHeight = -height;
    bmpinfo.biSizeImage = (((width * bmpinfo.biBitCount + 31) & ~31) >> 3) * height;;
	bitsize = width*height*bp;
//    bitsize = size;

	RGBQUAD quad[256] = { 0, };
	for (int i = 0; i < 256; ++i) {
		quad[i].rgbRed = i;
		quad[i].rgbGreen = i;
		quad[i].rgbBlue = i;
		quad[i].rgbReserved = i;
	}

    // bmp write
//    fp=fopen(filename,"wb");
    fopen_s(&fp, filename,"w+b");
	if (fp == NULL)
        return (-1);
	
	//// y-axis conv
	//byte *conv = new byte[bitsize];
	//bmp2raw(img, conv, width, height, 1);

#if 1
	// write header
    fwrite(&(header), 1, 14, fp);
//    fwrite(&(header), 1, sizeof(header), fp);
    // write bmpinfo
    fwrite(&(bmpinfo), 1, 38, fp);
//    fwrite(&(bmpinfo), 1, sizeof(bmpinfo), fp);
    // write quad
//    RGBQUAD quad2[155] = {{0xFF,0xFF, 0xFF, 0xFF},};
 	fwrite(quad, 4, 256, fp);
    // write data
    fwrite(img, 1, bitsize, fp);
    // write padding
    char padd[2] = {0,};
    fwrite(padd, 1, 2, fp);
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
	for(int i=0, pos=0, rpos=imgWidth*imgHeight
		; i<imgHeight
		; ++i, pos += (stripe), rpos-=(stripe))
    {
		memcpy(conv+pos, bmp+rpos-(stripe), (stripe));
    }
 
#endif

    return (conv);
}
#endif



/// @brief color to grayscale
/// @param const ch_vec &
/// @param ch_vec &
/// @return ch_vec &
ch_vec& to_grayscale(const ch_vec &rgb, ch_vec &dst)
{
	if (rgb == dst) return dst;

	size_t rs = rgb.size()/3;

	double gray = 0.0 ;
	for (size_t i=0, j=0; i<rs; ++i, j+=3)
	{
		// gray = (red * 0.299) + (green * 0.587) + (blue * 0.114)
		gray = rgb[j+0] * 0.114;
		gray += rgb[j+1] * 0.587;
		gray += rgb[j+2] * 0.299;
		dst[i] = static_cast<byte>(gray+0.5);
	}

	return dst;
}

ch_vec& to_grayscale(const byte *data, int width, int height, ch_vec &dst)
{
	if (width * height < 1) return dst;

	dst.clear();
	dst.reserve(width*height);

	double gray;
	for (int y=0; y<height; ++y)
	{
		for (int x=0; x<width; ++x)
		{
			// gray = (red * 0.299) + (green * 0.587) + (blue * 0.114)
			//gray = *(data+(y*width)+x) * 0.299;
			//gray += *(data+(y*width)+x+1) * 0.587;
			//gray += *(data+(y*width)+x+2) * 0.114;
			gray = data[(y*width)+x] * 0.299;
			gray += data[(y*width)+x+1] * 0.587;
			gray += data[(y*width)+x+2] * 0.114;
			dst[(y*width)+x] = static_cast<byte>(gray);
		}

	}

	return dst;
}


/// #biref	ToGrey
void ToGrey(CBitmap& color,CBitmap& grey,unsigned int bitspixel)
{
  BITMAP    bmp;
  // get the bitmap dimensions
  if(color.m_hObject && color.GetObject(sizeof(bmp),&bmp))
  {
    BITMAP          bmpg = bmp;
    unsigned int    bpls = bmp.bmWidthBytes; // bytes per line source
    unsigned int    bpld; // bytes per line dest
    unsigned int    bpps,bppd; // bytes per pixel source/dest
    unsigned char*  lps; // source bitmap buffer
    unsigned char*  lpd; // dest bitmap buffer
    unsigned char*  lines; // current line source
    unsigned char*  lined; // current line dest
    unsigned char*  linee; // line end pointer
    unsigned char*  lpps; // pixel pointer source
    unsigned char*  lppd; // pixel pointer dest
    unsigned char*  lppe; // end of line source
    
    double          ar[256]; // map table for grey values corresponding to the rgb (byte) value
    double          ag[256];
    double          ab[256];
    unsigned int    ix; // index
 
    // bitmap info structure to create dib bitmap
    // including palette
    BITMAPINFO*      bmig = (BITMAPINFO*)malloc(sizeof(BITMAPINFO)+(sizeof(RGBQUAD)*256)); if(!bmig) return;
 
    // fill 0
    memset(bmig->bmiColors,0,sizeof(RGBQUAD)*256);
    // create color mapping tables
    for(ix=0;ix<256;ix++) ar[ix] = 0.2989 * ix;
    for(ix=0;ix<256;ix++) ag[ix] = 0.5870 * ix;
    for(ix=0;ix<256;ix++) ab[ix] = 0.1140 * ix;
    // create grey scale palette
    for(ix=0;ix<256;ix++) bmig->bmiColors[ix].rgbBlue = bmig->bmiColors[ix].rgbGreen = bmig->bmiColors[ix].rgbRed = ix;
 
    // allocating buffer for the source bitmap bits
    lps = (unsigned char*)malloc(bpls * bmp.bmHeight);
    if(lps) // only go a head on existing buffer
    {
      bmpg.bmBitsPixel = bitspixel; // set the pixel width in bits (only 8,24,32 in this demo)
      // calculating the line width in bytes for the gray bitmap
      bmpg.bmWidthBytes = bpld = ((((bmpg.bmWidth*bmpg.bmBitsPixel)+7)>>3) + 3) & ~3;
   
      // inittializing the bitmap info header
      bmig->bmiHeader.biSize          = sizeof(BITMAPINFO);
      bmig->bmiHeader.biWidth         = bmpg.bmWidth;
      bmig->bmiHeader.biHeight        = -bmpg.bmHeight;
      bmig->bmiHeader.biPlanes        = 1;
      bmig->bmiHeader.biBitCount      = bmpg.bmBitsPixel;
      bmig->bmiHeader.biCompression   = 0;
      bmig->bmiHeader.biSizeImage     = bmpg.bmWidthBytes * bmpg.bmHeight;
      bmig->bmiHeader.biXPelsPerMeter = 0;
      bmig->bmiHeader.biYPelsPerMeter = 0;
      bmig->bmiHeader.biClrUsed       = 256;
      bmig->bmiHeader.biClrImportant  = 0;
 
      // allocating the bitmap buffer for the grey bitmap
      lpd = (unsigned char*)malloc(bpld * bmpg.bmHeight);
      if(lpd) // continue only if you got the buffer
      {
        // get the bitmap bits from your original bitmap
        color.GetBitmapBits(bpls * bmp.bmHeight,lps);
 
        lines = lps; lined = lpd;
        // distinguish between the different bitmap resolutions
        // only valid for true color 24 or 32 bits per pixel
        switch(bmp.bmBitsPixel)
        {
          case 24:
          case 32:
            bpps = bmp.bmBitsPixel>>3;
            switch(bmpg.bmBitsPixel)
            {
              case  8:
                // this section makes a bitmap based on palette
                // loop through every line in source bitmap
                for(linee=lps+(bpls*bmp.bmHeight);lines<linee;lines+=bpls,lined+=bpld)
                {
                  // start of the line, set the pointers for source and dest
                  lpps = lines; lppd = lined;
                  // loop through the pixels in one line
                  for(lppe=lines+bpls;lpps<lppe;lpps+=bpps,lppd+=1)
                  {
                    // accumulete the pixel value by table and cast it to 8bit palette index
                    *lppd = (unsigned char)(ar[lpps[2]]+ag[lpps[1]]+ab[lpps[0]]);
                  }
                }
              break;
              case  24:
              case  32:
                // this section creates a bitmap with rgb values
                bppd = bmpg.bmBitsPixel>>3;
                // loop through every line in source bitmap
                for(linee=lps+(bpls*bmp.bmHeight);lines<linee;lines+=bpls,lined+=bpld)
                {
                  lpps = lines; lppd = lined;
                  // loop through the pixels in one line
                  for(lppe=lines+bpls;lpps<lppe;lpps+=bpps,lppd+=bppd)
                  {
                    // set rgb values to grey value
                    lppd[0] =
                    lppd[1] =
                    lppd[2] = (unsigned char)(ar[lpps[2]]+ag[lpps[1]]+ab[lpps[0]]);
                  }
                }
              break;
              default: ASSERT(0); /*not implemented*/ break;
          }
          break;
          default: ASSERT(0); /*not implemented*/ break;
        }
 
        // create the grey bitmap
        // usually the CreateBitmap functions are obsolet, thats why the bitmap is device dependant
        // that means if your screen resolution is 32-bit with this functions you can
        // only create 32-bit bitmaps
        // if you have 256 color resolution you only can create palette based bitmaps
        // oh yes i remember to the good old win 3.0 times
        switch(bmpg.bmBitsPixel)
        {
          case 8:
          {
            HDC    hdc = ::GetDC(0);
            bmig->bmiHeader.biClrImportant = 256;
            // you should use always this function
            // DIB stands for device independant bitmap
			HBITMAP gray;
            grey.Attach(gray=CreateDIBitmap(hdc,&bmig->bmiHeader,CBM_INIT,lpd,bmig,DIB_RGB_COLORS));
            ::ReleaseDC(0,hdc);
          }
          break;
          default:
            // this function creates a device dependant bitmap
            // you should never use that!
            // this function will fail if your screen resolution is 256 or hicolor (16-bit)
            grey.CreateBitmapIndirect(&bmpg); // create the bitmap
            grey.SetBitmapBits(bpld * bmpg.bmHeight,lpd); // apply the bitmap bits
          break;
        }
        free(lpd); // free the buffer
      }
      free(lps); // free the buffer
    }
    free(bmig); // free the buffer
  }
}

/// @brief DrawDIBSection
void DrawDIBSection( HDC hDC, HBITMAP hBitmap, int xDest, int yDest )
{
	HPALETTE hPal;
 
	HDC hDCMem = ::CreateCompatibleDC( hDC );
 
	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	if( ::GetObject(hBitmap, sizeof(ds), &ds ) == 0 )
		return;			// Not a DIB Section
	
	HGDIOBJ hBmpOld =  ::SelectObject(hDCMem, hBitmap);
 
	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << ds.dsBm.bmBitsPixel;
	
	if( ::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE )
	{
		// Create a halftone palette if colors > 256. 
		if( nColors > 256 )
			hPal = ::CreateHalftonePalette(hDC);
		else
		{
			// Create the palette
			RGBQUAD *pRGB = new RGBQUAD[nColors];
			
			::GetDIBColorTable( hDCMem, 0, nColors, pRGB );
			
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
			
			pLP->palVersion = 0x300;
			pLP->palNumEntries = nColors;
			
			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = pRGB[i].rgbRed;
				pLP->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}
			
			hPal = ::CreatePalette( pLP );
						delete[] pLP;
			delete[] pRGB;
		}
 
		HPALETTE hPalOld = ::SelectPalette(hDC,hPal,FALSE);
		::RealizePalette(hDC);
		BitBlt(hDC,xDest,yDest,bmInfo.biWidth,bmInfo.biHeight,hDCMem,0,0,SRCCOPY);
 
		::SelectPalette(hDC,hPalOld,FALSE);
		// delete GDI objects
		::DeleteObject(hPal);
	}
	else
		BitBlt(hDC,xDest,yDest,bmInfo.biWidth,bmInfo.biHeight,hDCMem,0,0,SRCCOPY);
 
	::SelectObject(hDCMem, hBmpOld);
	::DeleteDC(hDCMem);
}

/// @brief DrawGrayScale
void DrawGrayScale( CDC *pDC, HANDLE hDIB )
{
	CPalette pal;
	CPalette *pOldPalette = NULL;
 
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
				(1 << bmInfo.bmiHeader.biBitCount);
	
 
	// Create the palette if needed
	if( pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE && nColors <= 256 )
	{
		// The device supports a palette and bitmap has color table
		
		// Allocate memory for a palette
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
 
		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;
		
		for( int i=0; i < nColors; i++)
		{
			long lSquareSum = bmInfo.bmiColors[i].rgbRed 
						* bmInfo.bmiColors[i].rgbRed
						+ bmInfo.bmiColors[i].rgbGreen 
						* bmInfo.bmiColors[i].rgbGreen
						+ bmInfo.bmiColors[i].rgbBlue 
						* bmInfo.bmiColors[i].rgbBlue;
			int nGray = (int)sqrt(((double)lSquareSum)/3);
			pLP->palPalEntry[i].peRed = nGray;
			pLP->palPalEntry[i].peGreen = nGray;
			pLP->palPalEntry[i].peBlue = nGray;
			pLP->palPalEntry[i].peFlags = 0;
		}
		
		pal.CreatePalette( pLP );
		
		delete[] pLP;
 
		// Select the palette
		pOldPalette = pDC->SelectPalette(&pal, FALSE);
		pDC->RealizePalette();
	}
	else if((pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) == 0 && nColors <= 256 )
	{
		// Device does not supports palettes but bitmap has a color table
		// Modify the bitmaps color table directly
		// Note : This ends up changing the DIB. If that is not acceptable then
		// copy the DIB and then change the copy rather than the original
 
		for( int i=0; i < nColors; i++)
		{
			long lSquareSum = bmInfo.bmiColors[i].rgbRed 
						* bmInfo.bmiColors[i].rgbRed
						+ bmInfo.bmiColors[i].rgbGreen 
						* bmInfo.bmiColors[i].rgbGreen
						+ bmInfo.bmiColors[i].rgbBlue 
						* bmInfo.bmiColors[i].rgbBlue;
			int nGray = (int)sqrt(((double)lSquareSum)/3);
			bmInfo.bmiColors[i].rgbRed = nGray;
			bmInfo.bmiColors[i].rgbGreen = nGray;
			bmInfo.bmiColors[i].rgbBlue = nGray;
		}
	}
 
 
	int nWidth = bmInfo.bmiHeader.biWidth;
	int nHeight = bmInfo.bmiHeader.biHeight;
	
	
	// Draw the image 
	LPVOID lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	::SetDIBitsToDevice(pDC->m_hDC,	// hDC
		0,				// XDest
		0,				// YDest
		nWidth,				// nDestWidth
		nHeight,			// nDestHeight
		0,				// XSrc
		0,				// YSrc
		0,				// nStartScan
		nHeight,			// nNumScans
		lpDIBBits,			// lpBits
		(LPBITMAPINFO)hDIB,		// lpBitsInfo
		DIB_RGB_COLORS);		// wUsage
	
	
	pDC->SelectPalette(pOldPalette, FALSE);
}


/// @brief	histogram
ch_vec& histogram(const ch_vec &gray, ch_vec &hg)
{
	if (gray.size() < 1) return hg;

	hg.resize(256);
	for (int i=0; i<gray.size(); ++i)
		hg[gray[i]]++;

	return (hg);
}

/// @brief histogram equalization
ch_vec& hg_equalconst(const ch_vec &src, const imgRECT rect
					  , ch_vec &dst)
{
	// histogram
	ch_vec hg(256);
	histogram(src, hg);

	double MN = (rect.cx) * (rect.cy);
	MN = 255;// / MN;

	dst.clear();
	dst.resize(src.size());

	// histogram ezualization
	std::vector<double> eq(256);
	eq[0] = hg[0]/MN;
	for (int i=0; i<256; ++i)	{
		for (int j=0; j<i; ++j)	{
			eq[i] += (hg[j]/MN + 0.5); 
		}
	}

	// set pixel
	for (int y=rect.y, i=0; i<rect.cy; ++y, ++i)	{
		for (int x=rect.x, j=0; j<rect.cx; ++x, ++j)	{
			dst[i*rect.cx+j] = eq[src[i*rect.cx+j]];
		}
	}

	return (dst);
}


/// @brief	draw histogram
int draw_hg(HDC hdc, int x, int y, ch_vec &hg)
{
	if (hg.size() < 256) return (-1);

	int height = 200;

	CDC *dc = CDC::FromHandle(hdc);

	
	for (size_t i=0; i<hg.size(); ++i)	{
		dc->MoveTo(x+i, y+height);
		dc->LineTo(x+i, y+height-(hg[i]/5));
	}
	
	return (0);
}

/// @brief	sobel
ch_vec& sobel(const ch_vec &src, const imgRECT &rect, ch_vec &dst)
{
	if (src == dst || src.size() < 1) return dst;

	char sobel_h[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
	char sobel_v[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
	
	double gx = 0.0, gy = 0.0, Mxy = 0;
	int x2 = rect.x+rect.cx;
	int y2 = rect.y+rect.cy;
	int pos = 0;
	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
			gx = gy = 0.0;

			if (x==1 && y==1)
				x = 1;

			// gy
			for (int zy=-1; zy<2; ++zy)		{
				for(int zx=-1; zx<2; ++zx)		{
					if ( y+zy<0 || x+zx<0 || y+zy>= y2 || x+zx>=x2 )
						gy += 0;
					else
						gy += (int(src[((y+zy)*x2) + x+zx]) * sobel_h[zx+1][zy+1]);
				}

			}
			// gx
			for (int zy=-1; zy<2; ++zy)		{
				for(int zx=-1; zx<2; ++zx)		{
					if ( y+zy<0 || x+zx<0 || y+zy>=y2 || x+zx>=x2 )
						gx += 0;
					else
						gx += (int(src[((y+zy)*x2) + x+zx]) * sobel_v[zx+1][zy+1]);
				}
			}
			// M(x,y)
			Mxy = (abs(gx)+abs(gy));
			Mxy = Mxy > 255.0 ? 255.0 : Mxy < 0.0 ? 0.0 : Mxy;
			dst[(y*x2)+x] = static_cast<byte>(Mxy);
		}

	}
	
	return dst;
}

/// @brief bit_plus
imgRECT bit_plus(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, const imgRECT &rb)
{
	// a 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
    x = rb.x - ra.x;
    x = x < 0 ? 0 : x ;
    y = rb.y - ra.y;
    y = y < 0 ? 0 : y ;
    cx = x + rb.cx > ra.cx ? ra.cx - x : rb.cx ;
    cx = cx > rb.cx + rb.x - ra.x ? rb.cx + rb.x - ra.x :  cx ;
    cy = y + rb.cy > ra.cy ? ra.cy - y : rb.cy ;
    cy = cy > rb.cy + rb.y - ra.y ? rb.cy + rb.y - ra.y : cy ;

    if (x > ra.cx || y > ra.cy || cx < 1 || cy < 1 ) return imgRECT();

	// b 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = ra.cx > rb.x ? 0 : rb.cx-x ;
	by = ra.cy > rb.y ? 0 : rb.cy-y ;

	// bit plus
	size_t pos = (x + y*ra.cx);
	for (int i=0, k=y; i<cy; ++i, ++k)	{
		for (int j=0, l=x; j<cx; ++j, ++l)	{
			a[(i*cx)+j+pos] += b[(k*cx)+l];
		}
	}

	return imgRECT(x, y, cx, cy);
}

/// @brief bit_minus
imgRECT bit_minus(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, const imgRECT &rb)
{
	// a 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
    x = rb.x - ra.x;
    x = x < 0 ? 0 : x ;
    y = rb.y - ra.y;
    y = y < 0 ? 0 : y ;
    cx = x + rb.cx > ra.cx ? ra.cx - x : rb.cx ;
    cx = cx > rb.cx + rb.x - ra.x ? rb.cx + rb.x - ra.x :  cx ;
    cy = y + rb.cy > ra.cy ? ra.cy - y : rb.cy ;
    cy = cy > rb.cy + rb.y - ra.y ? rb.cy + rb.y - ra.y : cy ;

    if (x > ra.cx || y > ra.cy || cx < 1 || cy < 1 ) return imgRECT();

	// b 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = ra.cx > rb.x ? 0 : rb.cx-x ;
	by = ra.cy > rb.y ? 0 : rb.cy-y ;

	// bit minus
	size_t pos = (x + y*ra.cx);
	for (int i=0, k=y; i<cy; ++i, ++k)	{
		for (int j=0, l=x; j<cx; ++j, ++l)	{
			a[(i*cx)+j+pos] -= b[(k*cx)+l];
		}
	}

	return imgRECT(x, y, cx, cy);
}

/// @brief bit_minus
imgRECT bit_minus(const ch_vec& a, const imgRECT& ra
	, const ch_vec& b, const imgRECT& rb
	, ch_vec& c, imgRECT& rc)
{
	// a 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
	x = rb.x - ra.x;
	x = x < 0 ? 0 : x;
	y = rb.y - ra.y;
	y = y < 0 ? 0 : y;
	cx = x + rb.cx > ra.cx ? ra.cx - x : rb.cx;
	cx = cx > rb.cx + rb.x - ra.x ? rb.cx + rb.x - ra.x : cx;
	cy = y + rb.cy > ra.cy ? ra.cy - y : rb.cy;
	cy = cy > rb.cy + rb.y - ra.y ? rb.cy + rb.y - ra.y : cy;

	if (x > ra.cx || y > ra.cy || cx < 1 || cy < 1) return imgRECT();

	// b 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = ra.cx > rb.x ? 0 : rb.cx - x;
	by = ra.cy > rb.y ? 0 : rb.cy - y;

	// bit minus
	c.resize(a.size());
	rc = ra;
	size_t pos = (x + y * ra.cx);
	for (int i = 0, k = y; i < cy; ++i, ++k) {
		for (int j = 0, l = x; j < cx; ++j, ++l) {
			c[(i * cx) + j + pos] = a[(i * cx) + j + pos] - b[(k * cx) + l];
		}
	}

	return imgRECT(x, y, cx, cy);
}


/// #brief	bit_blz 함수
imgRECT bit_blz(ch_vec &a, const imgRECT &ra
	, ch_vec &b, const imgRECT &rb)
{
	// a < b 인 경우는 ?
	bool b_big = a.size() >= b.size() ? false : true;

	// a 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
	x = rb.x - ra.x;
	y = rb.y - ra.y;
	if (b_big)	{
		x = abs(x);
		y = abs(y);
		cx = x + rb.cx > ra.cx ? ra.cx : rb.cx;
		cy = y + rb.cy > ra.cy ? ra.cy : rb.cy;
	}
	else {
		x = x < 0 ? 0 : x;
		y = y < 0 ? 0 : y;
		cx = x + rb.cx > ra.cx ? ra.cx - x : rb.cx;
		cy = y + rb.cy > ra.cy ? ra.cy - y : rb.cy;
	}	
	cx = cx > rb.cx + rb.x - ra.x ? rb.cx + rb.x - ra.x : cx;
	cy = cy > rb.cy + rb.y - ra.y ? rb.cy + rb.y - ra.y : cy;

	if (/*x > ra.cx || y > ra.cy || */cx < 1 || cy < 1) return imgRECT();

	// b 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = ra.cx > rb.x ? 0 : rb.cx - x;
	by = ra.cy > rb.y ? 0 : rb.cy - y;

	// bit blz
	size_t a_pos = 0;
	size_t b_pos = 0;
	int k = y, l = x;
	for (int i = 0; i<cy; ++i, ++k)	{
		for (int j = 0; j < cx; ++j, ++l)	{
			a_pos = (i * ra.cx + j);
			b_pos = k*rb.cx + l;	


			if (b_big && l >= cx+x) {
				if (k < cy - y - 1)
					b_pos += (y * rb.cx + x); // (y == 0 ? x : (i * cx + j) ); // (i * cx + j));
				l = x;
			}
			else if (!b_big) {
				a_pos += (y * ra.cx + x);
				b_pos = (i * cx + j);
			}

			if (a_pos >= a.size())
				a_pos = a.size() - 1;
			if (b_pos >= b.size())
				b_pos = b.size() - 1;

			a[a_pos] = b[b_pos];
		}
	}

	return imgRECT(x, y, cx, cy);
}

/// @brief bit_and
bool bit_and(ch_vec &a, const imgRECT &ra
	, ch_vec &b, const imgRECT &rb, int threshold, int cnt)
{
	// a 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
	x = rb.x - ra.x;
	x = x < 0 ? 0 : x;
	y = rb.y - ra.y;
	y = y < 0 ? 0 : y;
	cx = x + rb.cx > ra.cx ? ra.cx - x : rb.cx;
	cx = cx > rb.cx + rb.x - ra.x ? rb.cx + rb.x - ra.x : cx;
	cy = y + rb.cy > ra.cy ? ra.cy - y : rb.cy;
	cy = cy > rb.cy + rb.y - ra.y ? rb.cy + rb.y - ra.y : cy;

	if (x > ra.cx || y > ra.cy || cx < 1 || cy < 1) return false;

	// b 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = ra.cx > rb.x ? 0 : rb.cx - x;
	by = ra.cy > rb.y ? 0 : rb.cy - y;

	// bit and
	int t = 0;
	size_t pos = x + y*ra.cx;
	for (int i = 0, k = y; i<cy; ++i, ++k)	{
		for (int j = 0, l = x; j<cx; ++j, ++l)	{
			if (a[(k*ra.cx) + l] >= threshold
				&& b[(i*cx) + j] == 255/*b[(k*cx) + l]*/) {
				if (++t >= cnt)	{
					return true;
				}
			}
		}
	}

	return false;
}


/// @brief 차집합
imgRECT set_minus(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, const imgRECT &rb
			 , ch_vec &c, imgRECT &rc)
{
	// a 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
    x = rb.x - ra.x;
    x = x < 0 ? 0 : x ;
    y = rb.y - ra.y;
    y = y < 0 ? 0 : y ;
    cx = x + rb.cx > ra.cx ? ra.cx - x : rb.cx ;
    cx = cx > rb.cx + rb.x - ra.x ? rb.cx + rb.x - ra.x :  cx ;
    cy = y + rb.cy > ra.cy ? ra.cy - y : rb.cy ;
    cy = cy > rb.cy + rb.y - ra.y ? rb.cy + rb.y - ra.y : cy ;

    if (x > ra.cx || y > ra.cy || cx < 1 || cy < 1 ) return imgRECT();

	// b 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = ra.cx > rb.x ? 0 : rb.cx-x ;
	by = ra.cy > rb.y ? 0 : rb.cy-y ;

	// c = a - b
	c = a;
	size_t pos = (x + y*ra.cx);
	for (int i=by; i<cy; ++i)	{
		for (int j=bx; j<cx; ++j)	{
			if (b[(i*cx)+j] != 0 /*== 255*/)
				c[(i*ra.cx)+j+pos] = 0;
		}
	}

	imgRECT rect(x, y, cx, cy);
	return rc = rect;
}

/// @brief set_intersect
imgRECT set_intersect(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, const imgRECT &rb
			 , ch_vec &c, imgRECT &rc)
{
	// a 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
    x = rb.x - ra.x;
    x = x < 0 ? 0 : x ;
    y = rb.y - ra.y;
    y = y < 0 ? 0 : y ;
    cx = x + rb.cx > ra.cx ? ra.cx - x : rb.cx ;
    cx = cx > rb.cx + rb.x - ra.x ? rb.cx + rb.x - ra.x :  cx ;
    cy = y + rb.cy > ra.cy ? ra.cy - y : rb.cy ;
    cy = cy > rb.cy + rb.y - ra.y ? rb.cy + rb.y - ra.y : cy ;

    if (x > ra.cx || y > ra.cy || cx < 1 || cy < 1 ) return imgRECT();

	// b 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = ra.cx > rb.x ? 0 : rb.cx-x ;
	by = ra.cy > rb.y ? 0 : rb.cy-y ;

	size_t pos = (x + y*ra.cx);
	for (int i=by; i<cy; ++i)	{
		for (int j=bx; j<cx; ++j)	{
			if (b[(i*cx)+j] == a[(i*ra.cx)+j+pos])
				c[(i*ra.cx)+j+pos] = b[(i*cx)+j];
			else
				c[(i*ra.cx)+j+pos] = 0;
		}
	}

	return imgRECT(x, y, cx, cy);
}

/// @brief 여집합 -  a는 b의 여집합. a>b 여야 한다.
imgRECT set_complement(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, imgRECT &rb)
{
	// a 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
    x = rb.x - ra.x;
    x = x < 0 ? 0 : x ;
    y = rb.y - ra.y;
    y = y < 0 ? 0 : y ;
    cx = x + rb.cx > ra.cx ? ra.cx - x : rb.cx ;
    cx = cx > rb.cx + rb.x - ra.x ? rb.cx + rb.x - ra.x :  cx ;
    cy = y + rb.cy > ra.cy ? ra.cy - y : rb.cy ;
    cy = cy > rb.cy + rb.y - ra.y ? rb.cy + rb.y - ra.y : cy ;

    if (x > ra.cx || y > ra.cy || cx < 1 || cy < 1 ) return imgRECT();

	// b 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = ra.cx > rb.x ? 0 : rb.cx-x ;
	by = ra.cy > rb.y ? 0 : rb.cy-y ;

	// a는 b의 여집합
	for (int k=0; k<a.size(); ++k)
		a[k] = 255;

	size_t pos = (x + y*ra.cx);
	for (int i=by; i<cy; ++i)	{
		for (int j=bx; j<cx; ++j)	{
			a[(i*ra.cx)+j+pos] = ~b[(i*cx)+j];
		}
	}

	return imgRECT(ra);
}



/// @brief	set_union
imgRECT set_union(ch_vec &a, const imgRECT &ra
			 , ch_vec &b, const imgRECT &rb
			 , ch_vec &c, imgRECT &rc)
{
	/// @todo 합집합은 일단 보류.
	return imgRECT();
}

/// @brief compare for qsort
int comp_int (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
double comp_dbl (const void * a, const void * b)
{
  return ( *(double*)a - *(double*)b );
}

/// @brief order_statistics(순서-통계(비선형)필터)
ch_vec& order_statistics(const ch_vec &src, int width, int height
						 , ch_vec &dst, int kernel)
{
	// 중간값 index
	size_t m = 0.5+(kernel*kernel/2.0);

	// dst ready
	dst.clear();
	dst.reserve(width*height);
	dst.resize(width*height);

	// 중간값필터 blurring
	ch_vec z(kernel*kernel);

	int pos = 0, kpos = 0;
	for (int y=0; y<height; ++y)	{
		for (int x=0; x<width; ++x)		{
			pos = (y*width)+x;

			for (int k = -(kernel / 2.0), zy = 0; zy<kernel; ++k, ++zy)	{
				for (int l = -(kernel / 2.0), zx = 0; zx<kernel; ++l, ++zx)	{

					kpos = (k*width) + l;
					if (kpos + pos<0
						|| kpos+pos>=(width)*(height)) 
						z[(zy*kernel)+zx] = 0;
					else
						z[(zy*kernel)+zx] = src[pos+kpos];
				}
			}

			std::sort(std::begin(z), std::end(z));
			dst[pos] = z[m];
		}
	}

	return dst;
}

ch_vec& unsharp_masking(const ch_vec &src, int width, int height
						, ch_vec &dst)
{
	// 1. blur
	ch_vec blur(width*height);
	order_statistics(src, width, height, blur, 3);

	// 2. src - blur = mask
	ch_vec mask(width*height);
//	mask.insert(mask.begin(), src.begin(), src.end());
	mask = src;
	imgRECT rt(0, 0, width, height);
	bit_minus(mask, rt, blur, rt);
	
	// dst ready
//	dst.clear();
	dst = src;

	// 3. src + mask = dst
	bit_plus(dst, rt, mask, rt);

	return dst;
}


/// @brief	otsu
ch_vec& otsu(const ch_vec &src, const imgRECT &rect
			 , ch_vec &dst, double &threshold)
{
	// 1. 정규화 히스토그램
	ch_vec hg(256);
	histogram(src, hg);
	double mn = rect.cx * rect.cy;
	db_vec Pi(256);	// 정규화 히스토그램
	for (int i=0; i<hg.size(); ++i)	{
		Pi[i] = hg[i] / mn;	
	}
	
	// 2. 누적합
	db_vec Pk(256);	// 누적합	
	for (int k=0; k<Pk.size(); ++k)	{
		for (int i=0; i<k; ++i)	{
			Pk[k] += Pi[i];
		}
	}

	// 3. 누적평균
	db_vec mk(256);
	for (int k=0; k<mk.size(); ++k)	{
		for (int i=0; i<k; ++i)		{
			mk[k] = i * Pi[i];
		}
	}
	
	// 4. 전역 평균 밝기
	double mg = 0.0;
	for (int i=0; i<Pi.size(); ++i)	{
		mg += (i * Pi[i]);
	}
	
	// 5.클래스간 분산
	db_vec var_b(256);
	for (int k=0; k<var_b.size(); ++k)	{
		if (Pk[k] == 0 || Pk[k] == 1)
			var_b[k] = 0;
		else {
			var_b[k] = pow(((mg*Pk[k])-mk[k]), 2);
			var_b[k] /= (Pk[k]*(1-Pk[k]));
		}
	}

	// 6. k* 구하기
//	db_vec var_max(var_b);
//	qsort(&var_max[0], var_max.size(), sizeof(double), comp_dbl);
//	double var_max = std::max_element(var_b.begin(), var_b.end());
//	auto var_max = std::minmax_element(var_b.begin(), var_b.end());
	double var_max = 0.0;
	for (int i=0; i<var_b.size(); ++i)	{
		var_max = var_max < var_b[i] ? var_b[i] : var_max ;
	}
	ch_vec k_conj;
	for (int k=0; k<var_b.size(); ++k)	{
		if (var_b[k] == var_max)
			k_conj.push_back(k);
	}
	
	// 7. 에타* 구하기
	double eta_conj = 0.0;
	for (int i=0; i<k_conj.size(); ++i)	{
		eta_conj += k_conj[i];
	}
	eta_conj /= k_conj.size();
	eta_conj += 0.5;

	// 문턱치 구하기.
	threshold = eta_conj;

	// 이진화
	int pos = 0;
	int y2 = rect.y + rect.cy;
	int x2 = rect.x + rect.cx;
	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
			pos = (y*rect.cx)+x;
			dst[pos] = src[pos] > threshold ? 255 : 0 ;
		}
	}

	return dst;
}

/// @ brief segmentation for 이진화영상.
rt_set&	segmentation(const ch_vec &src, const imgRECT &rect
					 , rt_set &seg)
{
	if (src.size() < 1) return seg;

	seg.clear();

	int pos = 0, kpos = 0;
	int y2 = rect.y + rect.cy;
	int x2 = rect.x + rect.cx;
	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
seg_start:
			pos = (y*rect.cx)+x;
			// segmantation
			if (src[pos] == 255 )	{
				// skip seg				
				for (auto it=seg.begin();
					it != seg.end();
					++it)	{
					if (x > it->x && x < it->x+it->cx
						&& y > it->y && y < it->y+it->cy)	{
						++x;
						goto seg_start;
					}
				}

				imgRECT frt(x-1, y-1, 3, 3);
				int fend = 15;
				while (fend != 0)	{
					if (frt.x<=0 || frt.y<=0 
						|| frt.x+frt.cx>=rect.cx
						|| frt.y+frt.cy>=rect.cy)
						break;					

					// scan north
					kpos = (frt.y)*rect.cx+frt.x;
					for (int n1=kpos, n2=0
						; n2<frt.cx+1
						; ++n1, ++n2)	{
						if (n1 < rect.cx*rect.cy
							&& src[n1] == 255)	{
							frt.y>0 ? --frt.y : frt.y=frt.y;
							frt.y+frt.cy<rect.cy ? 
								++frt.cy : frt.cy=frt.cy;
							fend |= 1;
							break;
						} else 
							fend &= ~1;
					}	

					// scan west
					kpos = (frt.y)*rect.cx+frt.x;
					for (int w1=kpos, w2=0
						; w2<frt.cy+1
						; w1+=rect.cx, ++w2)	{
						if (w1 < rect.cx*rect.cy
							&& src[w1] == 255)	{
							 frt.x>0? --frt.x : frt.x=frt.x;
							 frt.x+frt.cx<rect.cx ? 
								 ++frt.cx : frt.cx=frt.cx;
							 fend |= 2;
							 break;
						} else 
							fend &= ~2;
					}
					
					// scan south
					kpos = (frt.y)*rect.cx+frt.x;
					for (int s1=kpos+(frt.cy*rect.cx), s2=0
						;  s2<frt.cx+1
						; ++s1, ++s2)	{
						if (s1 < rect.cx*rect.cy
							&& src[s1] == 255)	{
							 frt.y+frt.cy<rect.cy ?
								 ++frt.cy : frt.cx=frt.cx;
							 fend |= 4;
							 break;
						} else 
							fend &= ~4;
					}

					// scan east
					kpos = (frt.y)*rect.cx+frt.x ;
					for (int e1=kpos+frt.cx, e2=0
						; e2<frt.cy+1
						; e1+=rect.cx, ++e2)	{
						if (e1 < rect.cx*rect.cy
							&& src[e1] == 255)	{
							 frt.x+frt.cx<rect.cx ? 
								 ++frt.cx : frt.cx=frt.cx;
							 fend |= 8;
							 break;
						} else 
							fend &= ~8;
					}
				};

				// frt 여백 추가 - pos 버그 보정용
				frt.x-=2; frt.y--; frt.cx+=3; frt.cy+=2;
				// 비트맵 틀어지기 방지용패팅
				frt.cx += (4-(frt.cx%4));
				frt.cy += (4-(frt.cy%4));
				seg.insert(frt);
			}
		}
	}

	return seg;
}

/// @ brief segmentation for 이진화영상.
rt_vec&	segmentation(const ch_vec &src, const imgRECT &rect
	, rt_vec &seg)
{
	if (src.size() < 1) return seg;

	seg.clear();

	int pos = 0;
	int kpos = 0;
	int y2 = rect.y + rect.cy;
	int x2 = rect.x + rect.cx;
	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
seg_start:
			pos = (y*rect.cx)+x;
			// segmantation
			if (src[pos] == 255 )	{
				// skip seg				
				for (auto it=seg.begin();
					it != seg.end();
					++it)	{
					if (x >= it->x && x <= it->x+it->cx
						&& y >= it->y && y <= it->y+it->cy)	{
						++x;
						goto seg_start;
					}
				}

				imgRECT frt(x-1, y-1, 3, 3);
				int fend = 15;
				while (fend != 0)	{
					if (frt.x<=0 || frt.y<=0 
						|| frt.x+frt.cx>=rect.cx
						|| frt.y+frt.cy>=rect.cy)
						break;
					
					// fend 수정
					fend = 15;

					// scan north
					kpos = (frt.y)*rect.cx+frt.x;
					for (int n1=kpos, n2=0
						; n2<frt.cx+1
						; ++n1, ++n2)	{
						if (n1 < rect.cx*rect.cy
							&& src[n1] == 255)	{
							frt.y>0 ? --frt.y : frt.y=frt.y;
							frt.y+frt.cy<rect.cy ? ++frt.cy 
								: frt.cy=frt.cy;
							fend |= 1;
							break;
						} else 
							fend &= ~1;
					}	

					// scan west
					kpos = (frt.y)*rect.cx+frt.x;
					for (int w1=kpos, w2=0
						; w2<frt.cy+1
						; w1+=rect.cx, ++w2)	{
						if (w1 < rect.cx*rect.cy
							&& src[w1] == 255)	{
							 frt.x>0? --frt.x : frt.x=frt.x;
							 frt.x+frt.cx<rect.cx ? ++frt.cx
								 : frt.cx=frt.cx;
							 fend |= 2;
							 break;
						} else 
							fend &= ~2;
					}
					
					// scan south
					kpos = (frt.y)*rect.cx+frt.x;
					for (int s1=kpos+(frt.cy*rect.cx), s2=0
						;  s2<frt.cx+1
						; ++s1, ++s2)	{
						if (s1 < rect.cx*rect.cy
							&& src[s1] == 255)	{
							 frt.y+frt.cy<rect.cy ? ++frt.cy 
								 : frt.cy=frt.cy;
							 fend |= 4;
							 break;
						} else 
							fend &= ~4;
					}

					// scan east
					kpos = (frt.y)*rect.cx+frt.x ;
					for (int e1=kpos+frt.cx, e2=0
						; e2<frt.cy+1
						; e1+=rect.cx, ++e2)	{
						if (e1 < rect.cx*rect.cy
							&& src[e1] == 255)	{
							 frt.x+frt.cx<rect.cx ? ++frt.cx : frt.cx=frt.cx;
							 fend |= 8;
							 break;
						} else 
							fend &= ~8;
					}
				};

				// frt 여백 추가 - pos 버그 보정용
				frt.x-=2; //frt.y--; frt.cx+=3; frt.cy+=2;
				// 비트맵 틀어지기 방지용패팅
				frt.cx += (4-(frt.cx%4));
				frt.cy += (4-(frt.cy%4));
				seg.emplace_back(frt);
			}
		}
	}

	return seg;
}


/// @brief	seg_copy
ch_vec& seg_copy(const ch_vec &src, const imgRECT &rect
				 , ch_vec &cp, const imgRECT &seg)
{
	// src 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
    x = seg.x - rect.x;
    x = x < 0 ? 0 : x ;
    y = seg.y - rect.y;
    y = y < 0 ? 0 : y ;
    cx = x + seg.cx > rect.cx ? rect.cx - x : seg.cx ;
    cx = cx > seg.cx + seg.x - rect.x ? seg.cx + seg.x - rect.x :  cx ;
    cy = y + seg.cy > rect.cy ? rect.cy - y : seg.cy ;
    cy = cy > seg.cy + seg.y - rect.y ? seg.cy + seg.y - rect.y : cy ;

    if (x > rect.cx || y > rect.cy || cx < 1 || cy < 1 ) return ch_vec();

	// cp 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = rect.cx > seg.x ? 0 : seg.cx-x ;
	by = rect.cy > seg.y ? 0 : seg.cy-y ;

	size_t pos = (x + y*rect.cx);
	for (int i=by; i<cy; ++i)	{
		for (int j=bx; j<cx; ++j)	{
			cp[(i*cx)+j] = src[(i*rect.cx)+j+pos];
		}
	}
	
	return (cp);
}

/// @brief	seg_paste
ch_vec& seg_paste(ch_vec &tar, const imgRECT rect
				  , const ch_vec &paste, const imgRECT &seg)
{
	// tar 의 교차영역
	int x = 0, y = 0, cx = 0, cy = 0;
    x = seg.x - rect.x;
    x = x < 0 ? 0 : x ;
    y = seg.y - rect.y;
    y = y < 0 ? 0 : y ;
    cx = x + seg.cx > rect.cx ? rect.cx - x : seg.cx ;
    cx = cx > seg.cx + seg.x - rect.x ? seg.cx + seg.x - rect.x :  cx ;
    cy = y + seg.cy > rect.cy ? rect.cy - y : seg.cy ;
    cy = cy > seg.cy + seg.y - rect.y ? seg.cy + seg.y - rect.y : cy ;

    if (x > rect.cx || y > rect.cy || cx < 1 || cy < 1 ) return ch_vec();

	// cp 의 교차영역 - cx, cy 는 동일하다.
	int bx = 0, by = 0;
	bx = rect.cx > seg.x ? 0 : seg.cx-x ;
	by = rect.cy > seg.y ? 0 : seg.cy-y ;

	size_t pos = (x + y*rect.cx);
	for (int i=by; i<cy; ++i)	{
		for (int j=bx; j<cx; ++j)	{
			tar[(i*rect.cx)+j+pos] = paste[(i*cx)+j];
		}
	}

	return (tar);
}



/// @brief draw segmaentation
int draw_seg(HDC hdc, const rt_set &seg)
{
	CDC *dc = CDC::FromHandle(hdc);

	for (rt_set::iterator it=seg.begin(); it!=seg.end(); ++it)	{
		dc->Draw3dRect(it->x, it->y, it->cx, it->cy
			, RGB(255,0,0), RGB(255, 0, 0));
	}	

	return (0);
}
int draw_seg(HDC hdc, const rt_vec &seg)
{
	CDC *dc = CDC::FromHandle(hdc);

	for (rt_vec::const_iterator it = seg.begin(); it != seg.end()
		; ++it)	{
		dc->Draw3dRect(it->x, it->y, it->cx, it->cy
			, RGB(255,0,0), RGB(255, 0, 0));
	}	

	return (0);
}

/// @brief affine rotate
ch_vec& affine(const ch_vec &src, imgRECT &rect, ch_vec &dst
			   , double x_rot, double y_rot
			   , double x_expan , double y_expan 
			   , int x_move , int y_move )
{
#if 0	//  순방향.
	 /* output image generation by affine transformation
     and bilinear transformation  */

	double affine[2][2], beta[2];

	/* forward affine transformation */
	affine[0][0] = x_expan * cos(x_rot*PI/180.0);
	affine[0][1] = y_expan * sin(y_rot*PI/180.0);
	affine[1][0] = x_expan * sin(x_rot*PI/180.0);
	affine[1][1] = y_expan * cos(y_rot*PI/180.0);
	beta[0] = x_move;
	beta[1] = y_move;

	double x_new, y_new, x_frac, y_frac;
	double gray_new;
	int gv_new;
	int x, y, m, n;
	int pos; 
	int x_size2 = rect.x+rect.cx;
	int y_size2 = rect.y+rect.cy;
	for (int y=rect.y; y<y_size2; ++y)
	{
		for (int x=rect.x; x<x_size2; ++x)
		{
			pos = y*rect.cx + x;			

			x_new = beta[0] + affine[0][0]*(x-x_size2/2.0)
					+ affine[0][1]*(y-y_size2/2.0) + x_size2/2.0;
			y_new = beta[1] + affine[1][0]*(x-x_size2/2.0)
					+ affine[1][1]*(y-y_size2/2.0) + y_size2/2.0;
			m = (int)floor(x_new);
			n = (int)floor(y_new);
			x_frac = x_new - m;
			y_frac = y_new - n;

			if (m >= 0 && m+1 < x_size2 && n >= 0 && n+1 < y_size2) 
			{
				gray_new = (1.0 - y_frac)*((1.0 - x_frac)*src[n*rect.cx+m] +
							x_frac *src[n*rect.cx+m+1]) +
							y_frac *((1.0 - x_frac)*src[(n+1)*rect.cx+m] +
							x_frac *src[(n+1)*rect.cx+m+1]);
				dst[pos] = (unsigned char)gray_new;
			} 
			else if (m+1 == x_size2 && n >= 0 && n < y_size2 
					|| n+1 == y_size2 && m >= 0 && m < x_size2) 
			{
				dst[pos] = src[n*rect.cx+m];
			} 
			else 
			{
				dst[pos] = 0;
			}			
		}
	}
#endif
#if 1	// 역방향
	 /* output image generation by inverse affine transformation
     and bilinear transformation  */

	double affine[2][2], beta[2];

	/* forward affine transformation */
	affine[0][0] = x_expan * cos(x_rot*PI/180.0);
	affine[0][1] = y_expan * sin(y_rot*PI/180.0);
	affine[1][0] = x_expan * sin(x_rot*PI/180.0);
	affine[1][1] = y_expan * cos(y_rot*PI/180.0);
	beta[0] = x_move;
	beta[1] = y_move;

	/* determination of inverse affine transformation */
	double det, i_affine[2][2], i_beta[2];
	
	det = affine[0][0]*affine[1][1] - affine[0][1]*affine[1][0];
	if (det == 0.0) {
		i_affine[0][0] = 1.0;
		i_affine[0][1] = 0.0;
		i_affine[1][0] = 0.0;
		i_affine[1][1] = 1.0;
		i_beta[0] = -beta[0];
		i_beta[1] = -beta[1];
	} else {
		i_affine[0][0] =  affine[1][1]/det;
		i_affine[0][1] = -affine[0][1]/det;
		i_affine[1][0] = -affine[1][0]/det;
		i_affine[1][1] =  affine[0][0]/det;
		i_beta[0] = -i_affine[0][0]*beta[0]-i_affine[0][1]*beta[1];
		i_beta[1] = -i_affine[1][0]*beta[0]-i_affine[1][1]*beta[1];
	}

	double x_new, y_new, x_frac, y_frac;
	double gray_new;
	int gv_new;
	int x, y, m, n;
	int pos; 
	int x_size2 = rect.x+rect.cx;
	int y_size2 = rect.y+rect.cy;
	for (int y=rect.y; y<y_size2; ++y)	{
		for (int x=rect.x; x<x_size2; ++x)	{
			pos = y*rect.cx + x;			

			x_new = i_beta[0] + i_affine[0][0]*(x-x_size2/2.0)
					+ i_affine[0][1]*(y-y_size2/2.0) + x_size2/2.0;
			y_new = i_beta[1] + i_affine[1][0]*(x-x_size2/2.0)
					+ i_affine[1][1]*(y-y_size2/2.0) + y_size2/2.0;
			m = (int)floor(x_new);
			n = (int)floor(y_new);
			x_frac = x_new - m;
			y_frac = y_new - n;

			if (m >= 0 && m+1 < x_size2 && n >= 0 
				&& n+1 < y_size2) {
				gray_new = (1.0 - y_frac)*((1.0 - x_frac)*src[n*rect.cx+m] +
							x_frac *src[n*rect.cx+m+1]) +
							y_frac *((1.0 - x_frac)*src[(n+1)*rect.cx+m] +
							x_frac *src[(n+1)*rect.cx+m+1]);
				dst[pos] = (unsigned char)gray_new;
			} else if (m+1 == x_size2 && n >= 0 && n < y_size2 
					|| n+1 == y_size2 && m >= 0 && m < x_size2) {
				dst[pos] = src[n*rect.cx+m];
			} else {
				dst[pos] = 0;
			}			
		}
	}
#endif	
	return (dst);
}


/// @brief contour
ch_vec& contour(const ch_vec &src, const imgRECT &rect, ch_vec &dst, double Tm)
{
	// Tm = 179(70%), A = 90도, Ta = 45도, K는 25픽셀

	// M, Ax, Ay 영상을 소벨로 얻는다.
	ch_vec M(src.size()), Ax(src.size()), Ay(src.size());

	char sobel_h[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
	char sobel_v[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
	
	double gx = 0.0, gy = 0.0, Mxy = 0.0, Axy=0.0;
	int x2 = rect.x+rect.cx;
	int y2 = rect.y+rect.cy;
	int pos = 0;
	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
			pos = (y*x2)+x;
			gx = gy = 0.0;

			if (x==1 && y==1)
				x = 1;

			// gy
			for (int zy=-1; zy<2; ++zy)	{
				for(int zx=-1; zx<2; ++zx)	{
					if ( y+zy<0 || x+zx<0 || y+zy>= y2 
						|| x+zx>=x2 )
						gy += 0;
					else
						gy += (int(src[((y+zy)*x2) + x+zx])
						* sobel_h[zx+1][zy+1]);
				}

			}
			// gx
			for (int zy=-1; zy<2; ++zy)	{
				for(int zx=-1; zx<2; ++zx)	{
					if ( y+zy<0 || x+zx<0 || y+zy>=y2 
						|| x+zx>=x2 )
						gx += 0;
					else
						gx += (int(src[((y+zy)*x2) + x+zx]) 
						* sobel_v[zx+1][zy+1]);
				}
			}
			// M(x,y)
			gx = abs(gx);
			gy = abs(gy);
			Mxy = gx + gy;
			Mxy = Mxy > 255.0 ? 255.0 : Mxy < 0.0 ? 0.0 : Mxy;
			M[pos] = static_cast<byte>(Mxy);
			// Axy
			Ax[pos] = static_cast<byte>(gx);
			Ay[pos] = static_cast<byte>(gy);
		}
	}

	// bin 으로 만들기
	imgRECT rt(rect);
	pos = 0;
	for (int y=rt.y; y<y2; ++y)	{
		for (int x=rt.x; x<x2; ++x)	{
			pos = y*x2 + x;
			M[pos] = M[pos] > Tm ? 255 : 0 ;
			Ax[pos] = Ax[pos] > Tm ? 255 : 0 ;
			Ay[pos] = Ay[pos] > Tm ? 255 : 0 ;
		}
	}
	
	// 현재는 굳이 컨투어 안해도 된다. 김구정.
	//saveBMP("Am.bmp", M.data(), rect.cx
	//		, rect.cy, 1);
	//saveBMP("Ax.bmp", Ax.data(), rect.cx
	//		, rect.cy, 1);
	//saveBMP("Ay.bmp", Ay.data(), rect.cx
	//		, rect.cy, 1);

	// 회전하기
	ch_vec Ax1(M.size()), Ax2(M.size()), Ay1(M.size()), Ay2(M.size());
	affine(Ax, rt, Ax1, 45, -45);
	affine(Ax, rt, Ax2, -45, 45);
	affine(Ay, rt, Ay1, 45, -45);
	affine(Ay, rt, Ay2, -45, 45);

	// K = 25
	fill_line(Ax1, rt, Ax1, 25);
	fill_line(Ay1, rt, Ay1, 25);
	fill_line(Ax2, rt, Ax2, 25);
	fill_line(Ay2, rt, Ay2, 25);
	
	// 회전 원위치
	affine(Ax1, rt, Ax1, -45, 45);
	affine(Ax2, rt, Ax2, 45, -45);
	affine(Ay1, rt, Ay1, -45, 45);
	affine(Ay2, rt, Ay2, 45, -45);
	
	// M OR Axy
	x2 = rt.x+rt.cx;
	y2 = rt.y+rt.cy;
	pos = 0;
	byte	ch;
	for (int y=rt.y; y<y2; ++y)	{
		for (int x=rt.x; x<x2; ++x)	{
			pos = y*x2 + x;
			ch = M[pos];
			ch |= Ax[pos];
			ch |= Ay[pos];
			ch |= Ax1[pos];
			ch |= Ay1[pos];
			ch |= Ax2[pos];
			ch |= Ay2[pos];
			
			dst[pos] = ch > Tm ? 255 : 0;

		}
	}

	return (dst);
}


/// @brief fill_line
ch_vec& fill_line(const ch_vec &src, const imgRECT &rect, ch_vec &dst
				, int K)
{
	dst = src;

	int x2 = rect.x+rect.cx;
	int y2 = rect.y+rect.cy;
	int pos = 0;
	int s = -1, e = 0;
	for (int y=rect.y; y<y2; ++y)	{
		s = -1; e = 0;	// new line start
		for (int x=rect.x; x<x2; ++x)	{
			pos = (y*x2)+x;
			
			if (-1 < s)	{
				++e;
			}	

			if (-1 < s
				&& e > 0 && e < K		// fill limit
				&& dst[pos] == 255)	{
				// fill line
				for (int i=0; i<e; ++i)	{
					dst[s+i] = 255;  
				}
				s = -1; e = 0;
			}		

			if (e > K)	{
				s = -1; e = 0;
			}

			if (dst[pos] == 255
				&& e == 0)	{
				s = pos;		// fill start 이조건은 꼭 뒤에 있어야 한다.
			}

		}
	}

	return (dst);
}


/// @brief	gaussian
ch_vec& gaussian(const ch_vec &src, const imgRECT &rect, ch_vec &dst)
{
	// init kernel
	size_t kernel = 5;
	size_t zm = 159;
	ch_vec z {2,4,5,4,2
			, 4,9,12,9,4
			, 5,12,15,12,5
			, 4, 9, 12, 9, 4
			, 2, 4, 5, 4, 2};
	double zv = 0.0;

	int x2 = rect.x + rect.cx;
	int y2 = rect.y + rect.cy;
	int pos = 0, kpos = 0;
	for (int y = rect.y; y < y2; ++y)	{
		for (int x = rect.x; x < x2; ++x)	{
			pos = (y*x2) + x;

			zv = 0.0;
			for (int k = -(kernel / 2.0), zy = 0; zy<kernel; ++k, ++zy)	{
				for (int l = -(kernel / 2.0), zx = 0; zx<kernel; ++l, ++zx)	{
					
					kpos = (k*rect.cx) + l;
					if (!(kpos + pos<0
						|| kpos + pos>=(rect.cx)*(rect.cy)))	{
						zv += z[(zy*kernel) + zx] * src[pos + kpos];
					}
				}
			}

			zv /= zm;
			zv = zv > 255.0 ? 255.0 : zv < 0.0 ? 0.0 : zv;
			dst[pos] = zv;
		}
	}

	return dst;
}

/// @brief	canny
ch_vec& canny(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, double t_high, double t_low)
{
	//	1. gaussian fileter
	ch_vec gauss(src.size());
	gaussian(src, rect, gauss);
	
	//	2. gradient of sobel
	db_vec M(src.size());
	db_vec Gx(src.size()), Gy(src.size());

	char sobel_h[3][3] = { { -1, -2, -1 }, { 0, 0, 0 }, { 1, 2, 1 } };
	char sobel_v[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };

	double gx = 0.0, gy = 0.0, Mxy = 0.0, Axy = 0.0;
	int x2 = rect.x + rect.cx;
	int y2 = rect.y + rect.cy;
	int pos = 0;
	for (int y = rect.y; y<y2; ++y)	{
		for (int x = rect.x; x<x2; ++x)	{
			pos = (y*x2) + x;
			gx = gy = 0.0;

			// gy
			for (int zy = -1; zy<2; ++zy)	{
				for (int zx = -1; zx<2; ++zx)	{
					if (y + zy<0 || x + zx<0 || y + zy >= y2
						|| x + zx >= x2)
						gy += 0;
					else
						gy += (int(gauss[((y + zy)*x2) + x + zx])
						* sobel_h[zx + 1][zy + 1]);
				}

			}
			// gx
			for (int zy = -1; zy<2; ++zy)	{
				for (int zx = -1; zx<2; ++zx)	{
					if (y + zy<0 || x + zx<0 || y + zy >= y2
						|| x + zx >= x2)
						gx += 0;
					else
						gx += (int(gauss[((y + zy)*x2) + x + zx])
						* sobel_v[zx + 1][zy + 1]);
				}
			}
			// M(x,y)
			Gx[pos] = gx = abs(gx);
			Gy[pos] = gy = abs(gy);
			//gx = abs(gx);
			//gy = abs(gy);
			Mxy = gx + gy;
			M[pos] = Mxy;
		}
	}

	//	3. non maximum suppression
	dst.clear();
	dst.reserve(src.size());
	dst.resize(src.size());

	std::vector<BYTE*> stack;
	stack.reserve(M.size());
	double alpha = 0.0;

	// 외곽 1픽셀을 안쓴다.
	--y2; --x2;
	for (int y = rect.y + 1; y<y2; ++y)	{
		for (int x = rect.x + 1; x<x2; ++x)	{
			pos = (y*x2) + x;

			Mxy = M[pos];
			if (Mxy >= t_low) {

				// non maximum suppression - 인터넷을 참고했다.

				gx = Gx[pos];
				gy = Gy[pos];

				{
					alpha = atan2(gy, gx);
					alpha = alpha * 180 / PI;
					//alpha = (int(gy) << 10) / gx;

					{
						if (((alpha > -22.5 && alpha <= 22.5) 
							|| (alpha > 157.5 || alpha <= -157.5))
							&& (Mxy < M[pos - 1] || Mxy < M[pos + 1]))	{
							continue;
						}
						else if (((alpha > 22.5 && alpha <= 67.5)
							|| (alpha > -157.5 && alpha <= -112.5))
							&& (Mxy < M[pos + rect.cx - 1] || Mxy < M[pos - rect.cx + 1]))	{
							continue;
						}
						else if (((alpha > 67.5 && alpha <= 112.5)
							|| (alpha > -112.5 && alpha <= -67.5))
							&& (Mxy < M[pos + rect.cx] || Mxy < M[pos - rect.cx]))	{
							continue;
						}
						else if (((alpha > 112.5 && alpha <= 157.5)
							|| (alpha > -67.5 && alpha <= -22.5))
							&& (Mxy < M[pos + rect.cx + 1] || Mxy < M[pos - rect.cx - 1]))	{
							continue;
						}
					}
				}

				if (Mxy >= t_high)	{
					dst[pos] = 255;
					stack.emplace_back(dst.data() + pos);
				}
				else
					dst[pos] = t_low;
			}
		}
	}
	
	//	4. hyteresis threshold
	for (auto it = stack.begin(); it != stack.end(); ++it)	{
		BYTE *p = (*it);

		if (*(p + 1) == t_low)	{
			*(p + 1) = 255;
			stack.emplace_back((p + 1));
		}
		if (*(p - rect.cx + 1) == t_low)	{
			*(p - rect.cx + 1) = 255;
			stack.emplace_back((p - rect.cx + 1));
		}
		if (*(p - rect.cx) == t_low)	{
			*(p - rect.cx) = 255;
			stack.emplace_back((p - rect.cx));
		}
		if (*(p - rect.cx - 1) == t_low)	{
			*(p - rect.cx - 1) = 255;
			stack.emplace_back((p - rect.cx - 1));
		}
		if (*(p - 1) == t_low)	{
			*(p - 1) = 255;
			stack.emplace_back((p - 1));
		}
		if (*(p + rect.cx - 1) == t_low)	{
			*(p + rect.cx - 1) = 255;
			stack.emplace_back((p + rect.cx - 1));
		}
		if (*(p + rect.cx) == t_low)	{
			*(p + rect.cx) = 255;
			stack.emplace_back((p + rect.cx));
		}
		if (*(p + rect.cx + 1) == t_low)	{
			*(p + rect.cx + 1) = 255;
			stack.emplace_back((p + rect.cx + 1));
		}
	}

	//	5. binary
	for (auto &v : dst)	{
		v = v == 255 ? 255 : 0;
	}

	return dst;
}

ch_vec& canny2(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, double t_high, double t_low, bool L2gradient)
{
	//	1. gaussian fileter
	ch_vec gauss(src.size());
	gaussian(src, rect, gauss);

	//	2. gradient of sobel
	int_vec M(src.size());
	sh_vec Gx(src.size()), Gy(src.size());

	char sobel_h[3][3] = { { -1, -2, -1 }, { 0, 0, 0 }, { 1, 2, 1 } };
	char sobel_v[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };

	double gx = 0.0, gy = 0.0, Mxy = 0.0, Axy = 0.0;
	int x2 = rect.x + rect.cx;
	int y2 = rect.y + rect.cy;
	int pos = 0;
	for (int y = rect.y; y<y2; ++y)	{
		for (int x = rect.x; x<x2; ++x)	{
			pos = (y*x2) + x;
			gx = gy = 0.0;

			// gy
			for (int zy = -1; zy<2; ++zy)	{
				for (int zx = -1; zx<2; ++zx)	{
					if (y + zy<0 || x + zx<0 || y + zy >= y2
						|| x + zx >= x2)
						gy += 0;
					else
						gy += (int(gauss[((y + zy)*x2) + x + zx])
						* sobel_h[zx + 1][zy + 1]);
				}

			}
			// gx
			for (int zy = -1; zy<2; ++zy)	{
				for (int zx = -1; zx<2; ++zx)	{
					if (y + zy<0 || x + zx<0 || y + zy >= y2
						|| x + zx >= x2)
						gx += 0;
					else
						gx += (int(gauss[((y + zy)*x2) + x + zx])
						* sobel_v[zx + 1][zy + 1]);
				}
			}
			// M(x,y)
			Gx[pos] = floor(gx);
			Gy[pos] = floor(gy);
			gx = abs(gx);
			gy = abs(gy);
			Mxy = gx + gy;
			M[pos] = floor(Mxy);
		}
	}

	// 3. non maximum suppression - OpenCV를 참조했다.
	// buffer 는 mag_buf의 범퍼까지 크게 만든다.
	if (L2gradient)
	{
		t_low = min(32767.0, t_low);
		t_high = min(32767.0, t_high);

		if (t_low > 0) t_low *= t_low;
		if (t_high > 0) t_high *= t_high;
	}
	int low = floor(t_low);
	int high = floor(t_high);


	ptrdiff_t mapstep = rect.cx;// +2;
	ch_vec buffer((rect.cx + 2)*(rect.cy + 2) + (mapstep + 2) * 3 * sizeof(int)
		, 1);

	// mag_buf는 링버퍼이다.
	int* mag_buf[3];
	mag_buf[0] = (int*)(uchar*)buffer.data();
	mag_buf[1] = mag_buf[0] + mapstep;
	mag_buf[2] = mag_buf[1] + mapstep;

	memset(mag_buf[0], 0, mapstep*sizeof(int));

	uchar* map = (uchar*)(mag_buf[2] + mapstep);

	// stack은 연결점을 저장한다.
	std::vector<uchar*> stack;
	stack.reserve(M.size());

	for (int i = 0; i < rect.cy; ++i)	{
		int* _norm = mag_buf[(i > 0) + 1] + 1;
		if (i < rect.cy)	{
			short* _dx = (short*)(Gx.data() + (i)*rect.cx);
			short* _dy = (short*)(Gy.data() + (i)*rect.cx);

			if (!L2gradient)
			{
				int j = 0, width = rect.cx;

				for (; j < width; ++j)
					_norm[j] = std::abs(int(_dx[j])) + std::abs(int(_dy[j]));
			}
			else {
				int j = 0, width = rect.cx;

				for (; j < width; ++j)
					_norm[j] = int(_dx[j])*_dx[j] + int(_dy[j])*_dy[j];
			}
		}

		// at the very beginning we do not have a complete ring
		// buffer of 3 magnitude rows for non-maxima suppression
		if (i == 0)
			continue;

		uchar* _map = map + mapstep*i + 1;
		_map[-1] = _map[rect.cx] = 1;

		int* _mag = mag_buf[1] + 1; // take the central row
		ptrdiff_t magstep1 = mag_buf[2] - mag_buf[1];
		ptrdiff_t magstep2 = mag_buf[0] - mag_buf[1];

		const short* _x = (short*)(Gx.data() + (i - 1)*rect.cx);
		const short* _y = (short*)(Gy.data() + (i - 1)*rect.cx);
		
		int prev_flag = 0;
		for (int j = 0; j < rect.cx; ++j)	{
			const int CANNY_SHIFT = 15;
			const int TG22 = (int)(0.4142135623730950488016887242097*(1 << CANNY_SHIFT) + 0.5);

			int m = _mag[j];

			if (m > low)
			{
				int xs = _x[j];
				int ys = _y[j];
				int x = std::abs(xs);
				int y = std::abs(ys) << CANNY_SHIFT;

				int tg22x = x * TG22;

				if (y < tg22x)
				{
					if (m > _mag[j - 1] && m >= _mag[j + 1]) goto __ocv_canny_push;
				}
				else
				{
					int tg67x = tg22x + (x << (CANNY_SHIFT + 1));
					if (y > tg67x)
					{
						if (m > _mag[j + magstep2] && m >= _mag[j + magstep1]) goto __ocv_canny_push;
					}
					else
					{
						int s = (xs ^ ys) < 0 ? -1 : 1;
						if (m > _mag[j + magstep2 - s] && m > _mag[j + magstep1 + s]) goto __ocv_canny_push;
					}
				}
			}
			prev_flag = 0;
			_map[j] = uchar(1);
			continue;
		__ocv_canny_push:
			if (!prev_flag && m > high && _map[j - mapstep] != 2)
			{
				_map[j] = 2;
				stack.emplace_back(_map+j);

				prev_flag = 1;
			}
			else
				_map[j] = 0;
		}

		// scroll the ring buffer
		_mag = mag_buf[0];
		mag_buf[0] = mag_buf[1];
		mag_buf[1] = mag_buf[2];
		mag_buf[2] = _mag;
	}
	
	//	4. hyteresis threshold
	for (auto it = stack.begin(); it != stack.end(); ++it)	{
		BYTE *p = (*it);

		if (*(p + 1) == 0)	{
			*(p + 1) = 2;
			stack.emplace_back((p + 1));
		}
		if (*(p - mapstep + 1) == 0)	{
			*(p - mapstep + 1) = 2;
			stack.emplace_back((p - mapstep + 1));
		}
		if (*(p - mapstep) == 0)	{
			*(p - mapstep) = 2;
			stack.emplace_back((p - mapstep));
		}
		if (*(p - mapstep - 1) == 0)	{
			*(p - mapstep - 1) = 2;
			stack.emplace_back((p - mapstep - 1));
		}
		if (*(p - 1) == 0)	{
			*(p - 1) = 2;
			stack.emplace_back((p - 1));
		}
		if (*(p + mapstep - 1) == 0)	{
			*(p + mapstep - 1) = 2;
			stack.emplace_back((p + mapstep - 1));
		}
		if (*(p + mapstep) == 0)	{
			*(p + mapstep) = 2;
			stack.emplace_back((p + mapstep));
		}
		if (*(p + mapstep + 1) == 0)	{
			*(p + mapstep + 1) = 2;
			stack.emplace_back((p + mapstep + 1));
		}
	}

	//	5. make dst
	dst.clear();
	dst.reserve(src.size());
	dst.resize(src.size());

	// the final pass, form the final image
//	const uchar* pmap = map + mapstep + 1;
	const uchar* pmap = map + mapstep + 1;
	uchar* pdst = dst.data();
	for (int i = 0; i < rect.cy; ++i, pmap += mapstep) {
		for (int j = 0; j < rect.cx; ++j)	{
			//pdst[i] = (uchar)-(pmap[i] >> 1);
			dst[i*rect.cx + j] = (uchar)-(pmap[j] >> 1);
		} 
	}

	return dst;
}




/// @brief 침식
/// -	SE 의 가로/세로는 꼭 홀수이여야 한다.
ch_vec& erode(const ch_vec &src, const imgRECT &rect
				, ch_vec &dst, const byte se[3][3], const imgRECT &se_rt)
{
	// se 복사.
	ch_vec scan(se_rt.cx*se_rt.cy);
	ch_vec scan2;
	scan2.insert(scan2.begin(), *se, *se+scan.size());
	scan = scan2;

	int x2 = rect.x+rect.cx;
	int y2 = rect.y+rect.cy;
	int sx2 = (se_rt.x+se_rt.cx) / 2;
	int sy2 = (se_rt.y+se_rt.cy) / 2;

	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
se_scan:
			memset(const_cast<byte*>(scan.data()), 
				0x00, scan.size());
			for (int zy=-sy2; zy<sy2+1; ++zy)	{
				for(int zx=-sx2; zx<sx2+1; ++zx)	{
					if ( y+zy<0 || x+zx<0 || y+zy>=y2 
						|| x+zx>=x2 )	{
						continue;
					}
										
					if (src[((y+zy)*x2) + x+zx] == se[zy+1][zx+1])
						scan[ (zy+1)*se_rt.cx+(zx+1) ] = 
						se[zy+1][zx+1];
				}
			}
			if (scan == scan2)
				dst[ ((y*x2)+x) ] = 255;
		}
	}
	
	return (dst);
}

ch_vec& erode(const ch_vec &src, const imgRECT &rect
				, ch_vec &dst, const ch_vec &se
				, const imgRECT &se_rt)
{
	// se 복사.
	ch_vec scan(se.size());

	int x2 = rect.x+rect.cx;
	int y2 = rect.y+rect.cy;
	int sx2 = (se_rt.x+se_rt.cx) / 2;
	int sy2 = (se_rt.y+se_rt.cy) / 2;

	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
se_scan:
			memset(const_cast<byte*>(scan.data())
				, 0x00, scan.size());
			for (int zy=-sy2; zy<sy2+1; ++zy)	{
				for(int zx=-sx2; zx<sx2+1; ++zx)	{
					if ( y+zy<0 || x+zx<0 || y+zy>=y2 
						|| x+zx>=x2 )	{
						continue;
					}
										
					if (src[((y+zy)*x2) + x+zx] 
						== se[(zy+sy2)*se_rt.cx+(zx+sx2)])
						scan[(zy+sy2)*se_rt.cx+(zx+sx2)] 
						= se[(zy+sy2)*se_rt.cx+(zx+sx2)];
				}
			}
			if (scan == se)
				dst[ ((y*x2)+x) ] = 255;
		}
	}
	
	return (dst);
}

/// @brief 팽창
ch_vec& dilate(const ch_vec &src, const imgRECT &rect
				, ch_vec &dst, const byte se[3][3]
				, const imgRECT &se_rt)
{
	dst = src;

	int x2 = rect.x+rect.cx;
	int y2 = rect.y+rect.cy;
	int sx2 = (se_rt.x+se_rt.cx) / 2;
	int sy2 = (se_rt.y+se_rt.cy) / 2;

	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
			if ( src[ ((y*x2)+x) ] == 255)	{
				for (int zy=-sy2; zy<sy2+1; ++zy)	{
					for(int zx=-sx2; zx<sx2+1; ++zx)	{
						if ( y+zy<0 || x+zx<0 || y+zy>=y2 
							|| x+zx>=x2 )
							continue;
						dst[((y+zy)*x2) + x+zx] 
							|= se[zy+1][zx+1];
					}
				}
			}
		}
	}

	return (dst);
}

ch_vec& dilate(const ch_vec &src, const imgRECT &rect
				, ch_vec &dst, const ch_vec &se
				, const imgRECT &se_rt)
{
	dst = src;

	int x2 = rect.x+rect.cx;
	int y2 = rect.y+rect.cy;
	int sx2 = (se_rt.x+se_rt.cx) / 2;
	int sy2 = (se_rt.y+se_rt.cy) / 2;

	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
			if ( src[ ((y*x2)+x) ] == 255)	{
				for (int zy=-sy2; zy<sy2+1; ++zy)	{
					for(int zx=-sx2; zx<sx2+1; ++zx)	{
						if ( y+zy<0 || x+zx<0 || y+zy>=y2 
							|| x+zx>=x2 )
							continue;
						dst[((y+zy)*x2) + x+zx] 
							|= se[(zy+sy2)*se_rt.cx+(zx+sx2)];
					}
				}
			}
		}
	}

	return (dst);
}



/// @brief 원점반사
ch_vec& reflect(const ch_vec &src, const imgRECT rect
				, ch_vec &dst)
{
	for (size_t i=0; i<src.size(); ++i)
		dst[src.size()-i-1] = src[i];

	return (dst);
}

/// @brief 열기 후 닫기
ch_vec& open2close(const ch_vec &src, const imgRECT rect
				, ch_vec &dst, imgRECT rt)
{
	// (a 열기 b) 닫기 b
	// se == b
	byte se[3][3] = {{255,255,255}, {255,255,255}, {255,255,255}};
//	byte se[3][3] = {{0,255,0}, {255,255,255}, {0,255,0}};
	imgRECT se_rt(0, 0, 3, 3);

	// 열기
	ch_vec a1(src.size()), a2(src.size());
	erode(src, rect, a1, se, se_rt);
	dilate(a1, rect, a2, se, se_rt);

	// 열기 후 닫기
//	memset(const_cast<byte*>(a1.data()), 0x00, a1.size());
	dilate(a2, rect, a1, se, se_rt);
//	memset(const_cast<byte*>(a2.data()), 0x00, a2.size());
	erode(a1, rect, a2, se, se_rt);
	

	return (dst = a2);
}



/// @brief 적중
ch_vec& match(const ch_vec &a, const imgRECT &seg
			  , ch_vec &dst, imgRECT &dr
			  , const byte se[3][3], imgRECT &se_rt)
{
	// (a 침식 se) - (a 팽창 b2의 반사)

	ch_vec a1(a.size()), a2(a.size());
	imgRECT a1_rt(seg), a2_rt(seg);

	// a 침식 se = a1
	erode(a, a1_rt, a1, se, se_rt);

	// for b2 = W - D
	ch_vec b2((se_rt.cx+2)*(se_rt.cy+2)), b3(b2.size());
	imgRECT b2_rt(0, 0, se_rt.cx+2, se_rt.cy+2);
	ch_vec se2;
	imgRECT se2_rt(1, 1, se_rt.cx, se_rt.cy);
	se2.insert(se2.begin(), *se, *se+9);
	set_complement(b3, b2_rt, se2, se2_rt);
	reflect(b3, b2_rt, b2);

	// a 팽창 b2 반사
	dilate(a1, a2_rt, a2, b2, b2_rt);

	// a1 - a2 = dst
	memset(const_cast<byte*>(dst.data()), 0x00, dst.size());
	// a2 > a1 이라서 바꿈.
//	set_minus(a1, a1_rt, a2, a2_rt, dst, dr);
	set_minus(a2, a2_rt, a1, a1_rt, dst, dr);

	//saveBMP("a0.bmp", dst.data(), seg.cx
	//		, seg.cy, 1);
	//saveBMP("a1.bmp", a1.data(), seg.cx
	//		, seg.cy, 1);
	//saveBMP("a2.bmp", a2.data(), seg.cx
	//		, seg.cy, 1);

	return (dst);
}

#if 1
/// @brief 세선화 - 이 세선화는 잘 안된다.
ch_vec& thinning(const ch_vec &src, const imgRECT &seg
				 , ch_vec &dst)
{
	byte b1[3][3] = {{0,0,0}, {0,255,0}, {255,255,255}};
	byte b2[3][3] = {{0,0,0}, {255,255,0}, {255,255,0}};
	byte b3[3][3] = {{255,0,0}, {255,255,0}, {255,0,0}};
	byte b4[3][3] = {{255,255,0}, {255,255,0}, {0,0,0}};
	byte b5[3][3] = {{255,255,255}, {0,255,0}, {0,0,0}};
	byte b6[3][3] = {{0,255,255}, {0,255,255}, {0,0,0}};
	byte b7[3][3] = {{0,0,255}, {0,255,255}, {0,0,255}};
	byte b8[3][3] = {{0,0,0}, {0,255,255}, {0,255,255}};

	imgRECT se_rt(0, 0, 3, 3);
	imgRECT dr;
	dst = src;
	ch_vec a1(src.size()), a2(src.size()), a3(src.size());
	imgRECT a1_rt(seg), a2_rt(seg), a3_rt(seg), a4_rt(seg);;
	a1 = a2 = a3 = src;
	do {
		a1 = a3;
		match(a1, seg, a2, dr, b1, se_rt);
		set_minus(a1, seg, a2, seg, a3, dr);
		//match(a1, seg, a3, dr, b1, se_rt);
		//set_complement(a2, a2_rt, a3, a3_rt);
		//set_intersect(a1, a1_rt, a2, a2_rt, a3, a3_rt);
		a1 = a3;
		match(a1, seg, a2, dr, b2, se_rt);
		set_minus(a1, seg, a2, seg, a3, dr);
		//match(a1, seg, a3, dr, b2, se_rt);
		//set_complement(a2, a2_rt, a3, a3_rt);
		//set_intersect(a1, a1_rt, a2, a2_rt, a3, a3_rt);
		a1 = a3;
		match(a1, seg, a2, dr, b3, se_rt);
		set_minus(a1, seg, a2, seg, a3, dr);
		//match(a1, seg, a3, dr, b3, se_rt);
		//set_complement(a2, a2_rt, a3, a3_rt);
		//set_intersect(a1, a1_rt, a2, a2_rt, a3, a3_rt);
		a1 = a3;
		match(a1, seg, a2, dr, b4, se_rt);
		set_minus(a1, seg, a2, seg, a3, dr);
		//match(a1, seg, a3, dr, b4, se_rt);
		//set_complement(a2, a2_rt, a3, a3_rt);
		//set_intersect(a1, a1_rt, a2, a2_rt, a3, a3_rt);
		a1 = a3;
		match(a1, seg, a2, dr, b5, se_rt);
		set_minus(a1, seg, a2, seg, a3, dr);
		//match(a1, seg, a3, dr, b5, se_rt);
		//set_complement(a2, a2_rt, a3, a3_rt);
		//set_intersect(a1, a1_rt, a2, a2_rt, a3, a3_rt);
		a1 = a3;
		match(a1, seg, a2, dr, b6, se_rt);
		set_minus(a1, seg, a2, seg, a3, dr);
		//match(a1, seg, a3, dr, b6, se_rt);
		//set_complement(a2, a2_rt, a3, a3_rt);
		//set_intersect(a1, a1_rt, a2, a2_rt, a3, a3_rt);
		a1 = a3; 
		match(a1, seg, a2, dr, b7, se_rt);
		set_minus(a1, seg, a2, seg, a3, dr);
		//match(a1, seg, a3, dr, b7, se_rt);
		//set_complement(a2, a2_rt, a3, a3_rt);
		//set_intersect(a1, a1_rt, a2, a2_rt, a3, a3_rt);
		a1 = a3;
		match(a1, seg, a2, dr, b8, se_rt);
		set_minus(a1, seg, a2, seg, a3, dr);
		//match(a1, seg, a3, dr, b7, se_rt);
		//set_complement(a2, a2_rt, a3, a3_rt);
		//set_intersect(a1, a1_rt, a2, a2_rt, a3, a3_rt);

		dst = a3;

		saveBMP("a1.bmp", a1.data(), seg.cx
			, seg.cy, 1);
		saveBMP("a3.bmp", a3.data(), seg.cx
			, seg.cy, 1);
	} while(dst != a1);

	return (dst);
}
#endif

/// @brief   홀채우기 by fill_line
ch_vec& fill_hole(const ch_vec &src, const imgRECT &rect
				  , ch_vec &dst, const imgRECT seg)
{
	ch_vec A(src.size()), A2(src.size());
	fill_line(src, rect, dst, 5);

	imgRECT a_rt(rect);
	affine(dst, a_rt, A, 90, -90);

	for (int i=0; i<A.size(); ++i)
		A[i] = A[i] > 179 ? 255 : 0 ;

	fill_line(A, a_rt, A2, 5);

	affine(A2, a_rt, dst, -90, 90);

	for (int i=0; i<dst.size(); ++i)
	dst[i] = dst[i] > 179 ? 255 : 0 ;
	
	return (dst);
}

/// @brief	경계추출
ch_vec& boundary(const ch_vec &src, const imgRECT &rect
				 , ch_vec &dst, const imgRECT &seg)
{
	byte se[3][3] = {{255,255,255}, {255,255,255}, {255,255,255}};
	imgRECT se_rt(0, 0, 3, 3);

	// seg copy
	ch_vec a(seg.cx*seg.cy);
	imgRECT a_rt(0, 0, seg.cx, seg.cy);
	seg_copy(src, rect, a, seg);

	// 침식
	ch_vec a2(a.size());
	erode(a, a_rt, a2, se, se_rt);

	// a-a2 = a3
	ch_vec a3(a.size());
	set_minus(a, a_rt, a2, a_rt, a3, a_rt);

	seg_paste(dst, rect, a3, seg);

	return (dst);
}





/// @brief	signature
db_vec& signature(const ch_vec &img, const imgRECT &rect
					, const imgRECT &seg, db_vec &sig)
{
	// sig resize
	sig.clear();
	sig.reserve(360);

	// seg center
	Point cpos((seg.x+seg.cx)/2, (seg.y+seg.cy)/2);

	double angle =  0.0;
	double db_x = 0.0;
	for (int a=0; a<360; ++a)	{
		angle = a*PI / 180.0;

		// calc line f(x)	
		// y = tan(angle)*(x-cpos.first)+cpos.second;

		// exist point in line
		for (int y=seg.y; y<seg.y+seg.cy; ++y)	{
			for (int x=seg.x; x<seg.x+seg.cx; ++x)	{
				if ( img[y*rect.cx+x] == 255)	{
					db_x = tan(angle)*(x-cpos.x)+cpos.y;
					if ( y == floor(db_x) )	{
						// select longest distance point, 못한다.
						// add cos to sigature
						sig.push_back(db_x);
					}
				}
			}
		}
	}
		
	return (sig);
}


/// @brief	signature2
db_vec& signature2(const ch_vec &img, const imgRECT &rect
	, const imgRECT &seg, db_vec &sig)
{
	// init
	double	theta = PI / 180.0;
	int num_theta = lround(PI / theta);
	db_vec _tan;
	_tan.reserve(num_theta);

	double angle = 0.0;
	for (int i = 0; i < num_theta; ++i, angle+=theta)	{
		_tan.emplace_back(tan(angle));
	}

	sig.clear();
	sig.reserve(360);

	// seg center
	Point cpos((seg.x + seg.cx) / 2.0, (seg.y + seg.cy) / 2.0);

	double db_x = 0.0, db_y = 0.0;
	db_vec lines;
	double dist = 0.0;
	angle = 0.0;
	for (int i = 0; i<num_theta; ++i, angle += theta)	{

		// calc line f(x)
		// y = tan(angle)*x 직선방정식 -< 이거 맞나?

		// cpos를 직나는 직선방정식
		// cpos.y = tan(angle)*cpos.x;
		// cpos를 지나는 식선위의 점 (x`, y`)
		// cpos.y - y` = tan(angle)*(cpos.x - x`)

		lines.clear();
		lines.emplace_back(0);	// 라인에 없는 경우.

		// exist point in line
		for (int y = seg.y; y<seg.y + seg.cy; ++y)	{
			for (int x = seg.x; x<seg.x + seg.cx; ++x)	{
				if (img[y*rect.cx + x] == 255)	{
					// is point in line ?
					//db_y = tan(angle)*(cpos.x - x) + y;
					db_y = _tan[i]*(cpos.x - x) + y;
					if (floor(cpos.y) == floor(db_y)) {
						// lines add distance
						// 즉, 거리만 저장하는 것이다
						dist = pow(cpos.x - x, 2) + pow(cpos.y - y, 2);
						dist = sqrt(dist);
						// Point(x`, y`)를 저장할 필요가 있을지 모르겠다.
						lines.emplace_back(dist);
					}
					//else
					//	lines.emplace_back(0);
				}
			}
		}

		// select longest distance point.
		std::sort(std::begin(lines), std::end(lines)
			, [](double a, double b){return a > b; });
		sig.emplace_back(lines[0]);
	}

	return (sig);
}

/// @brief signature3 - 세선화 가 선행되어야 한다.
ch_vec& signature3(const ch_vec &thin, const imgRECT &rect, const imgRECT &seg
	, ch_vec &sig) {
	// make binary
	ch_vec bin(seg.cx * seg.cy);
	seg_copy(thin, rect, bin, seg);

	// set to 8의 배수
	int padd = (bin.size() % 8) ? 8 - (bin.size() % 8) : 0;
	for (int i = 0; i < padd; ++i) {
		bin.push_back(0);
	}

	sig.clear();
	for (int i = 0; i < bin.size(); i+=8) {
		byte c = 0;
		c += bin[i] ? 0x80  : 0 ;
		c += bin[i + 1] ? 0x40 : 0;
		c += bin[i + 2] ? 0x20 : 0;
		c += bin[i + 3] ? 0x10 : 0;
		c += bin[i + 4] ? 0x08 : 0;
		c += bin[i + 5] ? 0x04 : 0;
		c += bin[i + 6] ? 0x02 : 0;
		c += bin[i + 7] ? 0x01 : 0;
		sig.push_back(c);
	}

	return sig;
}


/// @brief	허프변환 - OpenCV 
line_vec& hough_line(const ch_vec &img, const imgRECT &rect, line_vec& lines
	, double rho, double theta, double threshold
	, double min_theta, double max_theta)
{
	// prepare (rho, theta)
	int width = rect.cx;
	int height = rect.cy;
	int num_theta = lround(abs(max_theta - min_theta) / theta);
	int num_rho = lround(((width + height) * 2 + 1) / rho);

	int_vec A((num_rho + 2) * (num_theta + 2));	// local maxima를 위해서 2픽셀 더한다.
	db_vec _sin(num_theta);
	db_vec _cos(num_theta);

	double a = 0.0;
	for (long i = 0; i < num_theta; ++i, a += theta)	{
		_sin[i] = sin(a) / rho;	// 이산화 때문에 rho로 나누는듯 하다.
		_cos[i] = cos(a) / rho;
	}

	// (x,t) -> (rho, theta)
	for (int y = 0; y < height; ++y)	{
		for (int x = 0; x < width; ++x)	{
			if (img[y*width + x] != 0)	{
				for (int i = 0; i < num_theta; ++i)	{
					int r = lround(x*_cos[i] + y*_sin[i]);
					r += (num_rho - 1) / 2;	// -r 를 이동시킨다.
					++A[(i + 1) *(num_rho + 2) + (r + 1)];
				}
			}
		}
	}

	// local maxima
	int_vec maxima;
	maxima.reserve(A.size());
	for (int r = 0; r < num_rho; ++r)	{
		for (int a = 0; a < num_theta; ++a)	{
			int base = (a + 1) * (num_rho + 2) + (r + 1);
			if (A[base] > threshold
				&& A[base] > A[base - 1]
				&& A[base] >= A[base + 1]
				&& A[base] > A[base - num_rho - 2]
				&& A[base] >= A[base + num_rho + 2])
				maxima.emplace_back(base);
		}
	}

	// sort 한다. descent
	std::sort(std::begin(maxima), std::end(maxima)
		, [](int a, int b){ return a > b; });

	// make lines
	lines.clear();
	lines.reserve(maxima.size());

	double scale = 1.0 / (num_rho + 2);
	for (auto &idx : maxima)	{
		LinePolar line;
		//int idx = _sort_buf[i];
		int n = floor(idx*scale) - 1; // 역 이산화인듯
		int r = idx - (n + 1)*(num_rho + 2) - 1;	// 이동했던 rho를 원위치로.
		line.rho = (r - (num_rho - 1)*0.5f) * rho;
		line.theta = min_theta + n * theta;
		lines.emplace_back(line);
	}

	return lines;
}


/// @brief	라인찾기 - 정말 오래걸리고 신뢰도 떨어진다.
ch_vec& find_LinePolar(const ch_vec &src, const imgRECT &rect
	, const line_vec &lines, ch_vec &dst)
{
	// init
	int width = rect.cx, height = rect.cy;

	db_vec _sin, _cos;
	double rho = 0.0, theta = 0.0;
	for (auto &line : lines)	{
		rho = line.rho;
		theta = line.theta;
		_sin.emplace_back(sin(theta) * rho);
		_cos.emplace_back(cos(theta) * rho);
	}

	dst.clear();
	dst.reserve(src.size());
	dst.resize(src.size());

	// find
	int pos = 0;
	for (int y = 0; y < height; ++y)	{
		for (int x = 0; x < width; ++x)	{
			pos = y*width + x;

			if (src[pos] != 0 && dst[pos] != 255)	{
				for (size_t i = 0; i < lines.size(); ++i)	{
					//double rho = lines[i].rho;
					//double x0 = rho * _cos[i];	
					//double y0 = rho * _sin[i];
					double x0 = _cos[i];	// rho 를 미리 곱했다.
					double y0 = _sin[i];

					// y - y0 = (-x0 / y0) * (x - x0)	// 직선의 방정식
					double fx = (-x0 / y0) * (x - x0) + y0;
					if (y == lround(fx))	{
						dst[pos] = 255;
					}
				}
			}
		}
	}

	return dst;
}


/// @brief	허프 원 변환 - 기울기메소드, OpenCV 참조
///			소스분석을 하면서 한글 주석을 먼저 달고 코딩을 시작하자.
circle_vec & hough_circle(const ch_vec &img, const imgRECT &rect
	, circle_vec &circles, double dp, double min_dist
	, double threshold, double min_radius, double max_radius)
{
	// prepare -> canny 등의 엣지디텍터가 선실행되어야 한다.
	const int SHIFT = 10, ONE = 1 << SHIFT;

	int x, y, i, j, k, center_count, nz_count;
	float min_radius2 = (float)min_radius*min_radius;
	float max_radius2 = (float)max_radius*max_radius;
	int rows, cols, arows, acols;
	int astep, *adata;
	float* ddata;
	float idp, dr;

	// Use the Sobel Derivative to compute the local gradient
	// of all the non-zero pixels in the edge image
	sh_vec Gx(img.size()), Gy(img.size());

	char sobel_h[3][3] = { { -1, -2, -1 }, { 0, 0, 0 }, { 1, 2, 1 } };
	char sobel_v[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };

	double gx = 0.0, gy = 0.0, Mxy = 0.0, Axy = 0.0;
	int x2 = rect.x + rect.cx;
	int y2 = rect.y + rect.cy;
	int pos = 0;
	for (int y = rect.y; y<y2; ++y)	{
		for (int x = rect.x; x<x2; ++x)	{
			pos = (y*x2) + x;
			gx = gy = 0.0;

			// gy
			for (int zy = -1; zy<2; ++zy)	{
				for (int zx = -1; zx<2; ++zx)	{
					if (y + zy<0 || x + zx<0 || y + zy >= y2
						|| x + zx >= x2)
						gy += 0;
					else
						gy += (int(img[((y + zy)*x2) + x + zx])
						* sobel_h[zx + 1][zy + 1]);
				}

			}
			// gx
			for (int zy = -1; zy<2; ++zy)	{
				for (int zx = -1; zx<2; ++zx)	{
					if (y + zy<0 || x + zx<0 || y + zy >= y2
						|| x + zx >= x2)
						gx += 0;
					else
						gx += (int(img[((y + zy)*x2) + x + zx])
						* sobel_v[zx + 1][zy + 1]);
				}
			}
			// M(x,y)
			Gx[pos] = floor(gx);
			Gy[pos] = floor(gy);
		}
	}
	
	// Create sequences for the nonzero pixels in the edge image
	// and the centers of circles which could be detected
	int_vec accum;
	fl_vec dist_buf;
	point_vec nz;
	int_vec centers;

	if (dp < 1.f)
		dp = 1.f;
	idp = 1.f / dp;
	accum.reserve(((rect.cx*idp) + 2) * ((rect.cy*idp) + 2));
	accum.resize(((rect.cx*idp) + 2) * ((rect.cy*idp) + 2));

	nz.reserve(((rect.cx*idp) + 2) * ((rect.cy*idp) + 2));
	centers.reserve(((rect.cx*idp) + 2) * ((rect.cy*idp) + 2));

	rows = rect.cy;
	cols = rect.cx;
	arows = ((rect.cy*idp) + 2);
	acols = ((rect.cx*idp) + 2);
	adata = accum.data();
	astep = ((rect.cx*idp) + 2);
	// Accumulate circle evidence for each edge pixel
	for (y = 0; y < rows; ++y)
	{
		const uchar* edges_row = img.data() + y*rect.cx;
		const short* dx_row = (const short*)(Gx.data() + y*rect.cx);
		const short* dy_row = (const short*)(Gy.data() + y*rect.cx);

		for (x = 0; x < cols; x++)
		{
			float vx, vy;
			int sx, sy, x0, y0, x1, y1, r;
			Point pt;

			vx = dx_row[x];
			vy = dy_row[x];

			if (!edges_row[x] || (vx == 0 && vy == 0))
				continue;

			float mag = std::sqrt(vx*vx + vy*vy);
			assert(mag >= 1);
			sx = lround((vx*idp)*ONE / mag);	// mag 로 나누는 이유는 모르겠다.
			sy = lround((vy*idp)*ONE / mag);

			x0 = lround((x*idp)*ONE);
			y0 = lround((y*idp)*ONE);
			// Step from min_radius to max_radius in both directions of the gradient
			for (int k1 = 0; k1 < 2; k1++)
			{
				x1 = x0 + min_radius * sx;
				y1 = y0 + min_radius * sy;

				for (r = min_radius; r <= max_radius; x1 += sx, y1 += sy, r++)
				{
					int x2 = x1 >> SHIFT, y2 = y1 >> SHIFT;
					if ((unsigned)x2 >= (unsigned)acols ||
						(unsigned)y2 >= (unsigned)arows)
						break;
					adata[y2*astep + x2]++;
				}

				sx = -sx; sy = -sy;
			}

			pt.x = x; pt.y = y;
			nz.emplace_back(pt);
		}
	}

	nz_count = nz.size();
	if (!nz_count)
		return circles;
	
	// Find possible circle centers
	for (y = 1; y < arows - 1; ++y)
	{
		for (x = 1; x < acols - 1; ++x)
		{
			int base = y*(acols + 2) + x;
			if (adata[base] > threshold &&
				adata[base] > adata[base - 1] && adata[base] > adata[base + 1] &&
				adata[base] > adata[base - acols - 2] && adata[base] > adata[base + acols + 2])
				centers.emplace_back(base);
		}
	}

	center_count = centers.size();
	if (!center_count)
		return circles;
	
	// Sort candidate centers in descending order of their accumulator values,
	// so that the centers with the most supporting pixels appear first.
//	std::sort(std::begin(centers), std::begin(centers) + center_count
//		, [&accum](int a, int b){ return accum[a] > accum[b]	|| (accum[a] == accum[b] && a < b); });
	std::sort(std::begin(centers), std::begin(centers) + center_count
		, [](int a, int b){ return a > b; });

	int max_v = center_count > nz_count ? center_count : nz_count;
	//int_vec sort_buf(std::max(center_count, nz_count));
	int_vec sort_buf(max_v);


	dist_buf.reserve(nz_count);
	dist_buf.resize(nz_count);
	ddata = dist_buf.data();

	dr = dp;
	min_dist = min_dist > dp ? min_dist : dp ;
	min_dist *= min_dist;
	// For each found possible center
	// Estimate radius and check support
	for (i = 0; i < centers.size(); ++i)
	{
		int ofs = centers[i];
		y = ofs / (acols + 2);
		x = ofs - (y)*(acols + 2);
		//Calculate circle's center in pixels
		float cx = (float)((x + 0.5f)*dp), cy = (float)((y + 0.5f)*dp);
		float start_dist, dist_sum;
		float r_best = 0;
		int max_count = 0;
		// Check distance with previously detected circles
		for (j = 0; j < circles.size(); ++j)
		{
			float c_x = (float)circles[j].x;
			float c_y = (float)circles[j].y;

			if ((c_x - cx)*(c_x - cx) + (c_y - cy)*(c_y - cy) < min_dist)
				break;
		}

		if (j < circles.size())
			continue;
		// Estimate best radius
		for (j = k = 0; j < nz_count; j++)
		{
			Point pt;
			float _dx, _dy, _r2;
			pt = nz[j];
			_dx = cx - pt.x; _dy = cy - pt.y;
			_r2 = _dx*_dx + _dy*_dy;
			if (min_radius2 <= _r2 && _r2 <= max_radius2)
			{
				ddata[k] = _r2;
				sort_buf[k] = k;
				k++;
			}
		}

		int nz_count1 = k, start_idx = nz_count1 - 1;
		if (nz_count1 == 0)
			continue;

		//dist_buf->cols = nz_count1;
		dist_buf.resize(nz_count1);

		//cvPow(dist_buf, dist_buf, 0.5);
		std::for_each(std::begin(dist_buf), std::end(dist_buf)
			, [](float &a){ std::pow(a, 0.5f); });

		// Sort non-zero pixels according to their distance from the center.
		//std::sort(sort_buf.begin(), sort_buf.begin() + nz_count1, cv::hough_cmp_gt((int*)ddata));
//		std::sort(std::begin(dist_buf), std::begin(dist_buf) + nz_count1
//			, [](float a, float b){ return a > b; });
		std::sort(sort_buf.begin(), sort_buf.begin() + nz_count1
			, [&dist_buf](int a, int b){ return dist_buf[a] > dist_buf[b] || (dist_buf[a] == dist_buf[b] && a < b); });


		dist_sum = start_dist = ddata[sort_buf[nz_count1 - 1]];
		//dist_sum = start_dist = ddata[nz_count1 - 1];
		for (j = nz_count1 - 2; j >= 0; j--)
		{
			float d = ddata[sort_buf[j]];
			//float d = ddata[j];

			if (d > max_radius)
				break;

			if (d - start_dist > dr)
			{
				float r_cur = ddata[sort_buf[(j + start_idx) / 2]];
				//float r_cur = ddata[(j + start_idx) / 2];

				if ((start_idx - j)*r_best >= max_count*r_cur ||
					(r_best < FLT_EPSILON && start_idx - j >= max_count))
				{
					r_best = r_cur;
					max_count = start_idx - j;
				}
				start_dist = d;
				start_idx = j;
				dist_sum = 0;
			}
			dist_sum += d;
		}
		// Check if the circle has enough support
		if (max_count > threshold)
		{
			Circle c;
			c.x = cx;
			c.y = cy;
			c.r = r_best;

			circles.emplace_back(c);
		}
	}

	return circles;
}







/// @brief sub sampleing - 알고리즘이 필요하다.
ch_vec& subsampling(const ch_vec &src, const imgRECT &rect
					, ch_vec &dst, double x_expan, double y_expan)
{
	ch_vec expan(src.size());

	double affine[2][2], beta[2];
	double affine2[2][2];

	// 축소
	affine[0][0] = x_expan  ;
	affine[0][1] = 0 ;
	affine[1][0] = 0 ;
	affine[1][1] = y_expan  ;
	beta[0] = 0;
	beta[1] = 0;

	affine2[0][0] = 1/x_expan  ;
	affine2[0][1] = 0 ;
	affine2[1][0] = 0 ;
	affine2[1][1] = 1/y_expan ;

	double x_new, y_new, x_frac, y_frac;
	double gray_new;
	int gv_new;
	int x, y, m, n;
	int pos; 
	int x_size2 = rect.x+rect.cx;
	int y_size2 = rect.y+rect.cy;
	for (int y=rect.y; y<y_size2; y+=1)	{
		for (int x=rect.x; x<x_size2; x+=1)	{
			pos = y*rect.cx + x;			

			x_new = beta[0] + affine[0][0]*(x-x_size2/2.0)
					+ affine[0][1]*(y-y_size2/2.0) + x_size2/2.0;
			y_new = beta[1] + affine[1][0]*(x-x_size2/2.0)
					+ affine[1][1]*(y-y_size2/2.0) + y_size2/2.0;
			
			m = (int)floor(x_new);
			n = (int)floor(y_new);
			x_frac = x_new - m;
			y_frac = y_new - n;

 			x_new = beta[0] + affine2[0][0]*(x_new-x_size2/2.0)
					+ affine2[0][1]*(y_new-y_size2/2.0) + x_size2/2.0;
			y_new = beta[1] + affine2[1][0]*(x_new-x_size2/2.0)
					+ affine2[1][1]*(y_new-y_size2/2.0) + y_size2/2.0;

			m = (int)floor(x_new);
			n = (int)floor(y_new);
			x_frac = x_new - m;
			y_frac = y_new - n;

			if (m >= 0 && m+1 < x_size2 && n >= 0 
				&& n+1 < y_size2) {
				gray_new = (1.0 - y_frac)*((1.0 - x_frac)
							*src[n*rect.cx+m] +
							x_frac *src[n*rect.cx+m+1]) +
							y_frac *((1.0 - x_frac)*src[(n+1)
							*rect.cx+m] +
							x_frac *src[(n+1)*rect.cx+m+1]);
				//gray_new = ( src[n*rect.cx+m] +
				//			src[n*rect.cx+m+1] +
				//			src[(n+1)*rect.cx+m] +
				//			src[(n+1)*rect.cx+m+1] ) / 8;
				dst[pos] = (unsigned char)gray_new;
			} else if (m+1 == x_size2 && n >= 0 && n < y_size2 
					|| n+1 == y_size2 && m >= 0 && m < x_size2) {
				dst[pos] = src[n*rect.cx+m];
			} else {
				dst[pos] = 0;
			}			
		}
	}
	
	return (dst);
}

/// @brief	Lanczos resampling
double lanczos_size_ = 3.0;
inline double sinc(double x) {
  double pi = 3.1415926;
  x = (x * pi);
  if (x < 0.01 && x > -0.01)
    return 1.0 + x*x*(-1.0/6.0 + x*x*1.0/120.0);
  return sin(x)/x;
}
inline double LanczosFilter(double x) {
  if (std::abs(x) < lanczos_size_) {
    double pi = 3.1415926;
    return sinc(x)*sinc(x/lanczos_size_);
  } else {
    return 0.0;
  }
}
ch_vec& resampling(const ch_vec &src, const imgRECT &rect
				   , ch_vec &dst, imgRECT &rt)
{
	// resampling 구조를 만든다.
	dst.clear();
	dst.resize(BOX_SIZE);
	rt.cx = BOX_PIXEL*BOX_WIDTH; rt.cy = BOX_PIXEL*BOX_HEIGHT;
	
	// dst에 박스 긋기
	rt_vec boxs;
	imgRECT box; 
	for (int qy=0; qy<rt.cy; qy+=BOX_PIXEL)	{
		for (int qx=0; qx<rt.cx; qx+=BOX_PIXEL)	{
			box.x = qx; box.y = qy; box.cx = BOX_PIXEL; box.cy = BOX_PIXEL;
			boxs.push_back(box);
		}
	}

	int x2 = rect.x + rect.cx;
	int y2 = rect.y + rect.cy;
	int pos = 0;
	for (int y=rect.y; y<y2; ++y)	{
		for (int x=rect.x; x<x2; ++x)	{
			pos = (y*rect.cx + x);
			
			// resampling in box size
			// 리샘플링이 잘 안된다. - 이유는 모르겠다.
			if (src[pos] == 255) {
				for (auto &box : boxs) {
					// in box pixel
					if (x > box.x && x < box.x+box.cx
						&& y > box.y && y < box.y+box.cy
						) {
						dst[box.y*(rt.cx)+box.x] = 255;
						//dst[y*(rt.cx)+x] = 255;
						break;
					}
				}
			}
		}
	}

	return (dst);
}

ch_vec& resize(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, imgRECT &rt)
{
	double x_expan, y_expan;

	// resize rate
	x_expan = (double)rt.cx / (double)rect.cx;
	y_expan = (double)rt.cy / (double)rect.cy;

	/* output image generation by inverse affine transformation
	and bilinear transformation  */

	double affine[2][2], beta[2];

	/* forward affine transformation */
	affine[0][0] = x_expan;
	affine[0][1] = 0 ;
	affine[1][0] = 0 ;
	affine[1][1] = y_expan;
	beta[0] = 0;
	beta[1] = 0;

	/* determination of inverse affine transformation */
	double det, i_affine[2][2], i_beta[2];

	det = affine[0][0] * affine[1][1] - affine[0][1] * affine[1][0];
	if (det == 0.0) {
		i_affine[0][0] = 1.0;
		i_affine[0][1] = 0.0;
		i_affine[1][0] = 0.0;
		i_affine[1][1] = 1.0;
		i_beta[0] = -beta[0];
		i_beta[1] = -beta[1];
	}
	else {
		i_affine[0][0] = affine[1][1] / det;
		i_affine[0][1] = -affine[0][1] / det;
		i_affine[1][0] = -affine[1][0] / det;
		i_affine[1][1] = affine[0][0] / det;
		i_beta[0] = -i_affine[0][0] * beta[0] - i_affine[0][1] * beta[1];
		i_beta[1] = -i_affine[1][0] * beta[0] - i_affine[1][1] * beta[1];
	}
	
	// resize postion
	int x_gap = ((rt.cx - rect.cx)/2);
	int y_gap = ((rt.cy - rect.cy)/2);
	int gap_cx = rt.cx > rect.cx ? rt.cx : rect.cx;
	int gap_cy = rt.cy > rect.cy ? rt.cy : rect.cy;
//	int pos_gap = y_gap * gap_cx + x_gap;

	// resize buf
	ch_vec buf(gap_cx*gap_cy);
	ch_vec dst_buf(buf.size());
	imgRECT buf_rt(0, 0, gap_cx, gap_cy);
	imgRECT src_rt(x_gap<0?0:x_gap, y_gap<0?0:y_gap, rect.cx, rect.cy);
	imgRECT out_rt = bit_blz(buf, buf_rt, const_cast<ch_vec&>(src), src_rt);

	// ~~
	//saveBMP("mor_resize.bmp", src.data(), rect.cx
	//	, rect.cy, 1);
	//saveBMP("mor_resize2.bmp", buf.data(), gap_cx
	//	, gap_cy, 1);


	// affine
	double x_new, y_new, x_frac, y_frac;
	double gray_new;
	int gv_new;
	int x, y, m, n;
	int pos;
	int x_size2 = gap_cx; //rect.x + rect.cx;
	int y_size2 = gap_cy; //rect.y + rect.cy;
	for (int y = rect.y; y<y_size2; ++y)	{
		for (int x = rect.x; x<x_size2; ++x)	{
			pos = y*x_size2 + x;

			x_new = i_beta[0] + i_affine[0][0] * (x - x_size2 / 2.0)
				+ i_affine[0][1] * (y - y_size2 / 2.0) + x_size2 / 2.0;
			y_new = i_beta[1] + i_affine[1][0] * (x - x_size2 / 2.0)
				+ i_affine[1][1] * (y - y_size2 / 2.0) + y_size2 / 2.0;
			m = (int)floor(x_new);
			n = (int)floor(y_new);
			x_frac = x_new - m;
			y_frac = y_new - n;

			if (m >= 0 && m + 1 < x_size2 && n >= 0
				&& n + 1 < y_size2) {
				gray_new = (1.0 - y_frac)*((1.0 - x_frac)*buf[n*x_size2 + m] +
					x_frac *buf[n*x_size2 + m + 1]) +
					y_frac *((1.0 - x_frac)*buf[(n + 1)*x_size2 + m] +
					x_frac *buf[(n + 1)*x_size2 + m + 1]);
				
				dst_buf[pos] = (unsigned char)gray_new;
			}
			else if (m + 1 == x_size2 && n >= 0 && n < y_size2
				|| n + 1 == y_size2 && m >= 0 && m < x_size2) {

				dst_buf[pos] = buf[n*x_size2 + m];
			}
			//else {
			//	dst[pos] = 0;
			//}
		}
	}


	// ~~
	saveBMP("mor_resize3.bmp", dst_buf.data(), buf_rt.cx
		, buf_rt.cy, 1);


	// dst_buf to dst
	int buf_cx = buf_rt.cx;
	int buf_cy = buf_rt.cy;
	int buf_x = ((rt.cx - buf_cx) / 2) + rt.x;
	int buf_y = ((rt.cy - buf_cy) / 2) + rt.y;

	imgRECT dst_rt(buf_x, buf_y, rt.cx, rt.cy);
	if (buf_x < 0) {
		dst_rt.x = abs(buf_x);
		//buf_x += abs(buf_x);
	}
	if (buf_y < 0) {
		dst_rt.y = abs(buf_y);
		//buf_y += abs(buf_y);
	}
	//imgRECT dst_rt(buf_x, buf_y, rt.cx, rt.cy);

	imgRECT ret = bit_blz(dst, x_gap > 0 ? rt : x_gap == 0 ? buf_rt : dst_rt
		, const_cast<ch_vec&>(dst_buf), buf_rt);

	// ~~
//	saveBMP("mor_resize4.bmp", dst.data(), ret.cx
//		, ret.cy, 1);

	return dst;
}

/// @brief resampling2
/// -
ch_vec& resampling2(const ch_vec &src, const imgRECT &rect
	, ch_vec &dst, size_t s_x, size_t s_y)
{
	// 1. make kernel
	size_t k_x = 3, k_y = 3;
	
	/// 커널 모양을 바꿔보자.
	//ch_vec kernel(k_x * k_y, 0xFF);
	ch_vec kernel{ 0, 255, 0
	, 255, 255, 255
	, 0, 255, 0 };

	int buf_cx = (s_x * k_x);
	int buf_cy = (s_y * k_y);
	int buf_x = ((rect.cx - buf_cx) / 2) + rect.x;
	int buf_y = ((rect.cy - buf_cy) / 2) + rect.y;

	imgRECT src_rt(rect);
	if (buf_x < 0) {
		src_rt.x += abs(buf_x);
		buf_x += abs(buf_x);
	}
	if (buf_y < 0) {
		src_rt.y += abs(buf_y);
		buf_y += abs(buf_y);
	}

	imgRECT buf_rt(buf_x, buf_y, buf_cx, buf_cy);
	ch_vec dst_buf(buf_cx * buf_cy);

	imgRECT ker_rt(0, 0, k_x, k_y);

	// 2. resizing ... OK
	resize(src, rect, dst_buf, buf_rt);

	//// 이진화
	//for (int i = 0; i < dst_buf.size(); ++i) {
	//	dst_buf[i] = dst_buf[i] > 191 ? 255 : 0;
	//}


	// ~~
	saveBMP("resample2_1.bmp", dst_buf.data(), buf_cx
			, buf_cy, 1);


	// 3. resampling
	for (int y = 0, y2=0; y2 < s_y; y += k_y, ++y2)	{
		for (int x = 0, x2=0; x2 < s_x; x += k_x, ++x2)	{
			ker_rt.x = x;
			ker_rt.y = y;
			if (bit_and(dst_buf, buf_rt, kernel, ker_rt, 128, 3))	{
				dst[y2*s_x + x2] = 255;
			}
			else {
				dst[y2*s_x + x2] = 0;
			}
		}
	}


	// ~~
	saveBMP("resample2_2.bmp", dst.data(), s_x
		, s_y, 1);


	return dst;
}


/// @brief chaincode
/// - 체인코드는 4방향만 잘 되고, 리샘플링이 비정상적이다.
/// - 당분간 보류. locketk
ch_vec& chaincode(const ch_vec &src, const imgRECT rect
				  , ch_vec &dst, ch_vec &chain, const imgRECT seg)
{
	// seg copy
	ch_vec a(seg.cx*seg.cy);
	imgRECT a_rt(0, 0, seg.cx, seg.cy);
	seg_copy(src, rect, a, seg);

	//// inner boundary erase
	//ch_vec b, b1(a.size()), b2(a.size());
	//b1 = a;
	//cascade(a, a_rt, b, 1);
	//set_minus(b1, a_rt, a, a_rt, b2, a_rt);

	size_t s_x = 8, s_y = 20;
	ch_vec resample(s_x * s_y); 
	resampling2(a, a_rt, resample, s_x, s_y);
	
//	saveBMP("a.bmp", a.data(), a_rt.cx
//			, a_rt.cy, 1);
//	saveBMP("a1.bmp", resample.data(), s_x
//			, s_y, 1);

	// chaincode
	ch_vec a2(resample.size());
	a2 = resample;
	imgRECT a2_rt(seg.x, seg.y, s_x, s_y);
	imgRECT resample_rt(0, 0, s_x, s_y);
	cascade(resample, resample_rt, chain, 1);

	seg_paste(dst, rect, a2, a2_rt);
//	a2_rt.x = seg.x; a2_rt.y = seg.y;
//	seg_paste(dst, rect, a2, a2_rt); 

	return (dst);
}

/// @brief	boundary following
ch_vec& cascade(ch_vec &img, const imgRECT &rect
				, ch_vec &chain, int expan)
{
	int x2 = rect.x+rect.cx;
	int y2 = rect.y+rect.cy;
	int pos = 0;
	char c = 1;
	int bx=0, by=0;
	int x = 0, y = 0;

	// 시작점 찾기
	for (y=0; y<y2; y+=expan)	{
		for (x=0; x<x2; x+=expan)	{
			pos = y*rect.cx + x;
			if (img[pos] == 255) {
				bx = x;
				by = y;
				goto chain_follow;
			}			
		}
	}

	return (chain);
chain_follow:
	while (1) {
		{
			c = chain_follow(img, rect, x, y, expan, 8);

			if (-1 == c
				//|| (bx==x && by==y)
				)	{
				break;
			} else {
				chain.push_back(c);
				img[y*rect.cx+x] = 0;
			}
		}			
	}

	return (chain);
}

/// @brief	4연결은 잘 되지만, 8연결은 버그가 있다.
///			counter-clockwise
char chain_follow(ch_vec &img, const imgRECT rect
					   ,int &x, int &y, int expan, int type)
{
	char code = -1;
	
	// 4방향체인코드
	//		 0
	//	   1 c 3	
	//		 2
	if (type == 4) {
		if (0 < ((y)*rect.cx) + (x - expan)
			&& img[((y)*rect.cx)+ (x-expan)] == 255) {
			//y = y;
			 x -= expan;

			code = 1;
		}
		else if (img.size() > (y + expan)*rect.cx + (x)
				&& img[(y+expan)*rect.cx+ (x)] == 255) {
			y += expan;
			//x = x;
		
			code = 2;
		}
		else if (img.size() > (y)*rect.cx + (x + expan)
				&& img[(y)*rect.cx+ (x+expan)] == 255) {
			//y = y;
			 x += expan;
		
			code = 3;
		}
		else if (0 < (y - expan)*rect.cx + (x)
				&& img[(y-expan)*rect.cx+ (x)] == 255) {
			y -= expan;
			//x = x;
		
			code = 0;
		}
	} 
	else {  // 8방향 체인코드
			//		1 0 7
			//		2 c	6
			//		3 4 5
		if (0 < ((y - expan)*rect.cx) + (x - expan)
			&& img[((y-expan)*rect.cx)+ (x-expan)] == 255)	{
			y -= expan;
			x -= expan;

			code = 1;
		}
		else if (0 < (y)*rect.cx + (x - expan)
				&& img[(y)*rect.cx+ (x-expan)] == 255) {
			x -= expan;
			
			code = 2;
		}
		else if (img.size() > (y + expan)*rect.cx + (x - expan)
				&& img[(y+expan)*rect.cx+ (x-expan)] == 255) {
			y += expan;
			x -= expan;
		
			code = 3;
		}
		else if (img.size() > (y + expan)*rect.cx + (x)
				&& img[(y+expan)*rect.cx+ (x)] == 255) {
			y += expan;
			
			code = 4; 
		}
		else if (img.size() > (y + expan)*rect.cx + (x + expan)
				&& img[(y+expan)*rect.cx+ (x+expan)] == 255) {
			y += expan;
			x += expan;
		
			code = 5;
		}
		else if (img.size() > (y)*rect.cx + (x + expan)
				&& img[(y)*rect.cx+ (x+expan)] == 255) {	
			 x += expan;
		
			code = 6;
		}
		else if (0 < (y - expan)*rect.cx + (x + expan)
				&& img[(y-expan)*rect.cx+ (x+expan)] == 255) {
			y -= expan;
			x += expan;
		
			code = 7;
		}
		else if (0 < (y - expan)*rect.cx + (x)
				&& img[(y-expan)*rect.cx+ (x)] == 255) {
			y -= expan;
			
			code = 0;
		}
	}

	return (code);
}

/// @brief	min distance classify
char min_dist_classify(const chain_map &chains
					   , const ch_vec &ccode)
{
	if (ccode.size() < 1) return '0';
	
	char ch = 0;

	double xj = 0.0, min_xj = DBL_MAX;
	double mi = 0.0, mj = 0.0;
	byte val;
	int cnt = 0;

	for (auto &ci : ccode) {
		mi += ci;
		++cnt;
	}
	mi /= cnt;

	for (auto &it : chains) {
		// calc mj
		mi = mj = 0.0;
		cnt = 0;
		for (auto &wj : it.second) {
			val = wj - '0';
			if (val < 58) {	// 이부분을 해결 못해서...			
				mj += val;
				++cnt;
			}
		}
		mj /= cnt;//it.second.size();
		
		// distance ?
		xj = 0.0;
		for (auto & i1 : ccode) {
			xj += i1*(mi-mj);
		}

		// euclidan disance ?
		//xj -= (mj*mj/2);
		xj -= (0.5*(mi*mi - mj*mj));

		if (min_xj > fabs(xj)) {
			ch = it.first;
			min_xj = fabs(xj);
		}
	}

	return (ch);
}
/// @brief sig
std::ptrdiff_t min_dist_classify2(const sig_vec &sigs, const ch_vec &sig) {
	std::vector<unsigned long> dists(sigs.size());
	for (int i = 0; i < sigs.size(); ++i) {
		ch_vec sigt(sigs[i]);
		unsigned long dist = 0;
		size_t count = sigt.size() < sig.size() ? sigt.size() : sig.size();
		for (int j = 0; j < count; ++j) {
			dist += std::abs(sigt[j] - sig[j]);
		}
		dists[i] = dist;
	}
	auto r = std::min_element(std::begin(dists), std::end(dists));

	return std::distance(std::begin(dists), r);;
}

std::vector<std::pair<int, double>> min_dist_classify3(const sigf_vec &sigs, const fl_vec &sig, const rt_vec &segs, const imgRECT &rt) {
	double xj = 0.0, mxj = 0.0;
	double mi = 0.0, mj = 0.0;
	std::unordered_map<int, double> dists;
	db_vec mis, mjs;

	//// euclidean disance
	//{
	//	for (auto &i : sig) {
	//		mi += i;
	//	}
	//	for (auto &it : sigs) {

	//		xj = 0.0;
	//		mj = 0.0;
	//		for (int i = 0; i < it.size(); ++i) {
	//			//xj += std::pow(it[i] - sig[i], 2);
	//			//xj += std::fabs(it[i] - sig[i]);
	//			mj += it[i];
	//		}
	//		//xj = std::sqrt(xj);

	//		xj = std::fabs(mi - mj);
	//		dists.push_back(xj);
	//	}
	//}
/*
	int cnt = 0;
	int max_i = 0;
	int min_i = INT_MAX;
	for (int i = 0; i < sig.size(); ++i) {
		if (sig[i]) {
			//mi += 1;// ((32.0 / rt.cx) + (32.0 / rt.cy));
			min_i = i < min_i ? i : min_i;
			max_i = i > max_i ? i : max_i;
			++cnt;
		}
	}
	db_vec mic;
	double mie = 0.0;
	for (int i = 0; i < sig.size(); ++i) {
		if (sig[i]) {
			mi = std::fabs(double(i - min_i) / (max_i - min_i));
			mie += mi;
		}
	}
	mie /= cnt;
	mie /= (((double)rt.cx * 32.0) + ((double)rt.cy * 32.0));

	for (int j = 0; j < sigs.size(); ++j) {
		xj = 0.0;
		mj = 0.0;
		max_i = 0;
		min_i = INT_MAX;
		cnt = 0;
		
		for (int i = 0; i < sigs[j].size(); ++i) {
			if (sigs[j][i]) {
				//mj += 1;
				//mj += ((32.0 / (segs[j].cx)) + (32.0 / segs[j].cy));
				min_i = i < min_i ? i : min_i;
				max_i = i > max_i ? i : max_i;
				++cnt;
			}
		}
		db_vec mjc;
		double mje = 0.0;
		for (int i = 0; i < sigs[j].size(); ++i) {
			if (sigs[j][i]) {
				mj = std::fabs(double(i - min_i) / (max_i - min_i));
				mje += mj;
			}
		}
		mje /= cnt;
		mje /= (((double)segs[j].cx * 32.0) +((double)segs[j].cy * 32.0));

		// euclidean distance
		xj = std::fabs(mie - mje);
		
		mis.push_back(mie);
		mjs.push_back(mje);
		dists[j] = xj;
	}
	*/
	for (int i = 0; i != sigs.size(); ++i) {
		dists[i] = vectorDistance(sig.begin(), sig.end(), sigs[i].begin());
	}


	std::vector<std::pair<int, double>> top_3(5);
	std::partial_sort_copy(dists.begin(),
		dists.end(),
		top_3.begin(),
		top_3.end(),
		[](std::pair<int, double> const& l,
			std::pair<int, double> const& r)
	{
		return l.second < r.second;
	});

	return top_3;
}


std::vector<std::pair<int, double>> 
min_dist_classify4(const sigi_vec& sigs, const int_vec& sig) {
	double xj = 0.0, mxj = 0.0;
	double mi = 0.0, mj = 0.0;
	std::unordered_map<int, double> dists;

	db_vec mis, mjs;

	for (int i = 0; i != sigs.size(); ++i) {
	   dists[i]	= vectorDistance(sig.begin(), sig.end(), sigs[i].begin());
	}

	std::vector<std::pair<int, double>> top_3(5);
	std::partial_sort_copy(dists.begin(),
		dists.end(),
		top_3.begin(),
		top_3.end(),
		[](std::pair<int, double> const& l,
			std::pair<int, double> const& r)
	{
		return l.second < r.second;
	});

	return top_3;
}

template<class Iter_T, class Iter2_T>
double vectorDistance(Iter_T first, Iter_T last, Iter2_T first2) {
	double ret = 0.0;
	while (first != last) {
		double dist = (*first++) - (*first2++);
		ret += dist * dist;
	}
	return ret > 0.0 ? sqrt(ret) : 0.0;
}


int hammingDist(const sig_vec &sigs, const ch_vec &sig)
{
	int i = 0, count = 0;
	std::vector<int> dists;// (sigs.size());
	for (int i = 0; i < sigs.size(); ++i) {
		ch_vec sigt(sigs[i]);
		int dist = i;
		size_t count = sigt.size() < sig.size() ? sigt.size() : sig.size();
		for (int j=0; j<count; ++j)
		{
			if (sigt[j] ^ sig[j])
				++dist;
		}
		//dists[i] = dist;
		dists.push_back(dist);
	}

	auto r = std::min_element(std::begin(dists), std::end(dists));
	return std::distance(std::begin(dists), r);;
}

/// @brief adapt seg position
imgRECT& adapt_seg(imgRECT& seg1) {
	// adapt seg size
	size_t sz = max(seg1.cx, seg1.cy);
	size_t diff = (sz - min(seg1.cx, seg1.cy)) / 2;
	seg1.x -= seg1.cx < seg1.cy ? diff : 0;
	seg1.x = seg1.x < 0 ? 0 : seg1.x;
	seg1.y -= seg1.cy < seg1.cx ? diff : 0;
	seg1.y = seg1.y < 0 ? 0 : seg1.y;
	seg1.cx = sz; seg1.cy = sz;
	return seg1;
}