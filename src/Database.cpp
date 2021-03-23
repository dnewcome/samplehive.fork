#include <exception>

#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/string.h>

#include "Database.hpp"
#include "SettingsDialog.hpp"

Database::Database(wxInfoBar& infoBar)
    : m_InfoBar(infoBar)
{
    /* Create SQL statement */
    std::string sample = "CREATE TABLE IF NOT EXISTS SAMPLES("
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
        "TRASHED        INT     NOT NULL);";

    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);
        rc = sqlite3_exec(m_Database, sample.c_str(), NULL, 0, &m_ErrMsg);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot create table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Table created successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

Database::~Database()
{

}

void Database::InsertSample(int favorite, std::string filename,
                            std::string fileExtension, std::string samplePack,
                            std::string type, int channels, int length,
                            int sampleRate, int bitrate, std::string path,
                            int trashed)
{
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

        std::string insert = "INSERT INTO SAMPLES (FAVORITE, FILENAME, \
                              EXTENSION, SAMPLEPACK, TYPE, CHANNELS, LENGTH, \
                              SAMPLERATE, BITRATE, PATH, TRASHED) \
                              VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

        rc = sqlite3_prepare_v2(m_Database, insert.c_str(), insert.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_int(m_Stmt, 1, favorite);
        rc = sqlite3_bind_text(m_Stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC);
        rc = sqlite3_bind_text(m_Stmt, 3, fileExtension.c_str(), fileExtension.size(), SQLITE_STATIC);
        rc = sqlite3_bind_text(m_Stmt, 4, samplePack.c_str(), samplePack.size(), SQLITE_STATIC);
        rc = sqlite3_bind_text(m_Stmt, 5, type.c_str(), type.size(), SQLITE_STATIC);
        rc = sqlite3_bind_int(m_Stmt, 6, channels);
        rc = sqlite3_bind_int(m_Stmt, 7, length);
        rc = sqlite3_bind_int(m_Stmt, 8, sampleRate);
        rc = sqlite3_bind_int(m_Stmt, 9, bitrate);
        rc = sqlite3_bind_text(m_Stmt, 10, path.c_str(), path.size(), SQLITE_STATIC);
        rc = sqlite3_bind_int(m_Stmt, 11, trashed);

        if (sqlite3_step(m_Stmt) != SQLITE_DONE)
        {
            wxLogWarning("No data inserted.");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog* msgDialog;
            msgDialog = new wxMessageDialog(NULL,
                                      "Error! Cannot insert data into table.",
                                      "Error", wxOK | wxICON_ERROR);
            // msgDialog->ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogInfo("Data inserted successfully. %s", m_ErrMsg);
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateFolder(std::string folderName)
{
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot insert folder into table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

void Database::UpdateFavoriteFolderDatabase(std::string filename, std::string folderName)
{
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

        std::string update = "UPDATE SAMPLES SET FAVORITEFOLDER = ? WHERE FILENAME = ?;";

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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot update record.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

void Database::UpdateFavoriteColumn(std::string filename, int value)
{
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot update record.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

void Database::UpdateSampleType(std::string filename, std::string type)
{
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot update record.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

std::string Database::GetSampleType(std::string filename)
{
    std::string type;

    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

        std::string select = "SELECT TYPE FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");
            type = std::string(reinterpret_cast< const char* >(sqlite3_column_text(m_Stmt, 0)));
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot get favorite column value from table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

int Database::GetFavoriteColumnValueByFilename(std::string filename)
{
    int value = 0;

    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot get favorite column value from table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

void Database::RemoveSampleFromDatabase(std::string filename)
{
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

        std::string remove = "DELETE * FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, remove.c_str(), remove.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_DONE)
        {
            wxLogDebug("Record found, Deleting..");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot delete data from table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

std::string Database::GetSamplePathByFilename(std::string filename)
{
    std::string path;

    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

        std::string select = "SELECT PATH FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");
            path = std::string(reinterpret_cast< const char* >(sqlite3_column_text(m_Stmt, 0)));
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot select sample path from table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

std::string Database::GetSampleFileExtension(std::string filename)
{
    std::string extension;

    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

        std::string select = "SELECT EXTENSION FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");
            extension = std::string(reinterpret_cast< const char* >(sqlite3_column_text(m_Stmt, 0)));
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot select sample path from table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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
                       wxTreeCtrl& favorite_tree, wxTreeItemId& favorite_item,
                       wxTreeCtrl& trash_tree, wxTreeItemId& trash_item,
                       bool show_extension)
{
    try
    {
        if (sqlite3_open("Samples.db", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string load = "SELECT FAVORITE, FILENAME, EXTENSION, SAMPLEPACK, \
                            TYPE, CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH, \
                            TRASHED FROM SAMPLES;";

        rc = sqlite3_prepare_v2(m_Database, load.c_str(), load.size(), &m_Stmt, NULL);

        if (rc == SQLITE_OK)
        {
            int row = 0;

            while (SQLITE_ROW == sqlite3_step(m_Stmt))
            {
                wxLogDebug("Record found, fetching..");

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

                wxVector<wxVariant> vec;

                if (trashed == 1)
                    trash_tree.AppendItem(trash_item, filename);
                else
                {
                    if (favorite == 1)
                    {
                        vec.push_back(true);

                        favorite_tree.AppendItem(favorite_item, filename);
                    }
                    else
                        vec.push_back(false);

                    if (show_extension)
                    {
                        vec.push_back(path.AfterLast('/'));
                        wxLogDebug("With extension..");
                    }
                    else
                    {
                        vec.push_back(path.AfterLast('/').BeforeLast('.'));
                        wxLogDebug("Without extension..");
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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot load data from table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

wxVector<wxVector<wxVariant>> Database::FilterDatabaseBySampleName(wxVector<wxVector<wxVariant>>& sampleVec, std::string sampleName)
{
    try
    {
        if (sqlite3_open("Samples.db", &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string filter = "SELECT FAVORITE, FILENAME, SAMPLEPACK, TYPE, \
                              CHANNELS, LENGTH, SAMPLERATE, BITRATE     \
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
                wxString filename = wxString(std::string(reinterpret_cast< const char* >(sqlite3_column_text(m_Stmt, 1))));
                wxString sample_pack = wxString(std::string(reinterpret_cast< const char* >(sqlite3_column_text(m_Stmt, 2))));
                wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(m_Stmt, 3)));
                int channels = sqlite3_column_int(m_Stmt, 4);
                int length = sqlite3_column_int(m_Stmt, 5);
                int sample_rate = sqlite3_column_int(m_Stmt, 6);
                int bitrate = sqlite3_column_int(m_Stmt, 7);

                wxVector<wxVariant> vec;

                if (favorite == 1)
                    vec.push_back(true);
                else
                    vec.push_back(false);

                vec.push_back(filename);
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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot filter data from table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

bool Database::HasSample(std::string filename)
{
    std::string sample;
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

        std::string select = "SELECT * FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");
            sample = std::string(reinterpret_cast< const char* >(sqlite3_column_text(m_Stmt, 0)));
            return true;
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot find data in table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

bool Database::IsTrashed(std::string filename)
{
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot select sample path from table.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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

void Database::UpdateTrashColumn(std::string filename, int value)
{
    try
    {
        rc = sqlite3_open("Samples.db", &m_Database);

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
            wxMessageDialog* msgDialog = new wxMessageDialog(NULL, "Error! Cannot update record.", "Error", wxOK | wxICON_ERROR);
            msgDialog->ShowModal();
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
