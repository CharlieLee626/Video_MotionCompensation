//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"
#include <iostream>
#include <cmath>
#include <tuple>
using namespace std;
#define PI 3.1415926

// Constructor and Desctructors
MyImage::MyImage() 
{
	Data = NULL;
	YData = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
}

MyImage::~MyImage()
{
	if ( Data )
		delete Data;
	if (YData)
		delete YData;
}


// Copy constructor
MyImage::MyImage( MyImage *otherImage)
{
	Height = otherImage->Height;
	Width  = otherImage->Width;
	Data   = new unsigned char[Width*Height*3];
	YData  = new unsigned char[Width * Height];
	strcpy(otherImage->ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage->Data[i];
	}

	for (int i = 0; i < (Height * Width); i++)
	{
		YData[i] = otherImage->YData[i];
	}
}



// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
	//MyImage output;
	Height = otherImage.Height;
	Width  = otherImage.Width;
	Data   = new unsigned char[Width*Height*3];
	YData  = new unsigned char[Width * Height];
	strcpy( (char *)otherImage.ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}

	for (int i = 0; i < (Height * Width); i++)
	{
		YData[i] = otherImage.YData[i];
	}
	
	return *this;
}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if ( IN_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	unsigned char *Rbuf = new unsigned char[Height*Width];
	unsigned char *Gbuf = new unsigned char[Height*Width];
	unsigned char *Bbuf = new unsigned char[Height*Width];

	for (i = 0; i < Width*Height; i ++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}
	
	// Allocate Data structure and copy
	Data = new unsigned char[Width*Height*3];
	for (i = 0; i < Height*Width; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1]	= Gbuf[i];
		Data[3*i+2]	= Rbuf[i];

		/*cout << int(Bbuf[i]) << int(Gbuf[i]) << int(Rbuf[i]) << endl;*/
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;
}

// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if ( OUT_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	unsigned char *Rbuf = new unsigned char[Height*Width];
	unsigned char *Gbuf = new unsigned char[Height*Width];
	unsigned char *Bbuf = new unsigned char[Height*Width];

	for (i = 0; i < Height*Width; i++)
	{
		Bbuf[i] = Data[3*i];
		Gbuf[i] = Data[3*i+1];
		Rbuf[i] = Data[3*i+2];
	}

	
	// Write data to file
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}
	
	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;
}

void MyImage::convert()
{
	YData = new unsigned char[Width * Height];
	for (int i = 0; i < Height * Width; i++)
	{
		YData[i] = 0.299 * Data[3 * i + 2] + 0.587 * Data[3 * i + 1] + 0.114 * Data[3 * i];

		// Store the Y channel data back to R, G, B channels, {R, G, B} -> {Y, Y, Y}
		Data[3 * i] = YData[i];
		Data[3 * i + 1] = YData[i];
		Data[3 * i + 2] = YData[i];
	}
}

void MyImage::setYData(unsigned char* input_YData)
{
	Data = new unsigned char[Width * Height * 3];
	for (int i = 0; i < Height * Width; i++)
	{
		// Store the Y channel data back to R, G, B channels, {R, G, B} -> {Y, Y, Y}
		Data[3 * i] = input_YData[i];
		Data[3 * i + 1] = input_YData[i];
		Data[3 * i + 2] = input_YData[i];
	}
}

void MyImage::difference(unsigned char* predict_YData, unsigned char* actual_YData)
// Reconstructed frame(n+1) - frame(n+1)
// Reconstructed frame(n+1) = input_YData
// frame(n+1) = actual_YData
{
	Data = new unsigned char[Width * Height * 3];
	for (int i = 0; i < Height * Width; i++)
	{
		// Store the difference
		Data[3 * i] = abs(predict_YData[i] - actual_YData[i]);
		Data[3 * i + 1] = abs(predict_YData[i] - actual_YData[i]);
		Data[3 * i + 2] = abs(predict_YData[i] - actual_YData[i]);
	}
}
