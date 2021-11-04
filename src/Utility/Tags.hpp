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

#pragma once

#include <string>

#include <wx/string.h>

struct AudioInfo
{
    wxString title;
    wxString artist;
    wxString album;
    wxString genre;
    wxString comment;

    int channels;
    int length;
    int sample_rate;
    int bitrate;
};

class Tags
{
    public:
        Tags(const std::string& filepath);
        ~Tags();

    private:
        // -------------------------------------------------------------------
        const std::string& m_Filepath;

        bool bValid = false;

    public:
        // -------------------------------------------------------------------
        AudioInfo GetAudioInfo();
        void SetTitle(std::string artist);
        void SetArtist(std::string artist);
        void SetAlbum(std::string album);
        void SetGenre(std::string genre);
        void SetComment(std::string comment);

    public:
        // -------------------------------------------------------------------
        inline bool IsFileValid() { return bValid; }
};
