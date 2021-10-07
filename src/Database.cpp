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

#include "Database.hpp"

#include <deque>
#include <exception>

#include <wx/dataview.h>
#include <wx/dvrenderers.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/stringimpl.h>
#include <wx/variant.h>

Database::Database(wxInfoBar& infoBar)
    : m_InfoBar(infoBar)
{

}

Database::~Database()
{

}

void Database::CreateTableSamples(const std::string& dbPath)
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
        "HIVE           TEXT    NOT NULL);";

    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
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
            wxLogDebug("Samples table created successfully.");
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

void Database::CreateTableHives(const std::string& dbPath)
{
    /* Create SQL statement */
    std::string hives = "CREATE TABLE IF NOT EXISTS HIVES(HIVE TEXT NOT NULL);";

    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }
        else
        {
            wxLogDebug("Opening DB..");
        }

        rc = sqlite3_exec(m_Database, hives.c_str(), NULL, 0, &m_ErrMsg);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot create table hives.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Hives table created successfully.");
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
void Database::InsertIntoSamples(const std::string& dbPath, std::vector<Sample> samples)
{
    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
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
                              SAMPLERATE, BITRATE, PATH, TRASHED, HIVE) \
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

            std::string hive = "Favorites";

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
            rc = sqlite3_bind_text(m_Stmt, 12, hive.c_str(), hive.size(), SQLITE_STATIC);
        
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

void Database::InsertIntoHives(const std::string& dbPath, const std::string& hiveName)
{
    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }
        else
        {
            wxLogDebug("Opening DB..");
        }

        std::string insert = "INSERT INTO HIVES(HIVE) VALUES(?);";

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

        //     std::string hive = "Favourites";

        // rc = sqlite3_bind_int(m_Stmt, 1, sample.GetFavorite());
        rc = sqlite3_bind_text(m_Stmt, 1, hiveName.c_str(), hiveName.size(), SQLITE_STATIC);

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

void Database::UpdateHive(const std::string& dbPath, const std::string& hiveOldName, const std::string& hiveNewName)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string update = "UPDATE HIVES SET HIVE = ? WHERE HIVE = ?;";

        rc = sqlite3_prepare_v2(m_Database, update.c_str(), update.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, hiveNewName.c_str(), hiveNewName.size(), SQLITE_STATIC);
        rc = sqlite3_bind_text(m_Stmt, 2, hiveOldName.c_str(), hiveOldName.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) != SQLITE_DONE)
        {
            wxLogWarning("No data inserted.");
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL,
                                      "Error! Cannot update hive name.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Hive updated successfully. %s", m_ErrMsg);
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateHiveName(const std::string& dbPath, const std::string& filename, const std::string& hiveName)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string update = "UPDATE SAMPLES SET HIVE = ? WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, update.c_str(), update.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, hiveName.c_str(), hiveName.size(), SQLITE_STATIC);
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

void Database::UpdateFavoriteColumn(const std::string& dbPath, const std::string& filename, int value)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

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

void Database::UpdateSamplePack(const std::string& dbPath, const std::string& filename, const std::string& samplePack)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

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

void Database::UpdateSampleType(const std::string& dbPath, const std::string& filename, const std::string& type)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

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

std::string Database::GetSampleType(const std::string& dbPath, const std::string& filename)
{
    std::string type;

    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string select = "SELECT TYPE FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");

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
            wxLogDebug("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return type;
}

int Database::GetFavoriteColumnValueByFilename(const std::string& dbPath, const std::string& filename)
{
    int value = 0;

    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string select = "SELECT FAVORITE FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");
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
            wxLogDebug("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return value;
}

