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
#include <memory>
#include <vector>
#include "Sample.hpp"
#include <wx/vector.h>

class IDatabase
{    public:
        virtual ~IDatabase() = default;
        // -------------------------------------------------------------------
        // Create the table
        virtual void CreateTableSamples() = 0;
        virtual void CreateTableHives() = 0;

        // -------------------------------------------------------------------
        // Insert into database
        virtual void InsertIntoSamples(std::vector<Sample>) = 0;
        virtual void InsertIntoHives(const std::string& hiveName) = 0;
        
        // -------------------------------------------------------------------
        // Update database
        virtual void UpdateFavoriteColumn(const std::string& filename, int value) = 0;
        virtual void UpdateHive(const std::string& hiveOldName, const std::string& hiveNewName) = 0;
        virtual void UpdateHiveName(const std::string& filename, const std::string& hiveName) = 0;
        virtual void UpdateTrashColumn(const std::string& filename, int value) = 0;
        virtual void UpdateSamplePack(const std::string& filename, const std::string& samplePack) = 0;
        virtual void UpdateSampleType(const std::string& filename, const std::string& type) = 0;

        // -------------------------------------------------------------------
        // Get from database
        virtual int GetFavoriteColumnValueByFilename(const std::string& filename) = 0;
        virtual std::string GetHiveByFilename(const std::string& filename) = 0;
        virtual std::string GetSamplePathByFilename(const std::string& filename) = 0;
        virtual std::string GetSampleFileExtension(const std::string& filename) = 0;
        virtual std::string GetSampleType(const std::string& filename) = 0;

        // -------------------------------------------------------------------
        // Check database
        virtual bool IsTrashed(const std::string& filename) = 0;
        virtual wxArrayString CheckDuplicates(const wxArrayString& files) = 0;

        // -------------------------------------------------------------------
        // Remove from database
        virtual void RemoveSampleFromDatabase(const std::string& filename) = 0;
        virtual void RemoveHiveFromDatabase(const std::string& hiveName) = 0;

        // -------------------------------------------------------------------
        virtual wxVector<wxVector<wxVariant>>
        // LoadDatabase(wxVector<wxVector<wxVariant>> &vecSet,
        //              wxTreeCtrl& favorite_tree, wxTreeItemId& favorite_item,
        //              wxTreeCtrl& trash_tree, wxTreeItemId& trash_item, bool show_extension) = 0;
        LoadSamplesDatabase(wxVector<wxVector<wxVariant>>& vecSet,
                            wxDataViewTreeCtrl& favorite_tree, wxDataViewItem& favorite_item,
                            wxTreeCtrl& trash_tree, wxTreeItemId& trash_item, bool show_extension,
                            const std::string& icon_star_filled, const std::string& icon_star_emtpy) = 0;
        virtual void LoadHivesDatabase(wxDataViewTreeCtrl& favorite_tree) = 0;
        virtual wxVector<wxVector<wxVariant>>
        RestoreFromTrashByFilename(const std::string& filename,
                                   wxVector<wxVector<wxVariant>>& vecSet, bool show_extension,
                                   const std::string& icon_star_filled, const std::string& icon_star_empty) = 0;
        virtual wxVector<wxVector<wxVariant>>
        FilterDatabaseBySampleName(wxVector<wxVector<wxVariant>>& sampleVec,
                                   const std::string& sampleName, bool show_extension,
                                   const std::string& icon_star_filled, const std::string& icon_star_empty) = 0;
        virtual wxVector<wxVector<wxVariant>>
        FilterDatabaseByHiveName(wxVector<wxVector<wxVariant>>& sampleVec,
                                 const std::string& hiveName, bool show_extension,
                                 const std::string& icon_star_filled, const std::string& icon_star_empty) = 0;
};

enum class DatabaseType {
    SqlLite
};

std::unique_ptr<IDatabase> createSqlDatabase(DatabaseType type, wxInfoBar& infoBar, const std::string& dbPath);