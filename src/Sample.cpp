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
Sample::Sample(int favorite, const std::string& filename, const std::string& fileExtension,
               const std::string& samplePack, const std::string& type, int channels, int length,
               int sampleRate, int bitrate, const std::string& path, int trashed)
{
    Set(favorite, filename, fileExtension, samplePack, type, 
        channels, length, sampleRate, bitrate, path, trashed);
}  

///Clears all sample data
void Sample::Clear() 
{
    m_Favorite = 0;
    m_Channels = 0;
    m_Length = 0;
    m_SampleRate = 0;
    m_Bitrate = 0;
    m_Trashed = 0;
    m_Filename = "";
    m_FileExtension = "";
    m_SamplePack = "";
    m_Type = "";
    m_Path = "";
}

// int Sample::GetFavorite() { return m_Favorite; }
// int Sample::GetChannels() { return m_Channels; }
// int Sample::GetLength() { return m_Length; }
// int Sample::GetSampleRate() { return m_SampleRate; }
// int Sample::GetBitrate() { return m_Bitrate; }
// int Sample::GetTrashed () { return m_Trashed; }
// std::string Sample::GetFilename() { return m_Filename; }
// std::string Sample::GetFileExtension() { return m_FileExtension; }
// std::string Sample::GetSamplePack() { return m_SamplePack; }
// std::string Sample::GetType() { return m_Type; }
// std::string Sample::GetPath() { return m_Path; }

void Sample::Set(int favorite, const std::string& filename, const std::string& fileExtension,
		 const std::string& samplePack, const std::string& type, int channels, int length,
                 int sampleRate, int bitrate, const std::string& path, int trashed)
{
    m_Favorite = favorite;
    m_Filename = filename;
    m_FileExtension = fileExtension;
    m_SamplePack = samplePack;
    m_Type = type;
    m_Channels = channels;
    m_Length = length;
    m_SampleRate = sampleRate;
    m_Bitrate = bitrate;
    m_Path = path;
    m_Trashed = trashed;
}   

// void Sample::SetFavorite(int favorite) { m_Favorite = favorite; }
// void Sample::SetChannels(int channels) { m_Channels = channels; }
// void Sample::SetLength(int length) { m_Length = length; }
// void Sample::SetSampleRate(int sampleRate) { m_SampleRate = sampleRate; }
// void Sample::SetBitrate(int bitrate) { m_Bitrate = bitrate; }
// void Sample::SetTrashed (int trashed) { m_Trashed = trashed; }
// void Sample::SetFilename(std::string filename) { m_Filename = filename; }
// void Sample::SetFileExtension(std::string fileExtension) { m_FileExtension = fileExtension; }
// void Sample::SetSamplePack(std::string samplePack) { m_SamplePack = samplePack; }
// void Sample::SetType(std::string type) { m_Type = type; }
// void Sample::SetPath(std::string path) { m_Path = path; }