std::string Database::GetHiveByFilename(const std::string& dbPath, const std::string& filename)
{
    std::string hive;

    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string select = "SELECT HIVE FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");

            hive = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 0)));
        }

        rc = sqlite3_finalize(m_Stmt);

        if (rc != SQLITE_OK)
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot get hive value from table.",
                                      "Error", wxOK | wxICON_ERROR);
            msgDialog.ShowModal();
            sqlite3_free(m_ErrMsg);
        }
        else
        {
            wxLogDebug("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return hive;
}

void Database::RemoveSampleFromDatabase(const std::string& dbPath, const std::string& filename)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

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

void Database::RemoveHiveFromDatabase(const std::string& dbPath, const std::string& hiveName)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string remove = "DELETE FROM HIVES WHERE HIVE = ?;";

        rc = sqlite3_prepare_v2(m_Database, remove.c_str(), remove.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, hiveName.c_str(), hiveName.size(), SQLITE_STATIC);

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

std::string Database::GetSamplePathByFilename(const std::string& dbPath, const std::string& filename)
{
    std::string path;

    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string select = "SELECT PATH FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");
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
            wxLogDebug("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return path;
}

std::string Database::GetSampleFileExtension(const std::string& dbPath, const std::string& filename)
{
    std::string extension;

    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string select = "SELECT EXTENSION FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");
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
            wxLogDebug("Selected data from table successfully.");
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
Database::LoadSamplesDatabase(const std::string& dbPath, wxVector<wxVector<wxVariant>>& vecSet,
                              // wxTreeCtrl& favorite_tree, wxTreeItemId& favorite_item,
                              wxDataViewTreeCtrl& favorite_tree, wxDataViewItem& favorite_item,
                              wxTreeCtrl& trash_tree, wxTreeItemId& trash_item, bool show_extension,
                              const std::string& icon_star_filled, const std::string& icon_star_empty)
{
    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string load = "SELECT FAVORITE, FILENAME, EXTENSION, SAMPLEPACK, \
                            TYPE, CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH, \
                            TRASHED, HIVE FROM SAMPLES;";

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
                wxString hive_name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 11)));

                wxLongLong llLength = length;
                int total_min = static_cast<int>((llLength / 60000).GetValue());
                int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

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
                    wxVariant icon_filled, icon_empty;
                    icon_filled = wxVariant(wxBitmap(icon_star_filled));
                    icon_empty = wxVariant(wxBitmap(icon_star_empty));

                    if (favorite == 1)
                    {
                        // vec.push_back(true);
                        vec.push_back(icon_filled);

                        wxLogDebug("Loading hives..");

                        std::deque<wxDataViewItem> nodes;
                        nodes.push_back(favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), 0));

                        wxDataViewItem current_item, found_item;

                        int row = 0;
                        int hive_count = favorite_tree.GetChildCount(wxDataViewItem(wxNullPtr));

                        while(!nodes.empty())
                        {
                            current_item = nodes.front();
                            nodes.pop_front();

                            if (favorite_tree.GetItemText(current_item) == hive_name)
                            {
                                found_item = current_item;
                                wxLogDebug("Loading, hive name: %s", hive_name);
                                break;
                            }

                            wxDataViewItem child = favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), 0);

                            while (row < (hive_count - 1))
                            {
                                row ++;

                                child = favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), row);
                                nodes.push_back(child);
                            }
                        }

                        nodes.clear();

                        if (found_item.IsOk())
                        {
                            // wxLogDebug("Another hive by the name %s already exist. Please try with a different name.",
                                       // hive_name);
                            if (show_extension)
                                favorite_tree.AppendItem(found_item,
                                                         wxString::Format("%s.%s", filename, file_extension));
                            else
                                favorite_tree.AppendItem(found_item, filename);
                        }
                        // else
                        // {
                        //     favorite_tree.AppendItem(wxDataViewItem(wxNullPtr), hive_name);
                        // }

                    }
                    else
                        // vec.push_back(false);
                        vec.push_back(icon_empty);

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
                    vec.push_back(wxString::Format("%2i:%02i", total_min, total_sec));
                    vec.push_back(wxString::Format("%d", sample_rate));
                    vec.push_back(wxString::Format("%d", bitrate));
                    vec.push_back(path);

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
Database::FilterDatabaseBySampleName(const std::string& dbPath, wxVector<wxVector<wxVariant>>& sampleVec,
                                     const std::string& sampleName, bool show_extension,
                                     const std::string& icon_star_filled, const std::string& icon_star_empty)
{
    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
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
                wxLogDebug("Record found, fetching..");
                int favorite = sqlite3_column_int(m_Stmt, 0);
                wxString filename = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 1))));
                wxString sample_pack = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 2))));
                wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(m_Stmt, 3)));
                int channels = sqlite3_column_int(m_Stmt, 4);
                int length = sqlite3_column_int(m_Stmt, 5);
                int sample_rate = sqlite3_column_int(m_Stmt, 6);
                int bitrate = sqlite3_column_int(m_Stmt, 7);
                wxString path = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 8))));

                wxLongLong llLength = length;
                int total_min = static_cast<int>((llLength / 60000).GetValue());
                int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

                wxVector<wxVariant> vec;

                wxVariant icon_filled, icon_empty;
                icon_filled = wxVariant(wxBitmap(icon_star_filled));
                icon_empty = wxVariant(wxBitmap(icon_star_empty));

                if (favorite == 1)
                    vec.push_back(icon_filled);
                else
                    vec.push_back(icon_empty);

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
                vec.push_back(wxString::Format("%2i:%02i", total_min, total_sec));
                vec.push_back(wxString::Format("%d", sample_rate));
                vec.push_back(wxString::Format("%d", bitrate));
                vec.push_back(path);

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
Database::FilterDatabaseByHiveName(const std::string& dbPath, wxVector<wxVector<wxVariant>>& sampleVec,
                                   const std::string& hiveName, bool show_extension,
                                   const std::string& icon_star_filled, const std::string& icon_star_empty)
{
    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string filter = "SELECT FAVORITE, FILENAME, SAMPLEPACK, TYPE, \
                              CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH \
                              FROM SAMPLES WHERE HIVE = ? AND FAVORITE = 1;";

        rc = sqlite3_prepare_v2(m_Database, filter.c_str(), filter.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, hiveName.c_str(), hiveName.size(), SQLITE_STATIC);

        if (rc == SQLITE_OK)
        {
            int row = 0;

            while (SQLITE_ROW == sqlite3_step(m_Stmt))
            {
                wxLogDebug("Record found, fetching..");
                int favorite = sqlite3_column_int(m_Stmt, 0);
                wxString filename = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 1))));
                wxString sample_pack = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 2))));
                wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(m_Stmt, 3)));
                int channels = sqlite3_column_int(m_Stmt, 4);
                int length = sqlite3_column_int(m_Stmt, 5);
                int sample_rate = sqlite3_column_int(m_Stmt, 6);
                int bitrate = sqlite3_column_int(m_Stmt, 7);
                wxString path = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 8))));

                wxLongLong llLength = length;
                int total_min = static_cast<int>((llLength / 60000).GetValue());
                int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

                wxVector<wxVariant> vec;

                wxVariant icon_filled, icon_empty;
                icon_filled = wxVariant(wxBitmap(icon_star_filled));
                icon_empty = wxVariant(wxBitmap(icon_star_empty));

                if (favorite == 1)
                    vec.push_back(icon_filled);
                else
                    vec.push_back(icon_empty);

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
                vec.push_back(wxString::Format("%2i:%02i", total_min, total_sec));
                vec.push_back(wxString::Format("%d", sample_rate));
                vec.push_back(wxString::Format("%d", bitrate));
                vec.push_back(path);

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

