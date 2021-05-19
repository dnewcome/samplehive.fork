/**
 * @file        Sample.hpp
 * @brief       Defines Sample
 * @author      mill-j
 * @date        04/28/2021
 * @copyright   GNU GPL v3
 */

#include "Sample.hpp"

///Default Constructor, Creates an empty sample profile
Sample::Sample(){}

///Overloaded Constructor, Creates a sample profile with supplied data. @see Set()
Sample::Sample(int favorite, std::string filename,
       std::string fileExtension, std::string samplePack,
       std::string type, int channels, int length,
       int sampleRate, int bitrate, std::string path,
       int trashed)
{
    Set(favorite, filename, fileExtension, samplePack, type, 
       channels, length, sampleRate, bitrate, path, trashed);
}  


///Clears all sample data;
void Sample::Clear() 
{
    sFavorite = 0;
    sFilename = "";
    sFileExtension = "";
    sSamplePack = "";
    sType  = "";
    sChannels = 0;
    sLength = 0;
    sSampleRate  = 0;
    sBitrate  = 0;
    sPath = "";
    sTrashed = 0;
}


int Sample::GetFavorite(){return sFavorite;}
std::string Sample::GetFilename(){return sFilename;}
std::string Sample::GetFileExtension(){return sFileExtension;} 
std::string Sample::GetSamplePack(){return sSamplePack;}
std::string Sample::GetType(){return sType;} 
int Sample::GetChannels(){return sChannels;} 
int Sample::GetLength(){return sLength;}
int Sample::GetSampleRate(){return sSampleRate;} 
int Sample::GetBitrate(){return sBitrate;} 
std::string Sample::GetPath(){return sPath;}
int Sample::GetTrashed (){return sTrashed;}

void Sample::Set(int favorite, std::string filename,
       std::string fileExtension, std::string samplePack,
       std::string type, int channels, int length,
       int sampleRate, int bitrate, std::string path,
       int trashed)
{
    sFavorite = favorite;
    sFilename = filename;
    sFileExtension = fileExtension;
    sSamplePack = samplePack;
    sType = type;
    sChannels = channels;
    sLength = length;
    sSampleRate = sampleRate;
    sBitrate = bitrate;
    sPath = path;
    sTrashed = trashed;
}   

void Sample::SetFavorite(int favorite){sFavorite = favorite;}
void Sample::SetFilename(std::string filename){sFilename = filename;}
void Sample::SetFileExtension(std::string fileExtension){sFileExtension = fileExtension;} 
void Sample::SetSamplePack(std::string samplePack){sSamplePack = samplePack;}
void Sample::SetType(std::string type){sType = type;} 
void Sample::SetChannels(int channels){sChannels = channels;} 
void Sample::SetLength(int length){sLength = length;}
void Sample::SetSampleRate(int sampleRate){sSampleRate = sampleRate;} 
void Sample::SetBitrate(int bitrate){sBitrate = bitrate;} 
void Sample::SetPath(std::string path){sPath = path;}
void Sample::SetTrashed (int trashed){sTrashed = trashed;}
