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

#include <string>
#include <vector>
#include <iostream>

#include <wx/dataview.h>
#include <wx/infobar.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/vector.h>
#include <wx/variant.h>

#include <sqlite3.h>

#include "Sample.hpp"

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
        void CreateTableSamples(const std::string& dbPath);
        void CreateTableHives(const std::string& dbPath);

        // -------------------------------------------------------------------
        // Insert into database
        void InsertIntoSamples(const std::string& dbPath, std::vector<Sample>);
        void InsertIntoHives(const std::string& dbPath, const std::string& hiveName);
        
        // -------------------------------------------------------------------
        // Update database
        void UpdateFavoriteColumn(const std::string& dbPath, const std::string& filename, int value);
        void UpdateHive(const std::string& dbPath, const std::string& hiveOldName, const std::string& hiveNewName);
        void UpdateHiveName(const std::string& dbPath, const std::string& filename, const std::string& hiveName);
        void UpdateTrashColumn(const std::string& dbPath, const std::string& filename, int value);
        void UpdateSamplePack(const std::string& dbPath, const std::string& filename, const std::string& samplePack);
        void UpdateSampleType(const std::string& dbPath, const std::string& filename, const std::string& type);

        // -------------------------------------------------------------------
        // Get from database
        int GetFavoriteColumnValueByFilename(const std::string& dbPath, const std::string& filename);
        std::string GetHiveByFilename(const std::string& dbPath, const std::string& filename);
        std::string GetSamplePathByFilename(const std::string& dbPath, const std::string& filename);
        std::string GetSampleFileExtension(const std::string& dbPath, const std::string& filename);
        std::string GetSampleType(const std::string& dbPath, const std::string& filename);

        // -------------------------------------------------------------------
        // Check database
        bool IsTrashed(const std::string& dbPath, const std::string& filename);
        wxArrayString CheckDuplicates(const std::string& dbPath, const wxArrayString& files);

        // -------------------------------------------------------------------
        // Remove from database
        void RemoveSampleFromDatabase(const std::string& dbPath, const std::string& filename);
        void RemoveHiveFromDatabase(const std::string& dbPath, const std::string& hiveName);

        // -------------------------------------------------------------------
        wxVector<wxVector<wxVariant>>
        // LoadDatabase(wxVector<wxVector<wxVariant>> &vecSet,
        //              wxTreeCtrl& favorite_tree, wxTreeItemId& favorite_item,
        //              wxTreeCtrl& trash_tree, wxTreeItemId& trash_item, bool show_extension);
        LoadSamplesDatabase(const std::string& dbPath, wxVector<wxVector<wxVariant>>& vecSet,
                            wxDataViewTreeCtrl& favorite_tree, wxDataViewItem& favorite_item,
                            wxTreeCtrl& trash_tree, wxTreeItemId& trash_item, bool show_extension,
                            const std::string& icon_star_filled, const std::string& icon_star_emtpy);
        void LoadHivesDatabase(const std::string& dbPath, wxDataViewTreeCtrl& favorite_tree);
        wxVector<wxVector<wxVariant>>
        RestoreFromTrashByFilename(const std::string& dbPath, const std::string& filename,
                                   wxVector<wxVector<wxVariant>>& vecSet, bool show_extension,
                                   const std::string& icon_star_filled, const std::string& icon_star_empty);
        wxVector<wxVector<wxVariant>>
        FilterDatabaseBySampleName(const std::string& dbPath, wxVector<wxVector<wxVariant>>& sampleVec,
                                   const std::string& sampleName, bool show_extension,
                                   const std::string& icon_star_filled, const std::string& icon_star_empty);
        wxVector<wxVector<wxVariant>>
        FilterDatabaseByHiveName(const std::string& dbPath, wxVector<wxVector<wxVariant>>& sampleVec,
                                 const std::string& hiveName, bool show_extension,
                                 const std::string& icon_star_filled, const std::string& icon_star_empty);
};
