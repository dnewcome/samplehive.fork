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

#include <fstream>
#include <sstream>

#include <wx/colour.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/node/parse.h>

#include "Serialize.hpp"

Serializer::Serializer(const std::string& filepath)
    : m_Filepath(filepath)
{
    std::ifstream ifstrm(m_Filepath);

    wxFont font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    std::string system_font_face = font.GetFaceName().ToStdString();
    int system_font_size = font.GetPointSize();

    wxColour colour = "#FE9647";

    std::string dir = wxStandardPaths::Get().GetDocumentsDir().ToStdString();

    if (!ifstrm)
    {
        wxLogDebug("Genrating configuration file..");

        m_Emitter << YAML::Comment("This is the configuration file for SampleHive,"
                                   "feel free to edit the file as needed");
        m_Emitter << YAML::Newline;

        m_Emitter << YAML::BeginMap;

        m_Emitter << YAML::Newline << YAML::Key << "Window";
        m_Emitter << YAML::BeginMap;
        m_Emitter << YAML::Key << "Width" << YAML::Value << 1280;
        m_Emitter << YAML::Key << "Height" << YAML::Value << 720;
        m_Emitter << YAML::EndMap << YAML::Newline;

        m_Emitter << YAML::Newline << YAML::Key << "Media";
        m_Emitter << YAML::BeginMap;
        m_Emitter << YAML::Key << "Autoplay" << YAML::Value << false;
        m_Emitter << YAML::Key << "Loop" << YAML::Value << false;
        m_Emitter << YAML::Key << "Muted" << YAML::Value << false;
        m_Emitter << YAML::EndMap << YAML::Newline;

        m_Emitter << YAML::Newline << YAML::Key << "Display";
        m_Emitter << YAML::BeginMap;
        m_Emitter << YAML::Key << "Font";
        m_Emitter << YAML::BeginMap;
        m_Emitter << YAML::Key << "Family" << YAML::Value << system_font_face;
        m_Emitter << YAML::Key << "Size" << YAML::Value << system_font_size;
        m_Emitter << YAML::EndMap;
        m_Emitter << YAML::EndMap << YAML::Newline;

        m_Emitter << YAML::Newline << YAML::Key << "Waveform";
        m_Emitter << YAML::BeginMap;
        m_Emitter << YAML::Key << "Colour" << YAML::Value << colour.GetAsString().ToStdString();
        m_Emitter << YAML::EndMap << YAML::Newline;

        m_Emitter << YAML::Newline << YAML::Key << "Collection";
        m_Emitter << YAML::BeginMap;
        m_Emitter << YAML::Key << "AutoImport" << YAML::Value << false;
        m_Emitter << YAML::Key << "Directory" << YAML::Value << dir;
        m_Emitter << YAML::Key << "ShowFileExtension" << YAML::Value << true;
        m_Emitter << YAML::EndMap << YAML::Newline;

        m_Emitter << YAML::EndMap;

        std::ofstream ofstrm(m_Filepath);
        ofstrm << m_Emitter.c_str();

        wxLogDebug("Generated %s successfully!", m_Filepath);
    }
    else
    {
        wxLogDebug("Config file already exists! Skipping..");
    }
}

Serializer::~Serializer()
{

}

