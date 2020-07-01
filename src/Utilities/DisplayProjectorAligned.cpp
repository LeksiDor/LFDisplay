#include "DisplayProjectorAligned.h"




bool DisplayProjectorAligned::Save( const std::string& filepath ) const
{
	cv::FileStorage fs( filepath, cv::FileStorage::WRITE );
	if ( !fs.isOpened() )
		return false;

	fs << "viewerdistance" << this->ViewerDistance;
	fs << "resolution"     << this->ProjectorResolution;
	fs << "halfphyssize"   << this->HalfPhysSize;
	fs << "diffusionpower" << this->DiffusionPower;

	if ( this->DiffuserType == Diffuser::Linear )
		fs << "diffusertype" << "linear";
	else if ( this->DiffuserType == Diffuser::Conical )
		fs << "diffusertype" << "conical";
	else
		fs << "diffusertype" << "undefined";

	fs << "projectorlines" << "[";
	for ( auto line = this->ProjectorLines.begin(); line != this->ProjectorLines.end(); ++line )
	{
		fs << "{:"
			<< "start"  << line->start
			<< "step"   << line->step
			<< "number" << line->number
			<< "}";
	}
	fs << "]";

	fs.release();
	return true;
}


bool DisplayProjectorAligned::Load( const std::string& filepath )
{
	cv:: FileStorage fs( filepath, cv::FileStorage::READ );
	if ( !fs.isOpened() )
		return false;

	fs["viewerdistance"] >> this->ViewerDistance;
	fs["resolution"]     >> this->ProjectorResolution;
	fs["halfphyssize"]   >> this->HalfPhysSize;
	fs["diffusionpower"] >> this->DiffusionPower;

	if ( std::string(fs["diffusertype"]) == "linear" )
		this->DiffuserType = Diffuser::Linear;
	else if ( std::string(fs["diffusertype"]) == "conical" )
		this->DiffuserType = Diffuser::Conical;
	else
		this->DiffuserType = Diffuser(-1);

	this->ProjectorLines.clear();
	cv::FileNode projectorLinesList = fs["projectorlines"];
	for ( auto iter = projectorLinesList.begin(); iter != projectorLinesList.end(); ++iter )
	{
		ProjectorLine line;
		(*iter)["start"]  >> line.start;
		(*iter)["step"]   >> line.step;
		(*iter)["number"] >> line.number;
		this->ProjectorLines.push_back( line );
	}

	fs.release();
	return true;
}