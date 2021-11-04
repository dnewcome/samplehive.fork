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

#include "Database/Database.hpp"

#include <deque>
#include <exception>
#include <sstream>

#include <wx/dataview.h>
#include <wx/dvrenderers.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/stringimpl.h>
#include <wx/variant.h>

void throw_on_sqlite3_error(int rc)
{
    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errstr(rc));
    }
}

void debug_log_on_error(int rc)
{
    if (rc != SQLITE_OK)
    {
        sqlite3_errstr(rc);
    }
}

void show_modal_dialog_and_log(const std::string &message, const std::string &title, const std::string &error_msg)
{
    std::stringstream ss;
    ss << message << error_msg;

    const auto msg = ss.str();

    wxLogDebug(msg.c_str());

    wxMessageDialog msgDialog(NULL, _(msg), _(title), wxOK | wxICON_ERROR);
    msgDialog.ShowModal();
}

class Sqlite3Statement
{
    public:
        Sqlite3Statement(sqlite3 *database, const std::string &query)
        {
            throw_on_sqlite3_error(sqlite3_prepare_v2(database, query.c_str(), query.size(), &stmt, NULL));
        }
         ~Sqlite3Statement()
        {
            throw_on_sqlite3_error(sqlite3_finalize(stmt));
        }

        sqlite3_stmt *stmt = nullptr;
};

Database::Database(const std::string &dbPath)
{
    OpenDatabase(dbPath);
}

Database::~Database()
{
    CloseDatabase();
}

void Database::CreateTableSamples()
{
    /* Create SQL statement */
    const auto samples = "CREATE TABLE IF NOT EXISTS SAMPLES("
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
        throw_on_sqlite3_error(sqlite3_exec(m_Database, samples, NULL, 0, &m_ErrMsg));
        wxLogDebug(_("Samples table created successfully."));
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot create table samples: ", "Error", e.what());
    }
}

void Database::CreateTableHives()
{
    /* Create SQL statement */
    const auto hives = "CREATE TABLE IF NOT EXISTS HIVES(HIVE TEXT NOT NULL);";

    try
    {
        throw_on_sqlite3_error(sqlite3_exec(m_Database, hives, NULL, 0, &m_ErrMsg));
        wxLogDebug("Hives table created successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot create hives table", "Error", e.what());
    }
}

//Loops through a Sample array and adds them to the database
void Database::InsertIntoSamples(const std::vector<Sample> &samples)
{
    try
    {
        const auto sql = "INSERT INTO SAMPLES (FAVORITE, FILENAME, \
                          EXTENSION, SAMPLEPACK, TYPE, CHANNELS, LENGTH, \
                          SAMPLERATE, BITRATE, PATH, TRASHED, HIVE) \
                          VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_exec(m_Database, "BEGIN TRANSACTION", NULL, NULL, &m_ErrMsg));

        Sample sample;

        std::string filename;
        std::string file_extension;
        std::string sample_pack;
        std::string type;
        std::string path;

        for (unsigned int i = 0; i < samples.size(); i++)
        {
            sample = samples[i];

            filename = sample.GetFilename();
            file_extension = sample.GetFileExtension();
            sample_pack = sample.GetSamplePack();
            type = sample.GetType();
            path = sample.GetPath();

            std::string hive = "Favorites";

            throw_on_sqlite3_error(sqlite3_bind_int(statement.stmt, 1, sample.GetFavorite()));
            throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC));
            throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 3, file_extension.c_str(), file_extension.size(), SQLITE_STATIC));
            throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 4, sample_pack.c_str(), sample_pack.size(), SQLITE_STATIC));
            throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 5, type.c_str(), type.size(), SQLITE_STATIC));
            throw_on_sqlite3_error(sqlite3_bind_int(statement.stmt, 6, sample.GetChannels()));
            throw_on_sqlite3_error(sqlite3_bind_int(statement.stmt, 7, sample.GetLength()));
            throw_on_sqlite3_error(sqlite3_bind_int(statement.stmt, 8, sample.GetSampleRate()));
            throw_on_sqlite3_error(sqlite3_bind_int(statement.stmt, 9, sample.GetBitrate()));
            throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 10, path.c_str(), path.size(), SQLITE_STATIC));
            throw_on_sqlite3_error(sqlite3_bind_int(statement.stmt, 11, sample.GetTrashed()));
            throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 12, hive.c_str(), hive.size(), SQLITE_STATIC));

            sqlite3_step(statement.stmt);
            throw_on_sqlite3_error(sqlite3_clear_bindings(statement.stmt));
            throw_on_sqlite3_error(sqlite3_reset(statement.stmt));
        }

        throw_on_sqlite3_error(sqlite3_exec(m_Database, "END TRANSACTION", NULL, NULL, &m_ErrMsg));

        wxLogDebug("Data inserted successfully.");
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::InsertIntoHives(const std::string &hiveName)
{
    try
    {
        const auto sql = "INSERT INTO HIVES(HIVE) VALUES(?);";

        Sqlite3Statement statement(m_Database, sql);

        // rc = sqlite3_exec(m_Database, "BEGIN TRANSACTION", NULL, NULL, &m_ErrMsg);

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

        // rc = sqlite3_bind_int(statement.stmt, 1, sample.GetFavorite());
        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, hiveName.c_str(), hiveName.size(), SQLITE_STATIC));

        throw_on_sqlite3_error(sqlite3_step(statement.stmt));
        // rc = sqlite3_clear_bindings(statement.stmt);
        // rc = sqlite3_reset(statement.stmt);
        // }

        // rc = sqlite3_exec(m_Database, "END TRANSACTION", NULL, NULL, &m_ErrMsg);

        wxLogDebug("Data inserted successfully.");
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateHive(const std::string &hiveOldName, const std::string &hiveNewName)
{
    try
    {
        const auto sql = "UPDATE HIVES SET HIVE = ? WHERE HIVE = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, hiveNewName.c_str(), hiveNewName.size(), SQLITE_STATIC));
        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 2, hiveOldName.c_str(), hiveOldName.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) != SQLITE_DONE)
        {
            wxLogWarning("No data inserted.");
        }
        else
        {
            wxLogDebug("Hive updated successfully. %s", m_ErrMsg);
        }
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }
}