void Database::LoadHivesDatabase(const std::string& dbPath, wxDataViewTreeCtrl& treeCtrl)
{
    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string select = "SELECT HIVE FROM HIVES;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        if (rc == SQLITE_OK)
        {
            while (SQLITE_ROW == sqlite3_step(m_Stmt))
            {
                wxLogDebug("Record found, fetching..");
                wxString hive = wxString(std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 0))));

                treeCtrl.AppendContainer(wxDataViewItem(wxNullPtr), hive);
            }
        }
        else
        {
            wxMessageDialog msgDialog(NULL, "Error! Cannot load hive from hives table.",
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
wxArrayString Database::CheckDuplicates(const std::string& dbPath, const wxArrayString& files)
{
    wxArrayString sorted_files;

    std::string filename;
    std::string sample;

    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

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

bool Database::IsTrashed(const std::string& dbPath, const std::string& filename)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

        std::string select = "SELECT TRASHED FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, select.c_str(), select.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (sqlite3_step(m_Stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");

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
            wxLogDebug("Selected data from table successfully.");
        }

        sqlite3_close(m_Database);
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return false;
}

void Database::UpdateTrashColumn(const std::string& dbPath, const std::string& filename, int value)
{
    try
    {
        rc = sqlite3_open(dbPath.c_str(), &m_Database);

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

wxVector<wxVector<wxVariant>>
Database::RestoreFromTrashByFilename(const std::string& dbPath, const std::string& filename,
                                     wxVector<wxVector<wxVariant>>& vecSet, bool show_extension,
                                     const std::string& icon_star_filled, const std::string& icon_star_empty)
{
    try
    {
        if (sqlite3_open(dbPath.c_str(), &m_Database) != SQLITE_OK)
        {
            wxLogDebug("Error opening DB");
            throw sqlite3_errmsg(m_Database);
        }

        std::string restore = "SELECT FAVORITE, FILENAME, EXTENSION, SAMPLEPACK, \
                               TYPE, CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH, \
                               TRASHED, HIVE FROM SAMPLES WHERE FILENAME = ?;";

        rc = sqlite3_prepare_v2(m_Database, restore.c_str(), restore.size(), &m_Stmt, NULL);

        rc = sqlite3_bind_text(m_Stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC);

        if (rc == SQLITE_OK)
        {
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
                wxString hive_name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_Stmt, 11)));

                wxLongLong llLength = length;
                int total_min = static_cast<int>((llLength / 60000).GetValue());
                int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

                wxVector<wxVariant> vec;

                wxVariant icon_filled, icon_empty;
                icon_filled = wxVariant(wxBitmap(icon_star_filled));
                icon_empty = wxVariant(wxBitmap(icon_star_empty));

                if (trashed == 0)
                {
                    if (favorite == 1)
                        vec.push_back(icon_filled);
                    else
                        vec.push_back(icon_empty);

                    if (show_extension)
                        vec.push_back(path.AfterLast('/'));
                    else
                        vec.push_back(path.AfterLast('/').BeforeLast('.'));

                    vec.push_back(sample_pack);
                    vec.push_back(sample_type);
                    vec.push_back(wxString::Format("%d", channels));
                    vec.push_back(wxString::Format("%2i:%02i", total_min, total_sec));
                    vec.push_back(wxString::Format("%d", sample_rate));
                    vec.push_back(wxString::Format("%d", bitrate));
                    vec.push_back(path);

                    vecSet.push_back(vec);
                }
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
