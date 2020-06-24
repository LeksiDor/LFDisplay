#include <iostream>

#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>


struct Rectangle
{
	double minx;
	double miny;
	double maxx;
	double maxy;
};


std::vector<Rectangle> rectangles;
Rectangle bounds;


bool ReadRectanglesFromFile( const std::string& filepath )
{
	std::fstream filestream;
	rectangles.clear();
	try
	{
		filestream.open( filepath, std::ofstream::in );

		if ( !filestream.is_open() )
			throw;

		if ( filestream.eof() )
			throw;

		while ( !filestream.eof() )
		{
			Rectangle newitem;
			filestream >> newitem.minx >> newitem.miny >> newitem.maxx >> newitem.maxy;
			rectangles.push_back(newitem);
		}
		filestream.close();

	}
	catch ( ... )
	{
		filestream.close();
		return false;
	}
	filestream.close();

	const int numRectangles = rectangles.size();
	if ( numRectangles != 0 )
	{
		bounds = rectangles[0];
		for ( int i = 1; i < numRectangles; ++i )
		{
			const Rectangle& rect = rectangles[i];
			bounds.minx = std::min<double>( bounds.minx, rect.minx );
			bounds.miny = std::min<double>( bounds.miny, rect.miny );
			bounds.maxx = std::max<double>( bounds.maxx, rect.maxx );
			bounds.maxy = std::max<double>( bounds.maxy, rect.maxy );
		}
	}

	return true;
}


// Make the rectangles sheet have the range [-1,1]x[-1,1].
bool NormalizeRectangles()
{
	if ( rectangles.size() == 0 )
		return false;

	const double centx = 0.5 * (bounds.minx + bounds.maxx);
	const double centy = 0.5 * (bounds.miny + bounds.maxy);
	const double scalex = 2.0 / (bounds.maxx - bounds.minx);
	const double scaley = 2.0 / (bounds.maxy - bounds.miny);

	bounds.minx = scalex * (bounds.minx - centx);
	bounds.miny = scaley * (bounds.miny - centy);
	bounds.maxx = scalex * (bounds.maxx - centx);
	bounds.maxy = scaley * (bounds.maxy - centy);

	for ( int i = 0; i < rectangles.size(); ++i )
	{
		Rectangle& rect = rectangles[i];
		rect.minx = scalex * (rect.minx - centx);
		rect.miny = scaley * (rect.miny - centy);
		rect.maxx = scalex * (rect.maxx - centx);
		rect.maxy = scaley * (rect.maxy - centy);
	}

	return true;
}


bool SaveAsPBRTScene( const std::string& filepath )
{
	if ( rectangles.size() == 0 )
		return false;
	bool success = true;
	std::fstream stream;
	try
	{
		stream.open( filepath, std::ofstream::out );
		stream << std::setw(4);
		// White background.
		const double epsilon_shift = 0.00001;
		stream << "# White background" << std::endl;
		stream << "Material \"matte\" \"color Kd\" [1.0 1.0 1.0] Shape \"trianglemesh\" " << std::endl;
		stream << "\"point P\" [";
		stream
			<< -epsilon_shift << " " << bounds.minx << " " << bounds.miny << " "
			<< -epsilon_shift << " " << bounds.maxx << " " << bounds.miny << " "
			<< -epsilon_shift << " " << bounds.maxx << " " << bounds.maxy << " "
			<< -epsilon_shift << " " << bounds.minx << " " << bounds.maxy << " ";
		stream << "]" << std::endl;
		stream << "\"integer indices\" [";
		stream << 0 << " " << 1 << " " << 2 << " " << 2 << " " << 3 << " " << 0 << " ";
		stream << "]" << std::endl;
		stream << std::endl;
		// Black rectangles.
		const int numRectangles = rectangles.size();
		stream << "# Black rectangles" << std::endl;
		stream << "Material \"matte\" \"color Kd\" [0.0 0.0 0.0] Shape \"trianglemesh\" " << std::endl;
		stream << "\"point P\" [";
		for ( int i = 0; i < numRectangles; ++i )
		{
			const Rectangle& rect = rectangles[i];
			stream
				<< 0 << " " << rect.minx << " " << rect.miny << " "
				<< 0 << " " << rect.maxx << " " << rect.miny << " "
				<< 0 << " " << rect.maxx << " " << rect.maxy << " "
				<< 0 << " " << rect.minx << " " << rect.maxy << " ";
		}
		stream << "]" << std::endl;
		stream << "\"integer indices\" [";
		for ( int i = 0; i < numRectangles; ++i )
		{
			stream << 4*i+0 << " " << 4*i+1 << " " << 4*i+2 << " " << 4*i+2 << " " << 4*i+3 << " " << 4*i+0 << " ";
		}
		stream << "]" << std::endl;
	}
	catch ( ... )
	{
		success = false;
	}
	stream.close();
	return success;
}



int main(int argc, char** argv)
{
    std::cout << "USAF-to-PBRT Utility" << std::endl;
    std::cout << std::endl;

	const std::string rectangles_filepath = "../../data/USAF-1951-rectangles.txt";
	const std::string scene_filepath = "../../scenes/geometry/rectangle.pbrt";

	if ( !ReadRectanglesFromFile( rectangles_filepath ) )
	{
		std::cout << "Cannot read rectangles list from file: " << rectangles_filepath << std::endl;
		return 1;
	}

	if ( !NormalizeRectangles() )
	{
		std::cout << "Cannot normalize the list of rectangles." << std::endl;
		return 1;
	}

	if ( !SaveAsPBRTScene( scene_filepath ) )
	{
		std::cout << "Cannot save pbrt-v3 scene file: " << scene_filepath << std::endl;
		return 1;
	}

    return 0;
}