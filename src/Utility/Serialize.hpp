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

typedef std::pair<int, int> WindowSize;
// typedef std::pair<wxString, int> FontType;
typedef std::pair<bool, wxString> ImportDirInfo;

class Serializer
{
    public:
        Serializer();
        ~Serializer();

    private:
        // -------------------------------------------------------------------
        YAML::Emitter m_Emitter;

    public:
        // -------------------------------------------------------------------
        // Window size
        WindowSize DeserializeWinSize() const;

        // -------------------------------------------------------------------
        // Menu and status bar
        void SerializeShowMenuAndStatusBar(std::string key, bool value);
        bool DeserializeShowMenuAndStatusBar(std::string key) const;

        // -------------------------------------------------------------------
        // Browser controls
        void SerializeBrowserControls(std::string key, bool value);
        bool DeserializeBrowserControls(std::string key) const;

        // -------------------------------------------------------------------
        // Display settings
        void SerializeDisplaySettings(wxFont& font);
        wxFont DeserializeDisplaySettings() const;

        // -------------------------------------------------------------------
        // Waveform colour
        void SerializeWaveformColour(wxColour& colour);
        wxColour DeserializeWaveformColour() const;

        // -------------------------------------------------------------------
        // Auto import settings
        void SerializeAutoImportSettings(bool autoImport, const std::string& importDir);
        ImportDirInfo DeserializeAutoImportSettings() const;

        // -------------------------------------------------------------------
        // Follow symbolic links
        void SerializeFollowSymLink(bool followSymLink);
        bool DeserializeFollowSymLink() const;

        // -------------------------------------------------------------------
        // Show file extension
        void SerializeShowFileExtensionSetting(bool showExtension);
        bool DeserializeShowFileExtensionSetting() const;
};