void Database::UpdateHiveName(const std::string &filename, const std::string &hiveName)
{
    try
    {
        const auto sql = "UPDATE SAMPLES SET HIVE = ? WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, hiveName.c_str(), hiveName.size(), SQLITE_STATIC));
        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        wxLogDebug("Updated record successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot update record: ", "Error", e.what());
    }
}

void Database::UpdateFavoriteColumn(const std::string &filename, int value)
{
    try
    {
        const auto sql = "UPDATE SAMPLES SET FAVORITE = ? WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_int(statement.stmt, 1, value));
        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        wxLogDebug("Updated record successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot update record: ", "Error", e.what());
    }
}

void Database::UpdateSamplePack(const std::string &filename, const std::string &samplePack)
{
    try
    {
        const auto sql = "UPDATE SAMPLES SET SAMPLEPACK = ? WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, samplePack.c_str(), samplePack.size(), SQLITE_STATIC));
        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        wxLogDebug("Updated record successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot update record: ", "Error", e.what());
    }
}

void Database::UpdateSampleType(const std::string &filename, const std::string &type)
{
    try
    {
        const auto sql = "UPDATE SAMPLES SET TYPE = ? WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, type.c_str(), type.size(), SQLITE_STATIC));
        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }
        wxLogDebug("Updated record successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot update record: ", "Error", e.what());
    }
}

std::string Database::GetSampleType(const std::string &filename)
{
    std::string type;

    try
    {
        const auto sql = "SELECT TYPE FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");

            type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 0)));
        }

        wxLogDebug("Selected data from table successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot get sample type column value from table: ", "Error", e.what());
    }

    return type;
}

int Database::GetFavoriteColumnValueByFilename(const std::string &filename)
{
    int value = 0;

    try
    {
        const auto sql = "SELECT FAVORITE FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");
            value = sqlite3_column_int(statement.stmt, 0);
        }

        wxLogDebug("Selected data from table successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot get favorite column value from table: ", "Error", e.what());
    }

    return value;
}

std::string Database::GetHiveByFilename(const std::string &filename)
{
    std::string hive;

    try
    {
        const auto sql = "SELECT HIVE FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");
            hive = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 0)));
        }

        wxLogDebug("Selected data from table successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot get hive value from table: ", "Error", e.what());
    }

    return hive;
}

void Database::RemoveSampleFromDatabase(const std::string &filename)
{
    try
    {
        const auto sql = "DELETE FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_DONE)
        {
            wxLogDebug("Record found, Deleting..");
        }

        wxLogDebug("Deleted data from table successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot get hive value from table: ", "Error", e.what());
    }
}