int Serializer::DeserializeWinSize(std::string key, int size) const
{
    int width = 800, height = 600;

    try
    {
        YAML::Node data = YAML::LoadFile(m_Filepath);

        if (!data["Window"])
        {
            return false;
        }

        if (auto win = data["Window"])
        {
            if (key == "Height")
            {
                size = height;
                size = win["Height"].as<int>();
            }

            if (key == "Width")
            {
                size = width;
                size = win["Width"].as<int>();
            }
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    wxLogDebug("Window size: %d", size);

    return size;
}

bool Serializer::DeserializeBrowserControls(std::string key, bool control) const
{
    bool autoplay = false; bool loop = false; bool muted = false;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto media = config["Media"])
        {
            if (key == "autoplay")
            {
                control = autoplay;
                autoplay = media["Autoplay"].as<bool>();
            }

            if (key == "loop")
            {
                control = loop;
                loop = media["Loop"].as<bool>();
            }

            if (key == "muted")
            {
                control = muted;
                muted = media["Muted"].as<bool>();
            }
        }
        else
        {
            wxLogDebug("Error! Cannot fetch values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    wxLogDebug("Autoplay: %s, Loop: %s, Muted: %s",
               autoplay ? "enabled" : "disabled",
               loop ? "enabled" : "disabled",
               muted ? "muted" : "unmuted");

    return control;
}

void Serializer::SerializeDisplaySettings(wxFont& font)
{
    YAML::Emitter out;

    std::string font_face = font.GetFaceName().ToStdString();
    int font_size = font.GetPointSize();

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        auto display = config["Display"];

        if (auto fontSetting = display["Font"])
        {
            wxLogDebug("Changing font settings");
            wxLogDebug("Font face: %s", font_face);
            wxLogDebug("Font size: %d", font_size);

            fontSetting["Family"] = font_face;
            fontSetting["Size"] = font_size;

            out << config;

            std::ofstream ofstrm(m_Filepath);
            ofstrm << out.c_str();
        }
        else
        {
            wxLogDebug("Error! Cannot store font values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

FontType Serializer::DeserializeDisplaySettings() const
{
    wxString face;
    int size = 0 ;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        auto display = config["Display"];

        if (auto font_setting = display["Font"])
        {
            face = font_setting["Family"].as<std::string>();
            size = font_setting["Size"].as<int>();
        }
        else
        {
            wxLogDebug("Error! Cannot fetch font values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return { face, size };
}

void Serializer::SerializeWaveformColour(wxColour& colour)
{
    YAML::Emitter out;

    std::string colour_string = colour.GetAsString(wxC2S_HTML_SYNTAX).ToStdString();

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto waveform = config["Waveform"])
        {
            wxLogDebug("Changing waveform colour");
            wxLogDebug("Waveform colour: %s", colour_string);

            waveform["Colour"] = colour_string;

            out << config;

            std::ofstream ofstrm(m_Filepath);
            ofstrm << out.c_str();
        }
        else
        {
            wxLogDebug("Error! Cannot store waveform colour.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

wxColour Serializer::DeserializeWaveformColour() const
{
    std::string colour;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto waveform = config["Waveform"])
        {
            colour = waveform["Colour"].as<std::string>();
        }
        else
        {
            wxLogDebug("Error! Cannot fetch waveform colour.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return static_cast<wxString>(colour);
}

void Serializer::SerializeAutoImportSettings(wxTextCtrl& textCtrl, wxCheckBox& checkBox)
{
    YAML::Emitter out;

    std::string import_dir = textCtrl.GetValue().ToStdString();
    bool auto_import = checkBox.GetValue();

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto autoImportInfo = config["Collection"])
        {
            autoImportInfo["AutoImport"] = auto_import;
            autoImportInfo["Directory"] = import_dir;

            out << config;

            std::ofstream ofstrm(m_Filepath);
            ofstrm << out.c_str();
        }
        else
        {
            wxLogDebug("Error! Cannot store import dir values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

ImportDirInfo Serializer::DeserializeAutoImportSettings() const
{
    wxString dir;
    bool auto_import = false;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto autoImportInfo = config["Collection"])
        {
            auto_import = autoImportInfo["AutoImport"].as<bool>();
            dir = autoImportInfo["Directory"].as<std::string>();
        }
        else
        {
            wxLogDebug("Error! Cannot fetch import dir values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return { auto_import, dir};
}

void Serializer::SerializeShowFileExtensionSetting(wxCheckBox& checkBox)
{
    YAML::Emitter out;

    bool show_extension = checkBox.GetValue();

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto fileExtensionInfo = config["Collection"])
        {
            fileExtensionInfo["ShowFileExtension"] = show_extension;

            out << config;

            wxLogDebug("Changing show file extension value.");

            std::ofstream ofstrm(m_Filepath);
            ofstrm << out.c_str();
        }
        else
        {
            wxLogDebug("Error! Cannot store import dir values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

}

bool Serializer::DeserializeShowFileExtensionSetting() const
{
    bool show_extension = false;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto fileExtensionInfo = config["Collection"])
        {
            show_extension = fileExtensionInfo["ShowFileExtension"].as<bool>();
        }
        else
        {
            wxLogDebug("Error! Cannot fetch import dir values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return show_extension;
}

void Serializer::SerializeDataViewTreeCtrlItems(wxTreeCtrl& tree, wxTreeItemId& item)
{
    std::string path = "tree.yaml";

    std::ifstream ifstrm(path);

    YAML::Emitter out;

    out << YAML::BeginMap; // Container
    out << YAML::Key << "Container" << YAML::Value << "";

    if (tree.HasChildren(item))
    {
        out << YAML::Key << "Child";
        out << YAML::BeginMap; // Child

        for ( size_t i = 0; i < tree.GetChildrenCount(item); i++ )
        {
            // wxTreeItemIdValue cookie;
            wxString child = tree.GetItemText(tree.GetSelection());
            out << YAML::Key << "Item" << YAML::Value << child.ToStdString();
        }

        out << YAML::EndMap; // Child
    }

    out << YAML::EndMap; // Container

    std::ofstream ofstrm(path);
    ofstrm << out.c_str();
}

bool Serializer::DeserializeDataViewTreeCtrlItems(YAML::Emitter &out, wxDataViewItem item) const
{

    return false;
}
