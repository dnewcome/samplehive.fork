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

class Sample {
    private:  
        int sFavorite = 0;
        std::string sFilename = "";
        std::string sFileExtension = "";
        std::string sSamplePack = "";
        std::string sType  = "";
        int sChannels = 0;
        int sLength = 0;
        int sSampleRate  = 0;
        int sBitrate  = 0;
        std::string sPath = "";
        int sTrashed = 0;
    public:
        Sample();
    
        Sample(int favorite, std::string filename,
               std::string fileExtension, std::string samplePack,
               std::string type, int channels, int length,
               int sampleRate, int bitrate, std::string path,
               int trashed);
        
        void Clear();
        
        int GetFavorite();
        std::string GetFilename();
        std::string GetFileExtension(); 
        std::string GetSamplePack();
        std::string GetType(); 
        int GetChannels(); 
        int GetLength();
        int GetSampleRate(); 
        int GetBitrate(); 
        std::string GetPath();
        int GetTrashed ();
        
        void Set(int favorite, std::string filename,
               std::string fileExtension, std::string samplePack,
               std::string type, int channels, int length,
               int sampleRate, int bitrate, std::string path,
               int trashed);
        
        void SetFavorite(int favorite);
        void SetFilename(std::string filename);
        void SetFileExtension(std::string fileExtension); 
        void SetSamplePack(std::string samplePack);
        void SetType(std::string type); 
        void SetChannels(int channels); 
        void SetLength(int length);
        void SetSampleRate(int sampleRate); 
        void SetBitrate(int bitrate); 
        void SetPath(std::string path);
        void SetTrashed ( int trashed);
    
};

#endif
