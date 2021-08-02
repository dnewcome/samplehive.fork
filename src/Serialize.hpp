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

#include <wx/checkbox.h>
#include <wx/dataview.h>
#include <wx/font.h>
#include <wx/string.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/null.h>
#include <yaml-cpp/emittermanip.h>

struct FontType
{
    wxString font_face;
    int font_size;
};

struct ImportDirInfo
{
    bool auto_import;
    wxString import_dir;
};

class Serializer
{
    public:
        Serializer(const std::string& filepath);
        ~Serializer();

    private:
        // -------------------------------------------------------------------
        const std::string& m_Filepath;

        YAML::Emitter m_Emitter;

    public:
        // -------------------------------------------------------------------
        // Window size
        int DeserializeWinSize(std::string key, int size) const;

        // -------------------------------------------------------------------
        // Browser controls
        void SerializeBrowserControls();
        bool DeserializeBrowserControls(std::string key, bool control) const;

        // -------------------------------------------------------------------
        // Display settings
        void SerializeDisplaySettings(wxFont& font);
        FontType DeserializeDisplaySettings() const;
        void SerializeWaveformColour(wxColour& colour);
        wxColour DeserializeWaveformColour() const;

        // -------------------------------------------------------------------
        // Auto import settings
        void SerializeAutoImportSettings(wxTextCtrl& textCtrl, wxCheckBox& checkBox);
        ImportDirInfo DeserializeAutoImportSettings() const;

        // -------------------------------------------------------------------
        // Show file extension
        void SerializeShowFileExtensionSetting(wxCheckBox& checkBox);
        bool DeserializeShowFileExtensionSetting() const;

        // -------------------------------------------------------------------
        // Favorite samples
        void SerializeDataViewTreeCtrlItems(wxTreeCtrl& tree, wxTreeItemId& item);
        bool DeserializeDataViewTreeCtrlItems(YAML::Emitter& out, wxDataViewItem item) const;
};
