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

#include "Sample.hpp"

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

#include "IDatabase.hpp"

class Database : public IDatabase
{
    public:
        Database(wxInfoBar& infoBar, const std::string& dbPath);
        ~Database();

    private:
        // -------------------------------------------------------------------
        sqlite3* m_Database;
        int rc;
        char* m_ErrMsg;
    private:
        // -------------------------------------------------------------------
        wxInfoBar& m_InfoBar;

        void open(const std::string& dbPath);
        void close();

    public:
        // -------------------------------------------------------------------
        // Create the table
        void CreateTableSamples() override;
        void CreateTableHives() override;

        // -------------------------------------------------------------------
        // Insert into database
        void InsertIntoSamples(std::vector<Sample>) override;
        void InsertIntoHives(const std::string& hiveName) override;
        
        // -------------------------------------------------------------------
        // Update database
        void UpdateFavoriteColumn(const std::string& filename, int value) override;
        void UpdateHive(const std::string& hiveOldName, const std::string& hiveNewName) override;
        void UpdateHiveName(const std::string& filename, const std::string& hiveName) override;
        void UpdateTrashColumn(const std::string& filename, int value) override;
        void UpdateSamplePack(const std::string& filename, const std::string& samplePack) override;
        void UpdateSampleType(const std::string& filename, const std::string& type) override;

        // -------------------------------------------------------------------
        // Get from database
        int GetFavoriteColumnValueByFilename(const std::string& filename) override;
        std::string GetHiveByFilename(const std::string& filename) override;
        std::string GetSamplePathByFilename(const std::string& filename) override;
        std::string GetSampleFileExtension(const std::string& filename) override;
        std::string GetSampleType(const std::string& filename) override;

        // -------------------------------------------------------------------
        // Check database
        bool IsTrashed(const std::string& filename) override;
        wxArrayString CheckDuplicates(const wxArrayString& files) override;

        // -------------------------------------------------------------------
        // Remove from database
        void RemoveSampleFromDatabase(const std::string& filename) override;
        void RemoveHiveFromDatabase(const std::string& hiveName) override;

        // -------------------------------------------------------------------
        wxVector<wxVector<wxVariant>>
        // LoadDatabase(wxVector<wxVector<wxVariant>> &vecSet,
        //              wxTreeCtrl& favorite_tree, wxTreeItemId& favorite_item,
        //              wxTreeCtrl& trash_tree, wxTreeItemId& trash_item, bool show_extension) override;
        LoadSamplesDatabase(wxVector<wxVector<wxVariant>>& vecSet,
                            wxDataViewTreeCtrl& favorite_tree, wxDataViewItem& favorite_item,
                            wxTreeCtrl& trash_tree, wxTreeItemId& trash_item, bool show_extension,
                            const std::string& icon_star_filled, const std::string& icon_star_emtpy) override;
        void LoadHivesDatabase(wxDataViewTreeCtrl& favorite_tree) override;
        wxVector<wxVector<wxVariant>>
        RestoreFromTrashByFilename(const std::string& filename,
                                   wxVector<wxVector<wxVariant>>& vecSet, bool show_extension,
                                   const std::string& icon_star_filled, const std::string& icon_star_empty) override;
        wxVector<wxVector<wxVariant>>
        FilterDatabaseBySampleName(wxVector<wxVector<wxVariant>>& sampleVec,
                                   const std::string& sampleName, bool show_extension,
                                   const std::string& icon_star_filled, const std::string& icon_star_empty) override;
        wxVector<wxVector<wxVariant>>
        FilterDatabaseByHiveName(wxVector<wxVector<wxVariant>>& sampleVec,
                                 const std::string& hiveName, bool show_extension,
                                 const std::string& icon_star_filled, const std::string& icon_star_empty) override;
};
