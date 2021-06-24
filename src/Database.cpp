#include <deque>
#include <exception>

#include <wx/dataview.h>
#include <wx/dvrenderers.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/stringimpl.h>
#include <wx/variant.h>

#include "Database.hpp"
#include "SettingsDialog.hpp"

Database::Database(wxInfoBar& infoBar)
    : m_InfoBar(infoBar)
{

}

Database::~Database()
{

}

void Database::CreateTableSamples()
{
    /* Create SQL statement */
    std::string samples = "CREATE TABLE IF NOT EXISTS SAMPLES("
        "FAVORITE       INT     NOT NULL,"
        "FILENAME       TEXT    NOT NULL,"
        "EXTENSION      TEXT    NOT NULL,"
        "SAMPLEPACK     TEXT    NOT NULL,"
        "TYPE           TEXT    NOT NULL,"
        "CHANNELS       INT     NOT NULL,"
        "LENGTH         INT     NOT NULL,"
        "SAMPLERATE     INT     NOT NULL,"
        "BITRATE        INT     NOT NULL,"
        "PATH           TEXT    NOT NULL,"
        "TRASHED        INT     NOT NULL,"
        "FOLDER         TEXT    NOT NULL);";

    try
    {
        if (sqlite3_open("sample.hive", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }
        else
        {
            wxLogDebug("Opening DB..");
        }

        rc = sqlite3_exec(m_Database, samples.c_str(), NULL, 0, &m_ErrMsg);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot create table samples.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Table created successfully.");
        }

        rc = sqlite3_close(m_Database);

        if (rc == SQLITE_OK)
            wxLogDebug("DB Closed..");
        else
            wxLogDebug("Error! Cannot close DB, Error code: %d, Error message: %s", rc, m_ErrMsg);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::CreateTableCollections()
{
    /* Create SQL statement */
    std::string collections = "CREATE TABLE IF NOT EXISTS COLLECTIONS(FOLDERNAME TEXT NOT NULL);";

    try
    {
        if (sqlite3_open("sample.hive", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }
        else
        {
            wxLogDebug("Opening DB..");
        }

        rc = sqlite3_exec(m_Database, collections.c_str(), NULL, 0, &m_ErrMsg);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot create table collections.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Table created successfully.");
        }

        rc = sqlite3_close(m_Database);

        if (rc == SQLITE_OK)
            wxLogDebug("DB Closed..");
        else
            wxLogDebug("Error! Cannot close DB, Error code: %d, Error message: %s", rc, m_ErrMsg);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

//Loops through a Sample array and adds them to the database
void Database::InsertIntoSamples(std::vector<Sample> samples)
{
    try
    {
        if (sqlite3_open("sample.hive", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }
        else
        {
            wxLogDebug("Opening DB..");
        }

        std::string insert = "INSERT INTO SAMPLES (FAVORITE, FILENAME, \
                              EXTENSION, SAMPLEPACK, TYPE, CHANNELS, LENGTH, \
                              SAMPLERATE, BITRATE, PATH, TRASHED, FOLDER) \
                              VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

        rc = sqlite3_prepare_v2(m_Database, insert.c_str(), insert.size(), &m_Stmt, NULL);
        
        rc = sqlite3_exec(m_Database, "BEGIN TRANSACTION", NULL, NULL, &m_ErrMsg);
        
        if (rc != SQLITE_OK)
            wxLogDebug("Cannot prepare sql statement..");
        
        Sample sample;
        
        std::string filename;
        std::string file_extension;
        std::string sample_pack;
        std::string type;
        std::string path;
        
        for(unsigned int i = 0; i < samples.size(); i++) 
        {
            sample = samples[i];

            filename = sample.GetFilename();
            file_extension = sample.GetFileExtension();
            sample_pack = sample.GetSamplePack();
            type = sample.GetType();
            path = sample.GetPath();

            std::string folder = "Favourites";

            rc = sqlite3_bind_int(m_Stmt, 1, sample.GetFavorite());
            rc = sqlite3_bind_text(m_Stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC);
            rc = sqlite3_bind_text(m_Stmt, 3, file_extension.c_str(), file_extension.size(), SQLITE_STATIC);
            rc = sqlite3_bind_text(m_Stmt, 4, sample_pack.c_str(), sample_pack.size(), SQLITE_STATIC);
            rc = sqlite3_bind_text(m_Stmt, 5, type.c_str(), type.size(), SQLITE_STATIC);
            rc = sqlite3_bind_int(m_Stmt, 6, sample.GetChannels());
            rc = sqlite3_bind_int(m_Stmt, 7, sample.GetLength());
            rc = sqlite3_bind_int(m_Stmt, 8, sample.GetSampleRate());
            rc = sqlite3_bind_int(m_Stmt, 9, sample.GetBitrate());
            rc = sqlite3_bind_text(m_Stmt, 10, path.c_str(), path.size(), SQLITE_STATIC);
            rc = sqlite3_bind_int(m_Stmt, 11, sample.GetTrashed());
            rc = sqlite3_bind_text(m_Stmt, 12, folder.c_str(), folder.size(), SQLITE_STATIC);
        
            rc = sqlite3_step(m_Stmt);
            rc = sqlite3_clear_bindings(m_Stmt);
            rc = sqlite3_reset(m_Stmt);
        }

        rc = sqlite3_exec(m_Database, "END TRANSACTION", NULL, NULL, &m_ErrMsg);

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxLogDebug("Error! Cannot insert data into table. Error code: %d: Msg: %s", rc , sqlite3_errmsg(m_Database));
        }
        else
        {
            wxLogDebug("Data inserted successfully.");
        }

        if (rc == SQLITE_BUSY)
            wxLogDebug("SQLITE_BUSY");
        if (rc == SQLITE_ABORT)
            wxLogDebug("SQLITE_ABORT");
        if (rc == SQLITE_NOMEM)
            wxLogDebug("SQLITE_NOMEM");
        if (rc == SQLITE_LOCKED)
            wxLogDebug("SQLITE_LOCKED");
        if (rc == SQLITE_IOERR)
            wxLogDebug("SQLITE_IOERR");
        if (rc == SQLITE_CORRUPT)
            wxLogDebug("SQLITE_CORRUPT");
        if (rc == SQLITE_READONLY)
            wxLogDebug("SQLITE_READONLY");
        if (rc == SQLITE_ERROR)
            wxLogDebug("SQLITE_ERROR");
        if (rc == SQLITE_PERM)
            wxLogDebug("SQLITE_PERM");
        if (rc == SQLITE_INTERNAL)
            wxLogDebug("SQLITE_INTERNAL");

        rc = sqlite3_close(m_Database);

        if (rc == SQLITE_OK)
            wxLogDebug("DB Closed..");
        else
            wxLogDebug("Error! Cannot close DB, Error code: %d, Error message: %s", rc, m_ErrMsg);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::InsertIntoCollections(const std::string& folderName)
{
    try
    {
        if (sqlite3_open("sample.hive", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }
        else
        {
            wxLogDebug("Opening DB..");
        }

        std::string insert = "INSERT INTO COLLECTIONS(FOLDERNAME) VALUES(?);";

        rc = sqlite3_prepare_v2(m_Database, insert.c_str(), insert.size(), &m_Stmt, NULL);

        // rc = sqlite3_exec(m_Database, "BEGIN TRANSACTION", NULL, NULL, &m_ErrMsg);

        if (rc != SQLITE_OK)
            wxLogDebug("Cannot prepare sql statement..");

        // Sample sample;

        // std::string filename;
        // std::string file_extension;
        // std::string sample_pack;
        // std::string type;
        // std::string path;

        // for(unsigned int i = 0; i < samples.size(); i++)
        // {
        //     sample = samples[i];

        //     filename = sample.GetFilename();
        //     file_extension = sample.GetFileExtension();
        //     sample_pack = sample.GetSamplePack();
        //     type = sample.GetType();
        //     path = sample.GetPath();

        //     std::string folder = "Favourites";

        // rc = sqlite3_bind_int(m_Stmt, 1, sample.GetFavorite());
        rc = sqlite3_bind_text(m_Stmt, 1, folderName.c_str(), folderName.size(), SQLITE_STATIC);

        rc = sqlite3_step(m_Stmt);
            // rc = sqlite3_clear_bindings(m_Stmt);
            // rc = sqlite3_reset(m_Stmt);
        // }

        // rc = sqlite3_exec(m_Database, "END TRANSACTION", NULL, NULL, &m_ErrMsg);

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxLogDebug("Error! Cannot insert data into table. Error code: %d: Msg: %s", rc , sqlite3_errmsg(m_Database));
        }
        else
        {
            wxLogDebug("Data inserted successfully.");
        }

        if (rc == SQLITE_BUSY)
            wxLogDebug("SQLITE_BUSY");
        if (rc == SQLITE_ABORT)
            wxLogDebug("SQLITE_ABORT");
        if (rc == SQLITE_NOMEM)
            wxLogDebug("SQLITE_NOMEM");
        if (rc == SQLITE_LOCKED)
            wxLogDebug("SQLITE_LOCKED");
        if (rc == SQLITE_IOERR)
            wxLogDebug("SQLITE_IOERR");
        if (rc == SQLITE_CORRUPT)
            wxLogDebug("SQLITE_CORRUPT");
        if (rc == SQLITE_READONLY)
            wxLogDebug("SQLITE_READONLY");
        if (rc == SQLITE_ERROR)
            wxLogDebug("SQLITE_ERROR");
        if (rc == SQLITE_PERM)
            wxLogDebug("SQLITE_PERM");
        if (rc == SQLITE_INTERNAL)
            wxLogDebug("SQLITE_INTERNAL");

        rc = sqlite3_close(m_Database);

        if (rc == SQLITE_OK)
            wxLogDebug("DB Closed..");
        else
            wxLogDebug("Error! Cannot close DB, Error code: %d, Error message: %s", rc, m_ErrMsg);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateFolder(const std::string& folderName)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string update = "UPDATE SAMPLES SET FOLDER = ?;";

        rc = sqlite3_prepare_v2(m_Database, update.c_str(), update.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, folderName.c_str(), folderName.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) != SQLITE_DONE)
        {
            wxLogWarning("No data inserted.");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL,
                                      "Error! Cannot insert folder into table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogInfo("Folder inserted successfully. %s", m_ErrMsg);
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateFavoriteFolder(const std::string& filename, const std::string& folderName)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string update = "UPDATE SAMPLES SET FOLDER = ? WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, update.c_str(), update.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, folderName.c_str(), folderName.size(), SQLITE_STATIC);
        rc = sqlite3_bind_text(m_Stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot update record.", "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Updated record successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateFavoriteColumn(const std::string& filename, int value)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string update = "UPDATE SAMPLES SET FAVORITE = ? WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, update.c_str(), update.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_int(m_Stmt, 1, value);
        rc = sqlite3_bind_text(m_Stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot update record.", "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Updated record successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateSamplePack(const std::string& filename, const std::string& samplePack)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string update = "UPDATE SAMPLES SET SAMPLEPACK = ? WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, update.c_str(), update.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, samplePack.c_str(), samplePack.size(), SQLITE_STATIC);
        rc = sqlite3_bind_text(m_Stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot update record.", "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Updated record successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateSampleType(const std::string& filename, const std::string& type)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string update = "UPDATE SAMPLES SET TYPE = ? WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, update.c_str(), update.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, type.c_str(), type.size(), SQLITE_STATIC);
        rc = sqlite3_bind_text(m_Stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot update record.", "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Updated record successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

std::string Database::GetSampleType(const std::string& filename)
{
    std::string type;

    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string select = "SELECT TYPE FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");

            type = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 0)));
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot get sample type column value from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogInfo("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return type;
}

int Database::GetFavoriteColumnValueByFilename(const std::string& filename)
{
    int value = 0;

    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string select = "SELECT FAVORITE FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");
            value = sqlite3_column_int(m_Stmt, 0);
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot get favorite column value from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogInfo("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return value;
}

std::string Database::GetFavoriteFolderByFilename(const std::string& filename)
{
    std::string folder;

    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string select = "SELECT FOLDER FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");

            folder = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 0)));
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot get favorite folder value from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogInfo("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return folder;
}

void Database::RemoveSampleFromDatabase(const std::string& filename)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string remove = "DELETE FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, remove.c_str(), remove.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_DONE)
        {
            wxLogDebug("Record found, Deleting..");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot delete data from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Deleted data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::RemoveFolderFromCollections(const std::string& folderName)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string remove = "DELETE FROM COLLECTIONS WHERE FOLDERNAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, remove.c_str(), remove.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, folderName.c_str(), folderName.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_DONE)
        {
            wxLogDebug("Record found, Deleting..");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot delete data from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Deleted data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

std::string Database::GetSamplePathByFilename(const std::string& filename)
{
    std::string path;

    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string select = "SELECT PATH FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");
            path = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 0)));
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot select sample path from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogInfo("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return path;
}

std::string Database::GetSampleFileExtension(const std::string& filename)
{
    std::string extension;

    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string select = "SELECT EXTENSION FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");
            extension = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 0)));
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot select sample path from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogInfo("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return extension;
}

wxVector<wxVector<wxVariant>>
Database::LoadDatabase(wxVector<wxVector<wxVariant>>& vecSet,
                       // wxTreeCtrl& favorite_tree, wxTreeItemId& favorite_item,
                       wxDataViewTreeCtrl& favorite_tree, wxDataViewItem& favorite_item,
                       wxTreeCtrl& trash_tree, wxTreeItemId& trash_item,
                       bool show_extension)
{
    try
    {
        if (sqlite3_open("sample.hive", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string load = "SELECT FAVORITE, FILENAME, EXTENSION, SAMPLEPACK, \
                            TYPE, CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH, \
                            TRASHED, FOLDER FROM SAMPLES;";

        rc = sqlite3_prepare_v2(m_Database, load.c_str(), load.size(), &m_Stmt, NULL);

        if (rc == SQLITE_OK)
        {
            int row = 0;

            while (SQLITE_ROW == sqlite3_step(m_Stmt))
            {
                int favorite = sqlite3_column_int(m_Stmt, 0);
                wxString filename = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 1)));
                wxString file_extension = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 2)));
                wxString sample_pack = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 3)));
                wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(m_Stmt, 4)));
                int channels = sqlite3_column_int(m_Stmt, 5);
                int length = sqlite3_column_int(m_Stmt, 6);
                int sample_rate = sqlite3_column_int(m_Stmt, 7);
                int bitrate = sqlite3_column_int(m_Stmt, 8);
                wxString path = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 9)));
                int trashed = sqlite3_column_int(m_Stmt, 10);
                wxString folder_name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 11)));

                wxVector<wxVariant> vec;

                if (trashed == 1)
                {
                    if (show_extension)
                        trash_tree.AppendItem(trash_item, wxString::Format("%s.%s", filename, file_extension));
                    else
                        trash_tree.AppendItem(trash_item, filename);
                }
                else
                {
                    wxVariant icon_c, icon_gs;
                    icon_c << wxDataViewIconText(wxEmptyString, wxIcon("../assets/icons/icon-hive_16x16.png"));
                    icon_gs << wxDataViewIconText(wxEmptyString, wxIcon("../assets/icons/icon-hive_16x16-gs.png"));

                    if (favorite == 1)
                    {
                        // vec.push_back(true);
                        vec.push_back(icon_c);

                        wxLogDebug("Loading collection items..");

                        std::deque<wxDataViewItem> nodes;
                        nodes.push_back(favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), 0));

                        wxDataViewItem current_item, found_item;

                        int row = 0;
                        int folder_count = favorite_tree.GetChildCount(wxDataViewItem(wxNullPtr));

                        while(!nodes.empty())
                        {
                            current_item = nodes.front();
                            nodes.pop_front();

                            if (favorite_tree.GetItemText(current_item) == folder_name)
                            {
                                found_item = current_item;
                                wxLogDebug("Loading, folder name: %s", folder_name);
                                break;
                            }

                            wxDataViewItem child = favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), 0);

                            while (row < (folder_count - 1))
                            {
                                row ++;

                                child = favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), row);
                                nodes.push_back(child);
                            }
                        }

                        nodes.clear();

                        if (found_item.IsOk())
                        {
                            // wxLogDebug("Another folder by the name %s already exist. Please try with a different name.",
                                       // folder_name);
                            if (show_extension)
                                favorite_tree.AppendItem(found_item,
                                                         wxString::Format("%s.%s", filename, file_extension));
                            else
                                favorite_tree.AppendItem(found_item, filename);
                        }
                        // else
                        // {
                        //     favorite_tree.AppendItem(wxDataViewItem(wxNullPtr), folder_name);
                        // }

                    }
                    else
                        // vec.push_back(false);
                        vec.push_back(icon_gs);

                    if (show_extension)
                    {
                        vec.push_back(path.AfterLast('/'));
                    }
                    else
                    {
                        vec.push_back(path.AfterLast('/').BeforeLast('.'));
                    }

                    vec.push_back(sample_pack);
                    vec.push_back(sample_type);
                    vec.push_back(wxString::Format("%d", channels));
                    vec.push_back(wxString::Format("%d", length));
                    vec.push_back(wxString::Format("%d", sample_rate));
                    vec.push_back(wxString::Format("%d", bitrate));

                    vecSet.push_back(vec);
                }

                row++;
            }
        }
        else
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot load data from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }

        rc = sqlite3_finalize(m_Stmt);

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return vecSet;
}