void Database::RemoveHiveFromDatabase(const std::string &hiveName)
{
    try
    {
        const auto sql = "DELETE FROM HIVES WHERE HIVE = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, hiveName.c_str(), hiveName.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_DONE)
        {
            wxLogDebug("Record found, Deleting..");
        }

        wxLogDebug("Deleted data from table successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot get hive value from table: ", "Error", e.what());
    }
}

std::string Database::GetSamplePathByFilename(const std::string &filename)
{
    std::string path;

    try
    {
        const auto sql = "SELECT PATH FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");
            path = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 0)));
        }

        wxLogDebug("Selected data from table successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot select sample path from table: ", "Error", e.what());
    }

    return path;
}

std::string Database::GetSampleFileExtension(const std::string &filename)
{
    std::string extension;

    try
    {
        const auto sql = "SELECT EXTENSION FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogInfo("Record found, fetching..");
            extension = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 0)));
        }

        wxLogDebug("Selected data from table successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot select sample extension from table: ", "Error", e.what());
    }

    return extension;
}

wxVector<wxVector<wxVariant>> Database::LoadSamplesDatabase(wxDataViewTreeCtrl &favorite_tree,
                                                            wxDataViewItem &favorite_item,
                                                            wxTreeCtrl &trash_tree, wxTreeItemId &trash_item,
                                                            bool show_extension,
                                                            const std::string &icon_star_filled,
                                                            const std::string &icon_star_empty)
{
    wxVector<wxVector<wxVariant>> vecSet;

    wxVariant icon_filled, icon_empty;
    icon_filled = wxVariant(wxBitmap(icon_star_filled));
    icon_empty = wxVariant(wxBitmap(icon_star_empty));

    try
    {
        int num_rows = 0;

        Sqlite3Statement statement1(m_Database, "SELECT Count(*) FROM SAMPLES;");

        if (SQLITE_ROW == sqlite3_step(statement1.stmt))
        {
            num_rows = sqlite3_column_int(statement1.stmt, 0);

            wxLogDebug("Loading %d samples..", num_rows);

            vecSet.reserve(num_rows);
        }

        Sqlite3Statement statement(m_Database, "SELECT FAVORITE, FILENAME, EXTENSION, SAMPLEPACK, \
                                                TYPE, CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH, \
                                                TRASHED, HIVE FROM SAMPLES;");

        int row = 0;

        while (SQLITE_ROW == sqlite3_step(statement.stmt))
        {
            int favorite = sqlite3_column_int(statement.stmt, 0);
            wxString filename = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 1)));
            wxString file_extension = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 2)));
            wxString sample_pack = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 3)));
            wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 4)));
            int channels = sqlite3_column_int(statement.stmt, 5);
            int length = sqlite3_column_int(statement.stmt, 6);
            int sample_rate = sqlite3_column_int(statement.stmt, 7);
            int bitrate = sqlite3_column_int(statement.stmt, 8);
            wxString path = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 9)));
            int trashed = sqlite3_column_int(statement.stmt, 10);
            wxString hive_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 11)));

            wxLongLong llLength = length;
            int total_min = static_cast<int>((llLength / 60000).GetValue());
            int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

            wxVector<wxVariant> vec;
            vec.reserve(12);

            if (trashed == 1)
            {
                if (show_extension)
                    trash_tree.AppendItem(trash_item, wxString::Format("%s.%s", filename, file_extension));
                else
                    trash_tree.AppendItem(trash_item, filename);
            }
            else
            {
                if (favorite == 1)
                {
                    // vec.push_back(true);
                    vec.push_back(icon_filled);

                    std::deque<wxDataViewItem> nodes;
                    nodes.push_back(favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), 0));

                    wxDataViewItem current_item, found_item;

                    int row = 0;
                    int hive_count = favorite_tree.GetChildCount(wxDataViewItem(wxNullPtr));

                    while (!nodes.empty())
                    {
                        current_item = nodes.front();
                        nodes.pop_front();

                        if (favorite_tree.GetItemText(current_item) == hive_name)
                        {
                            found_item = current_item;
                            break;
                        }

                        wxDataViewItem child = favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), 0);

                        while (row < (hive_count - 1))
                        {
                            row++;

                            child = favorite_tree.GetNthChild(wxDataViewItem(wxNullPtr), row);
                            nodes.push_back(child);
                        }
                    }

                    nodes.clear();

                    if (found_item.IsOk())
                    {
                        if (show_extension)
                            favorite_tree.AppendItem(found_item,
                                                     wxString::Format("%s.%s", filename, file_extension));
                        else
                            favorite_tree.AppendItem(found_item, filename);
                    }
                }
                else
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
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot load data from table: ", "Error", e.what());
    }

    return vecSet;
}

