/* SampleHive
 * Copyright (C) 2021  Apoorv Singh
 * A simple, modern audio sample browser/manager for GNU/Linux.
 *
 * This file is a part of SampleHive
 *
 * SampleHive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SampleHive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file        Sample.hpp
 * @brief       Defines Sample
 * @author      mill-j
 * @date        04/28/2021
 * @copyright   GNU GPL v3
 */

#include "Utility/Sample.hpp"

///Default Constructor, Creates an empty sample profile
Sample::Sample()
{

}

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
