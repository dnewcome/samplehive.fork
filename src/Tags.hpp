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
