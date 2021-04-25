#include <mutex>
#include <string>

#include <wx/infobar.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/vector.h>
#include <wx/variant.h>

#include <sqlite3.h>

class Database
{
    public:
        Database(wxInfoBar& infoBar);
        ~Database();

    private:
        // -------------------------------------------------------------------
        sqlite3* m_Database;
        int rc;
        char* m_ErrMsg;
        sqlite3_stmt* m_Stmt;
    private:
        // -------------------------------------------------------------------
        wxInfoBar& m_InfoBar;

    public:
        // -------------------------------------------------------------------
        // Create the table
        void CreateDatabase();

        // -------------------------------------------------------------------
        // Insert into database
        void InsertSample(int favorite, std::string filename,
                          std::string fileExtension, std::string samplePack,
                          std::string type, int channels, int length,
                          int sampleRate, int bitrate, std::string path,
                          int trashed);

        // -------------------------------------------------------------------
        // Update database
        void UpdateFavoriteColumn(std::string filename, int value);
        void UpdateFolder(std::string folderName);
        void UpdateFavoriteFolderDatabase(std::string filename,
                                          std::string folderName);
        void UpdateTrashColumn(std::string filename, int value);
        void UpdateSamplePack(std::string filename, std::string samplePack);
        void UpdateSampleType(std::string filename, std::string type);

        // -------------------------------------------------------------------
        // Get from database
        std::string GetSampleType(std::string filename);
        int GetFavoriteColumnValueByFilename(std::string filename);
        std::string GetSamplePathByFilename(std::string filename);
        std::string GetSampleFileExtension(std::string filename);

        // -------------------------------------------------------------------
        // Check database
        bool HasSample(std::string filename);
        bool IsTrashed(std::string filename);

        // -------------------------------------------------------------------
        // Remove from database
        void RemoveSampleFromDatabase(std::string filename);

        // -------------------------------------------------------------------
        wxVector<wxVector<wxVariant>>
        LoadDatabase(wxVector<wxVector<wxVariant>> &vecSet,
                     wxTreeCtrl& favorite_tree, wxTreeItemId& favorite_item,
                     wxTreeCtrl& trash_tree, wxTreeItemId& trash_item, bool show_extension);
        wxVector<wxVector<wxVariant>>
        FilterDatabaseBySampleName(wxVector<wxVector<wxVariant>> &sampleVec,
                                   std::string sampleName);
};