wxVector<wxVector<wxVariant>>
Database::FilterDatabaseBySampleName(wxVector<wxVector<wxVariant>>& sampleVec,
                                     const std::string& sampleName, bool show_extension)
{
    try
    {
        if (sqlite3_open("sample.hive", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string filter = "SELECT FAVORITE, FILENAME, SAMPLEPACK, TYPE, \
                              CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH \
                              FROM SAMPLES WHERE FILENAME LIKE '%' || ? || '%';";

        rc = sqlite3_prepare_v2(m_Database, filter.c_str(), filter.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, sampleName.c_str(), sampleName.size(), SQLITE_STATIC);

        if (rc == SQLITE_OK)
        {
            int row = 0;

            while (SQLITE_ROW == sqlite3_step(m_Stmt))
            {
                wxLogInfo("Record found, fetching..");
                int favorite = sqlite3_column_int(m_Stmt, 0);
                wxString filename = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 1))));
                wxString sample_pack = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 2))));
                wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(m_Stmt, 3)));
                int channels = sqlite3_column_int(m_Stmt, 4);
                int length = sqlite3_column_int(m_Stmt, 5);
                int sample_rate = sqlite3_column_int(m_Stmt, 6);
                int bitrate = sqlite3_column_int(m_Stmt, 7);
                wxString path = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 8))));

                wxVector<wxVariant> vec;

                wxVariant icon_c, icon_gs;
                icon_c << wxDataViewIconText(wxEmptyString, wxIcon("../assets/icons/icon-hive_16x16.png"));
                icon_gs << wxDataViewIconText(wxEmptyString, wxIcon("../assets/icons/icon-hive_16x16-gs.png"));

                if (favorite == 1)
                    vec.push_back(icon_c);
                else
                    vec.push_back(icon_gs);

                // if (favorite == 1)
                //     vec.push_back(true);
                // else
                //     vec.push_back(false);

                // vec.push_back(filename);

                if (show_extension)
                {
                    vec.push_back(path.AfterLast('/'));
                }
                else
                {
                    vec.push_back(path.AfterLast('/').BeforeLast('.'));
                }

                vec.push_back(sample_pack);
                vec.push_back(sample_type);
                vec.push_back(wxString::Format("%d", channels));
                vec.push_back(wxString::Format("%d", length));
                vec.push_back(wxString::Format("%d", sample_rate));
                vec.push_back(wxString::Format("%d", bitrate));

                sampleVec.push_back(vec);

                row++;
            }
        }
        else
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot filter data from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }

        rc = sqlite3_finalize(m_Stmt);

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return sampleVec;
}

