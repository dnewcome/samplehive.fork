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
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"

#include <fstream>
#include <sstream>

#include <wx/colour.h>
#include <wx/filename.h>

#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/node/parse.h>

Serializer::Serializer()
{
    std::ifstream ifstrm(static_cast<std::string>(CONFIG_FILEPATH));

    wxFont font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    std::string system_font_face = font.GetFaceName().ToStdString();
    int system_font_size = font.GetPointSize();

    wxColour colour = "#FE9647";

    std::string dir = wxGetHomeDir().ToStdString();

    // Initialize the logger
    // SampleHive::Log::InitLogger("Serializer");

    if (!ifstrm)
    {
        SH_LOG_INFO("Genrating configuration file..");

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
        m_Emitter << YAML::Key << "Volume" << YAML::Value << 100;
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
        m_Emitter << YAML::Key << "RecursiveImport" << YAML::Value << false;
        m_Emitter << YAML::Key << "ShowFileExtension" << YAML::Value << true;
        m_Emitter << YAML::EndMap << YAML::Newline;

        m_Emitter << YAML::EndMap;

        std::ofstream ofstrm(CONFIG_FILEPATH);
        ofstrm << m_Emitter.c_str();

        SH_LOG_INFO("Generated {} successfully!", CONFIG_FILEPATH);
    }
}

Serializer::~Serializer()
{

}

void Serializer::SerializeWinSize(int w, int h)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto win = config["Window"])
        {
            win["Width"] = w;
            win["Height"] = h;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot store window size values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

WindowSize Serializer::DeserializeWinSize() const
{
    int width = 800, height = 600;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

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
        SH_LOG_ERROR(ex.what());
    }

    SH_LOG_INFO("Window size: {}, {}", width, height);

    return { width, height };
}

void Serializer::SerializeShowMenuAndStatusBar(std::string key, bool value)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto bar = config["Window"])
        {
            if (key == "menubar")
                 bar["ShowMenuBar"] = value;

            if (key == "statusbar")
                 bar["ShowStatusBar"] = value;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
            SH_LOG_ERROR("Error! Cannot store show bar values.");
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

bool Serializer::DeserializeShowMenuAndStatusBar(std::string key) const
{
    bool show = false;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto bar = config["Window"])
        {
            if (key == "menubar")
                 show = bar["ShowMenuBar"].as<bool>();

            if (key == "statusbar")
                 show = bar["ShowStatusBar"].as<bool>();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot fetch show bar values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    return show;
}

void Serializer::SerializeMediaOptions(std::string key, bool value)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto media = config["Media"])
        {
            if (key == "autoplay")
                media["Autoplay"] = value;

            if (key == "loop")
                media["Loop"] = value;

            if (key == "muted")
                media["Muted"] = value;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
            SH_LOG_ERROR("Error! Cannot store media values.");
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

bool Serializer::DeserializeMediaOptions(std::string key) const
{
    bool control = false;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

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
            SH_LOG_ERROR("Error! Cannot fetch media values.");
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    SH_LOG_INFO("{}: {}", key, control ? "enabled" : "disabled");

    return control;
}

void Serializer::SerializeMediaVolume(int volume)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto media = config["Media"])
        {
            media["Volume"] = volume;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
            SH_LOG_ERROR("Error! Cannot store volume values.");
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

int Serializer::DeserializeMediaVolume() const
{
    int volume = 0;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto media = config["Media"])
            volume = media["Volume"].as<int>();
        else
            SH_LOG_ERROR("Error! Cannot fetch volume values.");
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    SH_LOG_INFO("Volume: {}", volume);

    return volume;
}

void Serializer::SerializeFontSettings(wxFont& font)
{
    YAML::Emitter out;

    std::string font_face = font.GetFaceName().ToStdString();
    int font_size = font.GetPointSize();

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        auto display = config["Display"];

        if (auto fontSetting = display["Font"])
        {
            fontSetting["Family"] = font_face;
            fontSetting["Size"] = font_size;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot store font values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

wxFont Serializer::DeserializeFontSettings() const
{
    wxFont font;

    wxString face;
    int size = 0 ;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

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
            SH_LOG_ERROR("Error! Cannot fetch font values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    return font;
}

void Serializer::SerializeWaveformColour(wxColour& colour)
{
    YAML::Emitter out;

    std::string colour_string = colour.GetAsString(wxC2S_HTML_SYNTAX).ToStdString();

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        auto display = config["Display"];

        if (auto waveform = display["Waveform"])
        {
            waveform["Colour"] = colour_string;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot store waveform colour.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

wxColour Serializer::DeserializeWaveformColour() const
{
    std::string colour;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        auto display = config["Display"];

        if (auto waveform = display["Waveform"])
        {
            colour = waveform["Colour"].as<std::string>();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot fetch waveform colour.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    return static_cast<wxString>(colour);
}

void Serializer::SerializeAutoImport(bool autoImport, const std::string& importDir)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto autoImportInfo = config["Collection"])
        {
            autoImportInfo["AutoImport"] = autoImport;
            autoImportInfo["Directory"] = importDir;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot store import dir values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

ImportDirInfo Serializer::DeserializeAutoImport() const
{
    wxString dir;
    bool auto_import = false;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto autoImportInfo = config["Collection"])
        {
            auto_import = autoImportInfo["AutoImport"].as<bool>();
            dir = autoImportInfo["Directory"].as<std::string>();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot fetch import dir values.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    return { auto_import, dir };
}

void Serializer::SerializeFollowSymLink(bool followSymLinks)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto followSymLinks = config["Collection"])
        {
            followSymLinks["FollowSymLink"] = followSymLinks;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot store follow symbolic links value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

bool Serializer::DeserializeFollowSymLink() const
{
    bool follow_sym_links = false;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto followSymLinks = config["Collection"])
        {
            follow_sym_links = followSymLinks["FollowSymLink"].as<bool>();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot fetch follow symbolic links value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    return follow_sym_links;
}

void Serializer::SerializeRecursiveImport(bool recursiveImport)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto recursive = config["Collection"])
        {
            recursive["RecursiveImport"] = recursiveImport;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot store recursive import value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

bool Serializer::DeserializeRecursiveImport() const
{
    bool recursive_import = false;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto recursive = config["Collection"])
        {
            recursive_import = recursive["RecursiveImport"].as<bool>();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot fetch recursive import value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    return recursive_import;
}

void Serializer::SerializeShowFileExtension(bool showExtension)
{
    YAML::Emitter out;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto fileExtensionInfo = config["Collection"])
        {
            fileExtensionInfo["ShowFileExtension"] = showExtension;

            out << config;

            std::ofstream ofstrm(static_cast<std::string>(CONFIG_FILEPATH));
            ofstrm << out.c_str();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot store show file extension value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }
}

bool Serializer::DeserializeShowFileExtension() const
{
    bool show_extension = false;

    try
    {
        YAML::Node config = YAML::LoadFile(static_cast<std::string>(CONFIG_FILEPATH));

        if (auto fileExtensionInfo = config["Collection"])
        {
            show_extension = fileExtensionInfo["ShowFileExtension"].as<bool>();
        }
        else
        {
            SH_LOG_ERROR("Error! Cannot fetch show file extension value.");
        }
    }
    catch(const YAML::ParserException& ex)
    {
        SH_LOG_ERROR(ex.what());
    }

    return show_extension;
}
