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
 * @brief       Declares Sample
 * @author      mill-j
 * @date        04/28/2021
 * @copyright   GNU GPL v3
 */

#ifndef _SAMPLE_HPP__
#define _SAMPLE_HPP__

#include <string>

/**
 * @class Sample
 * @brief This class holds data for one sample.
 */

class Sample
{
    public:
        Sample();

        Sample(int favorite, const std::string& filename, const std::string& fileExtension,
               const std::string& samplePack, const std::string& type, int channels, int length,
               int sampleRate, int bitrate, const std::string& path, int trashed);

    private:
        // -------------------------------------------------------------------
        int m_Favorite = 0;
        int m_Channels = 0;
        int m_Length = 0;
        int m_SampleRate = 0;
        int m_Bitrate = 0;
        int m_Trashed = 0;
        std::string m_Filename;
        std::string m_FileExtension;
        std::string m_SamplePack;
        std::string m_Type;
        std::string m_Path;

    public:
        // -------------------------------------------------------------------
        // Getters
        // int GetFavorite();
        // int GetChannels();
        // int GetLength();
        // int GetSampleRate();
        // int GetBitrate();
        // int GetTrashed ();
        // std::string GetFilename();
        // std::string GetFileExtension();
        // std::string GetSamplePack();
        // std::string GetType();
        // std::string GetPath();

        int GetFavorite() const { return m_Favorite; }
        int GetChannels() const { return m_Channels; }
        int GetLength() const { return m_Length; }
        int GetSampleRate() const { return m_SampleRate; }
        int GetBitrate() const { return m_Bitrate; }
        int GetTrashed() const { return m_Trashed; }
        std::string GetFilename() const { return m_Filename; }
        std::string GetFileExtension() const { return m_FileExtension; }
        std::string GetSamplePack() const { return m_SamplePack; }
        std::string GetType() const { return m_Type; }
        std::string GetPath() const { return m_Path; }

        // -------------------------------------------------------------------
        // Clear sample data
        void Clear();

        // -------------------------------------------------------------------
        // Setters
        void Set(int favorite, const std::string& filename, const std::string& fileExtension,
                 const std::string& samplePack, const std::string& type, int channels, int length,
                 int sampleRate, int bitrate, const std::string& path, int trashed);

        // void SetFavorite(int favorite);
        // void SetChannels(int channels);
        // void SetLength(int length);
        // void SetSampleRate(int sampleRate);
        // void SetBitrate(int bitrate);
        // void SetTrashed(int trashed);
        // void SetFilename(std::string filename);
        // void SetFileExtension(std::string fileExtension);
        // void SetSamplePack(std::string samplePack);
        // void SetType(std::string type);
        // void SetPath(std::string path);

        void SetFavorite(int favorite) { m_Favorite = favorite; }
        void SetChannels(int channels) { m_Channels = channels; }
        void SetLength(int length) { m_Length = length; }
        void SetSampleRate(int sampleRate) { m_SampleRate = sampleRate; }
        void SetBitrate(int bitrate) { m_Bitrate = bitrate; }
        void SetTrashed(int trashed) { m_Trashed = trashed; }
        void SetFilename(const std::string& filename) { m_Filename = filename; }
        void SetFileExtension(const std::string& fileExtension) { m_FileExtension = fileExtension; }
        void SetSamplePack(const std::string& samplePack) { m_SamplePack = samplePack; }
        void SetType(const std::string& type) { m_Type = type; }
        void SetPath(const std::string& path) { m_Path = path; }
};

#endif