wxVector<wxVector<wxVariant>>
Database::FilterDatabaseByFolderName(wxVector<wxVector<wxVariant>>& sampleVec,
                                     const std::string& folderName, bool show_extension)
{
    try
    {
        if (sqlite3_open("sample.hive", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string filter = "SELECT FAVORITE, FILENAME, SAMPLEPACK, TYPE, \
                              CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH \
                              FROM SAMPLES WHERE FOLDER = ? AND FAVORITE = 1;";

        rc = sqlite3_prepare_v2(m_Database, filter.c_str(), filter.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, folderName.c_str(), folderName.size(), SQLITE_STATIC);

        if (rc == SQLITE_OK)
        {
            int row = 0;

            while (SQLITE_ROW == sqlite3_step(m_Stmt))
            {
                wxLogInfo("Record found, fetching..");
                int favorite = sqlite3_column_int(m_Stmt, 0);
                wxString filename = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 1))));
                wxString sample_pack = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 2))));
                wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(m_Stmt, 3)));
                int channels = sqlite3_column_int(m_Stmt, 4);
                int length = sqlite3_column_int(m_Stmt, 5);
                int sample_rate = sqlite3_column_int(m_Stmt, 6);
                int bitrate = sqlite3_column_int(m_Stmt, 7);
                wxString path = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 8))));

                wxVector<wxVariant> vec;

                wxVariant icon_c, icon_gs;
                icon_c << wxDataViewIconText(wxEmptyString, wxIcon("../assets/icons/icon-hive_16x16.png"));
                icon_gs << wxDataViewIconText(wxEmptyString, wxIcon("../assets/icons/icon-hive_16x16-gs.png"));

                if (favorite == 1)
                    vec.push_back(icon_c);
                else
                    vec.push_back(icon_gs);

                // if (favorite == 1)
                //     vec.push_back(true);
                // else
                //     vec.push_back(false);

                // vec.push_back(filename);

                if (show_extension)
                {
                    vec.push_back(path.AfterLast('/'));
                }
                else
                {
                    vec.push_back(path.AfterLast('/').BeforeLast('.'));
                }

                vec.push_back(sample_pack);
                vec.push_back(sample_type);
                vec.push_back(wxString::Format("%d", channels));
                vec.push_back(wxString::Format("%d", length));
                vec.push_back(wxString::Format("%d", sample_rate));
                vec.push_back(wxString::Format("%d", bitrate));

                sampleVec.push_back(vec);

                row++;
            }
        }
        else
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot filter data from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }

        rc = sqlite3_finalize(m_Stmt);

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return sampleVec;
}