wxVector<wxVector<wxVariant>>
Database::FilterDatabaseBySampleName(const std::string &sampleName, bool show_extension,
                                     const std::string &icon_star_filled, const std::string &icon_star_empty)
{
    wxVector<wxVector<wxVariant>> sampleVec;

    wxVariant icon_filled, icon_empty;
    icon_filled = wxVariant(wxBitmap(icon_star_filled));
    icon_empty = wxVariant(wxBitmap(icon_star_empty));

    try
    {
        Sqlite3Statement statement(m_Database, "SELECT FAVORITE, FILENAME, SAMPLEPACK, TYPE, \
                                                CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH \
                                                FROM SAMPLES WHERE FILENAME LIKE '%' || ? || '%' ;");

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, sampleName.c_str(), sampleName.size(), SQLITE_STATIC));

        int row = 0;

        while (SQLITE_ROW == sqlite3_step(statement.stmt))
        {
            wxLogDebug("Record found, fetching..");
            int favorite = sqlite3_column_int(statement.stmt, 0);
            wxString filename = wxString(std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 1))));
            wxString sample_pack = wxString(std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 2))));
            wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 3)));
            int channels = sqlite3_column_int(statement.stmt, 4);
            int length = sqlite3_column_int(statement.stmt, 5);
            int sample_rate = sqlite3_column_int(statement.stmt, 6);
            int bitrate = sqlite3_column_int(statement.stmt, 7);
            wxString path = wxString(std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 8))));

            wxLongLong llLength = length;
            int total_min = static_cast<int>((llLength / 60000).GetValue());
            int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

            wxVector<wxVariant> vec;

            if (favorite == 1)
                vec.push_back(icon_filled);
            else
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

            sampleVec.push_back(vec);

            row++;
        }
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot filter data from table: ", "Error", e.what());
    }

    return sampleVec;
}

wxVector<wxVector<wxVariant>>
Database::FilterDatabaseByHiveName(const std::string &hiveName, bool show_extension,
                                   const std::string &icon_star_filled, const std::string &icon_star_empty)
{
    wxVector<wxVector<wxVariant>> sampleVec;

    wxVariant icon_filled, icon_empty;
    icon_filled = wxVariant(wxBitmap(icon_star_filled));
    icon_empty = wxVariant(wxBitmap(icon_star_empty));

    try
    {
        Sqlite3Statement statement(m_Database, "SELECT FAVORITE, FILENAME, SAMPLEPACK, TYPE, \
                                                CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH \
                                                FROM SAMPLES WHERE HIVE = ? AND FAVORITE = 1;");

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, hiveName.c_str(), hiveName.size(), SQLITE_STATIC));

        int row = 0;

        while (SQLITE_ROW == sqlite3_step(statement.stmt))
        {
            wxLogDebug("Record found, fetching..");
            int favorite = sqlite3_column_int(statement.stmt, 0);
            wxString filename = wxString(std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 1))));
            wxString sample_pack = wxString(std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 2))));
            wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 3)));
            int channels = sqlite3_column_int(statement.stmt, 4);
            int length = sqlite3_column_int(statement.stmt, 5);
            int sample_rate = sqlite3_column_int(statement.stmt, 6);
            int bitrate = sqlite3_column_int(statement.stmt, 7);
            wxString path = wxString(std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 8))));

            wxLongLong llLength = length;
            int total_min = static_cast<int>((llLength / 60000).GetValue());
            int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

            wxVector<wxVariant> vec;

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
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot filter data from table: ", "Error", e.what());
    }

    return sampleVec;
}

void Database::LoadHivesDatabase(wxDataViewTreeCtrl &treeCtrl)
{
    try
    {
        const auto sql = "SELECT HIVE FROM HIVES;";

        Sqlite3Statement statement(m_Database, sql);

        while (SQLITE_ROW == sqlite3_step(statement.stmt))
        {
            wxLogDebug("Loading hives..");

            const auto hive = wxString(std::string(reinterpret_cast<const char *>
                                                   (sqlite3_column_text(statement.stmt, 0))));

            treeCtrl.AppendContainer(wxDataViewItem(wxNullPtr), hive);
        }
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot load hive from hives table: ", "Error", e.what());
    }
}

