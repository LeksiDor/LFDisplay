#include "Image2D.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#define TINYEXR_IMPLEMENTATION
#include "../../3dparty/tinyexr.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../3dparty/stb_image.h"
#include "../../3dparty/stb_image_write.h"


namespace LFOptics
{


Image2D::Image2D()
	:width(0)
	,height(0)
{

}


Image2D::Image2D( const Int width, const Int height )
{
	Resize( width, height );
}


bool Image2D::Set( const Int x, const Int y, const Vec4& value )
{
	if ( x < 0 || y < 0 || x >= Width() || y >= Height() )
		return false;
	data[ x + y * width ] = value;
	return true;
}


const Vec4 Image2D::at( const Int x, const Int y ) const
{
	const Int x_clamped = std::min<Int>( std::max<Int>(x, 0), width-1 );
	const Int y_clamped = std::min<Int>( std::max<Int>(y, 0), height-1 );
	return data[ x_clamped + y_clamped * width ];
}


const Vec4 Image2D::at( const Real x, const Real y, const PixelInterpolation& interpolation ) const
{
	if ( interpolation == PixelInterpolation::Neighbor )
		return at( std::lround(x), std::lround(y) );

	const Int xInt = std::lround(x-0.5);
	const Int yInt = std::lround(y-0.5);

	const Vec4 color00 = at(xInt+0,yInt+0);
	const Vec4 color10 = at(xInt+1,yInt+0);
	const Vec4 color01 = at(xInt+0,yInt+1);
	const Vec4 color11 = at(xInt+1,yInt+1);

	const Real lambdaHor = x - Real(xInt);
	const Real lambdaVert = y - Real(yInt);

	Real weightHor = 0.5, weightVert = 0.5;
	switch ( interpolation )
	{
	case PixelInterpolation::Linear:
		weightHor = lambdaHor;
		weightVert = lambdaVert;
		break;
	case PixelInterpolation::Trapezoidal:
		if ( lambdaHor < 0.25 )
			weightHor = 0.0;
		else if ( lambdaHor > 0.75 )
			weightHor = 1.0;
		else
			weightHor = 2.0*lambdaHor - 0.5;
		if ( lambdaVert < 0.25 )
			weightVert = 0.0;
		else if ( lambdaVert > 0.75 )
			weightVert = 1.0;
		else
			weightVert = 2.0*lambdaVert - 0.5;
		break;
	default:
		break;
	}

	const Vec4 colorLeft  = (1.0-weightVert)*color00 + weightVert*color01;
	const Vec4 colorRight = (1.0-weightVert)*color10 + weightVert*color11;
	const Vec4 color = (1.0-weightHor)*colorLeft + weightHor*colorRight;

	return color;
}


void Image2D::Resize( const Int width, const Int height )
{
	if ( width > 0 && height > 0 )
	{
		this->width = width;
		this->height = height;
		data.resize( width*height, Vec4(RealNaN,RealNaN,RealNaN,RealNaN) );
	}
}


bool Image2D::Load( const std::string& filepath )
{
	const std::string extension = filepath.substr( filepath.length() - 3 );
	if ( extension == "ppm" )
	{
		std::cout << "Loading ppm is not supported yet." << std::endl;
		return false;
	}
	else if ( extension == "exr" )
	{
		return LoadAsEXR(filepath);
	}
	else
	{
		std::cout << "LoadFramebuffer: unknown file format." << std::endl;
		return false;
	}
	return true;
}


bool Image2D::Save( const std::string& filepath ) const
{
	const size_t width = this->width;
	const size_t height = this->height;
	const std::string extension = filepath.substr( filepath.length()-3 );
	bool success = true;
	if ( extension == "ppm" )
	{
		return SaveAsPPM(filepath);
	}
	else if ( extension == "exr" )
	{
		return SaveAsEXR(filepath);
	}
	else if ( extension == "png" )
	{
		return SaveAsPNG(filepath);
	}
	else if ( extension == "bmp" )
	{
		return SaveAsBMP(filepath);
	}
	else if ( extension == "tga" )
	{
		return SaveAsTGA(filepath);
	}
	else if ( extension == "jpg" )
	{
		return SaveAsJPG(filepath);
	}
	else
	{
		std::cout << "SaveFramebuffer: unknown file format." << std::endl;
		success = false;
	}
	return success;
}


bool Image2D::LoadAsEXR(const std::string& filepath)
{
	Int width = 0, height = 0;
	float* rgba = nullptr;
	const char* err;

	const int ret = LoadEXR( &rgba, &width, &height, filepath.c_str(), &err );
	if ( ret != 0 ) {
		printf( "err: %s\n", err );
		//return -1;
		return false;
	}

	this->Resize( width, height );
	Vec4 color;
	for ( Int y = 0; y < height; ++y )
	{
		for ( Int x = 0; x < width; ++x )
		{
			const Int dataId = x+y*width;
			const Vec4 color( rgba[4*dataId+3], rgba[4*dataId+0], rgba[4*dataId+1], rgba[4*dataId+2] );
			this->Set( x, y, color );
		}
	}
	free( rgba );
	return true;
}


bool Image2D::SaveAsPPM(const std::string& filepath) const
{
	bool success = true;
	std::ofstream ofs; // save the framebuffer to file
	ofs.open( filepath, std::ios::binary );
	if ( !ofs.is_open() )
	{
		success = false;
	}
	else
	{
		ofs << "P6\n" << width << " " << height << "\n255\n";
		for ( Int y = 0; y < height; ++y )
		{
			for ( Int x = 0; x < width; ++x )
			{
				const Vec4& color = this->at(x,y);
				for ( size_t channel = 0; channel < 3; ++channel )
				{
					ofs << (char)(255 * std::max( (Real)0, std::min( (Real)1, color[channel] ) ));
				}
			}
		}
		ofs.close();
	}
	return success;
}


bool Image2D::SaveAsEXR(const std::string& filepath) const
{
	bool success = true;

	EXRHeader header;
	InitEXRHeader( &header );

	EXRImage image;
	InitEXRImage( &image );
	image.num_channels = 4;

	std::vector<float> images[4];
	images[0].resize( width * height );
	images[1].resize( width * height );
	images[2].resize( width * height );
	images[3].resize( width * height );

	for ( Int y = 0; y < height; ++y )
	{
		for ( Int x = 0; x < width; ++x )
		{
			const Vec4& color = this->at(x,y);
			images[0][x+y*width] = (float)color[0];
			images[1][x+y*width] = (float)color[1];
			images[2][x+y*width] = (float)color[2];
			images[3][x+y*width] = (float)color[3];
		}
	}

	float* image_ptr[4];
	image_ptr[0] = &(images[3].at(0)); // A
	image_ptr[1] = &(images[2].at(0)); // B
	image_ptr[2] = &(images[1].at(0)); // G
	image_ptr[3] = &(images[0].at(0)); // R


	image.images = (unsigned char**)image_ptr;
	image.width = (int)width;
	image.height = (int)height;

	header.num_channels = 4;
	header.channels = (EXRChannelInfo*)malloc( sizeof( EXRChannelInfo ) * header.num_channels );
	// Must be BGR(A) order, since most of EXR viewers expect this channel order.
	header.channels[0].name[0] = 'B'; header.channels[0].name[1] = '\0';
	header.channels[1].name[0] = 'G'; header.channels[1].name[1] = '\0';
	header.channels[2].name[0] = 'R'; header.channels[2].name[1] = '\0';
	header.channels[3].name[0] = 'A'; header.channels[3].name[1] = '\0';

	header.pixel_types = (int*)malloc( sizeof( int ) * header.num_channels );
	header.requested_pixel_types = (int*)malloc( sizeof( int ) * header.num_channels );
	for ( int i = 0; i < header.num_channels; i++ ) {
		header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
														 //header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
		header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of output image to be stored in .EXR
	}

	const char* err;
	int ret = SaveEXRImageToFile( &image, &header, filepath.c_str(), &err );
	if ( ret != TINYEXR_SUCCESS ) {
		fprintf( stderr, "Save EXR err: %s\n", err );
		success = false;
	}
	free( header.channels );
	free( header.pixel_types );
	free( header.requested_pixel_types );
	return success;
}


void Image2D::Reset()
{
	this->data.clear();
	this->width = 0;
	this->height = 0;
}


void Image2D::Clear( const Vec4& color )
{
	for ( auto iter = data.begin(); iter != data.end(); ++iter )
		*iter = color;
}


bool Image2D::SaveQuantized( const std::string& filepath, const Int& type ) const
{
	const Int width = Width();
	const Int height = Height();
	const Int numChannels = 3;

	std::vector<unsigned char> image_data(width*height*numChannels);
	for ( Int x = 0; x < width; ++x )
	{
		for ( Int y = 0; y < height; ++y )
		{
			const Vec4 colorf = this->at(x,y);
			const Vec3i colori = Vec3i( std::lround(255.0*colorf.x), std::lround(255.0*colorf.y), std::lround(255.0*colorf.z) );
			image_data[numChannels*(x+width*y)+0] = (unsigned char) std::min<Int>( std::max<Int>(colori.x, 0), 255 );
			image_data[numChannels*(x+width*y)+1] = (unsigned char) std::min<Int>( std::max<Int>(colori.y, 0), 255 );
			image_data[numChannels*(x+width*y)+2] = (unsigned char) std::min<Int>( std::max<Int>(colori.z, 0), 255 );
		}
	}

	int result = 0;
	// 0 - png, 1 - bmp, 2 - tga, 3 - jpg.
	switch (type)
	{
	case 0:
		result = stbi_write_png( filepath.c_str(), width, height, numChannels, &image_data[0], width*numChannels );
		break;
	case 1:
		result = stbi_write_bmp( filepath.c_str(), width, height, numChannels, &image_data[0] );
		break;
	case 2:
		result = stbi_write_tga( filepath.c_str(), width, height, numChannels, &image_data[0] );
		break;
	case 3:
		result = stbi_write_jpg( filepath.c_str(), width, height, numChannels, &image_data[0], 95 );
		break;
	default:
		break;
	}
	
	return (result == 1);
}


} // namespace LFOptics