void Database::LoadCollectionFolder(wxDataViewTreeCtrl& treeCtrl)
{
    try
    {
        if (sqlite3_open("sample.hive", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string select = "SELECT FOLDERNAME FROM COLLECTIONS;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        if (rc == SQLITE_OK)
        {
            while (SQLITE_ROW == sqlite3_step(m_Stmt))
            {
                wxLogInfo("Record found, fetching..");
                wxString folder = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 0))));

                treeCtrl.AppendContainer(wxDataViewItem(wxNullPtr), folder);
            }
        }
        else
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot load foldername from collection table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }

        rc = sqlite3_finalize(m_Stmt);

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

//Compares the input array with the database and removes duplicates.
wxArrayString Database::CheckDuplicates(const wxArrayString& files)
{
    wxArrayString sorted_files;

    std::string filename;
    std::string sample;

    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string select = "SELECT * FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);
        
        for(unsigned int i = 0; i < files.size(); i++) 
        {
            filename = files[i].AfterLast('/').BeforeLast('.').ToStdString();

            rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);
            
            if (sqlite3_step(m_Stmt) != SQLITE_ROW)
                sorted_files.push_back(files[i]);
            else
                wxLogDebug("Already added: %s. Skipping..", files[i]);

            rc = sqlite3_clear_bindings(m_Stmt);
            rc = sqlite3_reset(m_Stmt);
        }
        
        sqlite3_finalize(m_Stmt);
        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return sorted_files; 
}

bool Database::IsTrashed(const std::string& filename)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string select = "SELECT TRASHED FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");

            if (sqlite3_column_int(m_Stmt, 0) == 1)
                return true;
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot select sample path from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogInfo("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return false;
}

void Database::UpdateTrashColumn(const std::string& filename, int value)
{
    try
    {
        rc = sqlite3_open("sample.hive", &m_Database);

        std::string update = "UPDATE SAMPLES SET TRASHED = ? WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, update.c_str(), update.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_int(m_Stmt, 1, value);
        rc = sqlite3_bind_text(m_Stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot update record.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Updated record successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}