//Compares the input array with the database and removes duplicates.
wxArrayString Database::CheckDuplicates(const wxArrayString &files)
{
    wxArrayString sorted_files;

    std::string filename;
    std::string sample;

    try
    {
        const auto sql = "SELECT * FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        for (unsigned int i = 0; i < files.size(); i++)
        {
            filename = files[i].AfterLast('/').BeforeLast('.').ToStdString();

            throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

            if (sqlite3_step(statement.stmt) != SQLITE_ROW)
                sorted_files.push_back(files[i]);
            else
                wxLogDebug("Already added: %s. Skipping..", files[i]);

            rc = sqlite3_clear_bindings(statement.stmt);
            rc = sqlite3_reset(statement.stmt);
        }
    }
    catch (const std::exception &exception)
    {
        wxLogDebug(exception.what());
    }

    return sorted_files;
}

bool Database::IsTrashed(const std::string &filename)
{
    try
    {
        const auto sql = "SELECT TRASHED FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, fetching..");

            if (sqlite3_column_int(statement.stmt, 0) == 1)
                return true;
        }

        wxLogDebug("Selected data from table successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot select sample path from table: ", "Error", e.what());
    }

    return false;
}

void Database::UpdateTrashColumn(const std::string &filename, int value)
{
    try
    {
        const auto sql = "UPDATE SAMPLES SET TRASHED = ? WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_int(statement.stmt, 1, value));
        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 2, filename.c_str(), filename.size(), SQLITE_STATIC));

        if (sqlite3_step(statement.stmt) == SQLITE_ROW)
        {
            wxLogDebug("Record found, updating..");
        }

        wxLogDebug("Updated record successfully.");
    }
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot update record: ", "Error", e.what());
    }
}

wxVector<wxVector<wxVariant>>
Database::RestoreFromTrashByFilename(const std::string &filename,
                                     wxVector<wxVector<wxVariant>> &vecSet, bool show_extension,
                                     const std::string &icon_star_filled, const std::string &icon_star_empty)
{
    wxVariant icon_filled, icon_empty;
    icon_filled = wxVariant(wxBitmap(icon_star_filled));
    icon_empty = wxVariant(wxBitmap(icon_star_empty));

    try
    {
        const auto sql = "SELECT FAVORITE, FILENAME, EXTENSION, SAMPLEPACK, \
                          TYPE, CHANNELS, LENGTH, SAMPLERATE, BITRATE, PATH, \
                          TRASHED, HIVE FROM SAMPLES WHERE FILENAME = ?;";

        Sqlite3Statement statement(m_Database, sql);

        throw_on_sqlite3_error(sqlite3_bind_text(statement.stmt, 1, filename.c_str(), filename.size(), SQLITE_STATIC));

        while (SQLITE_ROW == sqlite3_step(statement.stmt))
        {
            int favorite = sqlite3_column_int(statement.stmt, 0);
            wxString filename = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 1)));
            wxString file_extension = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 2)));
            wxString sample_pack = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 3)));
            wxString sample_type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 4)));
            int channels = sqlite3_column_int(statement.stmt, 5);
            int length = sqlite3_column_int(statement.stmt, 6);
            int sample_rate = sqlite3_column_int(statement.stmt, 7);
            int bitrate = sqlite3_column_int(statement.stmt, 8);
            wxString path = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 9)));
            int trashed = sqlite3_column_int(statement.stmt, 10);
            wxString hive_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement.stmt, 11)));

            wxLongLong llLength = length;
            int total_min = static_cast<int>((llLength / 60000).GetValue());
            int total_sec = static_cast<int>(((llLength % 60000) / 1000).GetValue());

            wxVector<wxVariant> vec;

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
    catch (const std::exception &e)
    {
        show_modal_dialog_and_log("Error! Cannot load data from table: ", "Error", e.what());
    }

    return vecSet;
}

void Database::OpenDatabase(const std::string &dbPath)
{
    throw_on_sqlite3_error(sqlite3_open(dbPath.c_str(), &m_Database));
}

void Database::CloseDatabase()
{
    throw_on_sqlite3_error(sqlite3_close(m_Database));
}
