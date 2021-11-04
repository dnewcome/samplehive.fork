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

#include "Utility/Serialize.hpp"

#include <fstream>
#include <sstream>

#include <wx/colour.h>
#include <wx/log.h>
#include <wx/filename.h>

#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/node/parse.h>

Serializer::Serializer(const std::string& filepath)
    : m_Filepath(filepath)
{
    std::ifstream ifstrm(m_Filepath);

    wxFont font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    std::string system_font_face = font.GetFaceName().ToStdString();
    int system_font_size = font.GetPointSize();

    wxColour colour = "#FE9647";

    std::string dir = wxGetHomeDir().ToStdString();

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
        m_Emitter << YAML::Key << "ShowMenuBar" << YAML::Value << true;
        m_Emitter << YAML::Key << "ShowStatusBar" << YAML::Value << true;
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
        m_Emitter << YAML::Key << "Waveform";
        m_Emitter << YAML::BeginMap;
        m_Emitter << YAML::Key << "Colour" << YAML::Value << colour.GetAsString().ToStdString();
        m_Emitter << YAML::EndMap;
        m_Emitter << YAML::EndMap << YAML::Newline;

        m_Emitter << YAML::Newline << YAML::Key << "Collection";
        m_Emitter << YAML::BeginMap;
        m_Emitter << YAML::Key << "AutoImport" << YAML::Value << false;
        m_Emitter << YAML::Key << "Directory" << YAML::Value << dir;
        m_Emitter << YAML::Key << "FollowSymLink" << YAML::Value << false;
        m_Emitter << YAML::Key << "ShowFileExtension" << YAML::Value << true;
        m_Emitter << YAML::EndMap << YAML::Newline;

        m_Emitter << YAML::EndMap;

        std::ofstream ofstrm(m_Filepath);
        ofstrm << m_Emitter.c_str();

        wxLogDebug("Generated %s successfully!", m_Filepath);
    }
}

Serializer::~Serializer()
{

}

WindowSize Serializer::DeserializeWinSize() const
{
    int width = 800, height = 600;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (!config["Window"])
        {
            return { width, height };
        }

        if (auto win = config["Window"])
        {
            height = win["Height"].as<int>();
            width = win["Width"].as<int>();
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    wxLogDebug("Window size: %d, %d", width, height);

    return { width, height };
}

void Serializer::SerializeShowMenuAndStatusBar(std::string key, bool value)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto bar = config["Window"])
        {
            if (key == "menubar")
                 bar["ShowMenuBar"] = value;

            if (key == "statusbar")
                 bar["ShowStatusBar"] = value;

            out << config;

            std::ofstream ofstrm(m_Filepath);
            ofstrm << out.c_str();
        }
        else
            wxLogDebug("Error! Cannot store show bar values.");
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

bool Serializer::DeserializeShowMenuAndStatusBar(std::string key) const
{
    bool show = false;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto bar = config["Window"])
        {
            if (key == "menubar")
                 show = bar["ShowMenuBar"].as<bool>();

            if (key == "statusbar")
                 show = bar["ShowStatusBar"].as<bool>();
        }
        else
        {
            wxLogDebug("Error! Cannot fetch show bar values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return show;
}

void Serializer::SerializeBrowserControls(std::string key, bool value)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto media = config["Media"])
        {
            if (key == "autoplay")
                media["Autoplay"] = value;

            if (key == "loop")
                media["Loop"] = value;

            if (key == "muted")
                media["Muted"] = value;

            out << config;

            std::ofstream ofstrm(m_Filepath);
            ofstrm << out.c_str();
        }
        else
            wxLogDebug("Error! Cannot store media values.");
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

bool Serializer::DeserializeBrowserControls(std::string key) const
{
    bool control = false;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto media = config["Media"])
        {
            if (key == "autoplay")
                control = media["Autoplay"].as<bool>();

            if (key == "loop")
                control = media["Loop"].as<bool>();

            if (key == "muted")
                control = media["Muted"].as<bool>();
        }
        else
            wxLogDebug("Error! Cannot fetch values.");
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    wxLogDebug("%s: %s", key, control ? "enabled" : "disabled");

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

wxFont Serializer::DeserializeDisplaySettings() const
{
    wxFont font;

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

            font.SetFaceName(face);
            font.SetPointSize(size);
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

    return font;
}

void Serializer::SerializeWaveformColour(wxColour& colour)
{
    YAML::Emitter out;

    std::string colour_string = colour.GetAsString(wxC2S_HTML_SYNTAX).ToStdString();

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        auto display = config["Display"];

        if (auto waveform = display["Waveform"])
        {
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

        auto display = config["Display"];

        if (auto waveform = display["Waveform"])
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

void Serializer::SerializeAutoImportSettings(bool autoImport, const std::string& importDir)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto autoImportInfo = config["Collection"])
        {
            autoImportInfo["AutoImport"] = autoImport;
            autoImportInfo["Directory"] = importDir;

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
    catch (const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return { auto_import, dir };
}

void Serializer::SerializeFollowSymLink(bool followSymLinks)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto followSymLinks = config["Collection"])
        {
            followSymLinks["FollowSymLinks"] = followSymLinks;

            out << config;

            std::ofstream ofstrm(m_Filepath);
            ofstrm << out.c_str();
        }
        else
        {
            wxLogDebug("Error! Cannot store follow symbolic links value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

bool Serializer::DeserializeFollowSymLink() const
{
    bool follow_sym_links = false;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto followSymLinks = config["Collection"])
        {
            follow_sym_links = followSymLinks["FollowSymLinks"].as<bool>();
        }
        else
        {
            wxLogDebug("Error! Cannot fetch follow symbolic links value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return follow_sym_links;
}

void Serializer::SerializeShowFileExtensionSetting(bool showExtension)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(m_Filepath);

        if (auto fileExtensionInfo = config["Collection"])
        {
            fileExtensionInfo["ShowFileExtension"] = showExtension;

            out << config;

            std::ofstream ofstrm(m_Filepath);
            ofstrm << out.c_str();
        }
        else
        {
            wxLogDebug("Error! Cannot store show file extension value.");
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
            wxLogDebug("Error! Cannot fetch show file extension value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return show_extension;
}
