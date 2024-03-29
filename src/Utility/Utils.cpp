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

#include <cmath>

#include "Database/Database.hpp"
#include "Utility/HiveData.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Signal.hpp"
#include "Utility/Tags.hpp"
#include "Utility/Utils.hpp"

#include <wx/dir.h>
#include <wx/gdicmn.h>
#include <wx/progdlg.h>
#include <wx/string.h>

#include <aubio/aubio.h>

namespace SampleHive {

    SampleHive::cUtils::FileInfo SampleHive::cUtils::GetFilenamePathAndExtension(const wxString& selected,
                                                                                 bool checkExtension, bool doGetFilename)
    {
        wxString path;
        std::string extension, filename;

        path = GetSamplePath(selected);

        if (checkExtension)
            extension = path.AfterLast('/').AfterLast('.').ToStdString();

        if (doGetFilename)
            filename = path.AfterLast('/').BeforeLast('.').ToStdString();

        return { path, extension, filename };
    }

    void SampleHive::cUtils::AddSamples(wxArrayString& files, wxWindow* parent)
    {
        SampleHive::cSerializer serializer;
        cDatabase db;

        wxBusyCursor busy_cursor;
        wxWindowDisabler window_disabler;

        wxProgressDialog* progressDialog = new wxProgressDialog(_("Adding files.."),
                                                                _("Adding files, please wait..."),
                                                                static_cast<int>(files.size()), parent,
                                                                wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_CAN_ABORT |
                                                                wxPD_AUTO_HIDE);
        progressDialog->CenterOnParent(wxBOTH);

        std::vector<Sample> sample_array;

        std::string path;
        std::string artist;
        std::string filename_with_extension;
        std::string filename_without_extension;
        std::string extension;
        std::string filename;

        float bpm = 0.0f;
        float key = 0.0f;

        //Check All Files At Once
        wxArrayString sorted_files;

        if (!serializer.DeserializeDemoMode())
        {
            sorted_files = db.CheckDuplicates(files);
            files = sorted_files;
        }

        if (files.size() < 1)
        {
            progressDialog->Destroy();
            return;
        }

        progressDialog->SetRange(files.size());

        for (unsigned int i = 0; i < files.size(); i++)
        {
            progressDialog->Update(i, wxString::Format(_("Getting Data For %s"), files[i].AfterLast('/')));

            if (progressDialog->WasCancelled())
            {
                progressDialog->Destroy();
                return;
            }

            path = files[i].ToStdString();
            filename_with_extension = files[i].AfterLast('/').ToStdString();
            filename_without_extension = files[i].AfterLast('/').BeforeLast('.').ToStdString();
            extension = files[i].AfterLast('.').ToStdString();

            filename = serializer.DeserializeShowFileExtension() ?
                filename_with_extension : filename_without_extension;

            bpm = GetBPM(path);

            Sample sample;

            sample.SetPath(path);
            sample.SetFilename(filename_without_extension);
            sample.SetFileExtension(extension);

            cTags tags(path);

            artist = tags.GetAudioInfo().artist.ToStdString();

            sample.SetSamplePack(artist);
            sample.SetChannels(tags.GetAudioInfo().channels);
            sample.SetBPM(static_cast<int>(bpm));
            sample.SetLength(tags.GetAudioInfo().length);
            sample.SetSampleRate(tags.GetAudioInfo().sample_rate);
            sample.SetBitrate(tags.GetAudioInfo().bitrate);

            wxString length = CalculateAndGetISOStandardTime(sample.GetLength());
            wxString bpm_str = GetBPMString(sample.GetBPM());

            wxVector<wxVariant> data;

            wxVariant icon = wxVariant(wxBitmap(ICON_STAR_EMPTY_16px, wxBITMAP_TYPE_PNG));

            if (tags.IsFileValid())
            {
                data.clear();
                data.push_back(icon);
                data.push_back(filename);
                data.push_back(sample.GetSamplePack());
                data.push_back("");
                data.push_back(wxString::Format("%d", sample.GetChannels()));
                data.push_back(bpm_str);
                data.push_back(length);
                data.push_back(wxString::Format("%d", sample.GetSampleRate()));
                data.push_back(wxString::Format("%d", sample.GetBitrate()));
                data.push_back(path);

                SH_LOG_INFO("Adding file: {}, Extension: {}", sample.GetFilename(), sample.GetFileExtension());

                SampleHive::cHiveData::Get().ListCtrlAppendItem(data);

                sample_array.push_back(sample);
            }
            else
            {
                wxString msg = wxString::Format(_("Error! Cannot open %s, Invalid file type."),
                                                filename_with_extension);

                SampleHive::cSignal::SendInfoBarMessage(msg, wxICON_ERROR, *parent);
            }
        }

        progressDialog->Pulse(_("Updating Database.."), NULL);

        db.InsertIntoSamples(sample_array);

        progressDialog->Destroy();
    }

    void cUtils::OnAutoImportDir(const wxString& pathToDirectory, wxWindow* parent)
    {
        SH_LOG_DEBUG("Start Importing Samples");

        wxBusyCursor busy_cursor;
        wxWindowDisabler window_disabler;

        wxString filepath;
        wxArrayString filepath_array;

        size_t number_of_files = wxDir::GetAllFiles(pathToDirectory, &filepath_array, wxEmptyString, wxDIR_DEFAULT);

        wxProgressDialog* progressDialog = new wxProgressDialog(_("Adding files.."),
                                                                _("Adding files, please wait..."),
                                                                static_cast<int>(number_of_files), parent,
                                                                wxPD_APP_MODAL | wxPD_SMOOTH |
                                                                wxPD_CAN_ABORT | wxPD_AUTO_HIDE);

        progressDialog->CenterOnParent(wxBOTH);

        for (size_t i = 0; i < number_of_files; i++)
        {
            filepath = filepath_array[i];

            if (wxFileExists(filepath))
            {
                filepath_array.push_back(filepath);
            }
            else if (wxDirExists(filepath))
            {
                wxDir::GetAllFiles(filepath, &filepath_array);
            }

            progressDialog->Pulse(_("Reading Samples"), NULL);
        }

        progressDialog->Destroy();

        AddSamples(filepath_array, parent);

        SH_LOG_DEBUG("Done Importing Samples");
    }

    std::string cUtils::GetSamplePath(const wxString& name)
    {
        SampleHive::cSerializer serializer;
        cDatabase db;

        wxString sample_path;

        if (m_PathCache.find(name.ToStdString()) != m_PathCache.end())
            return m_PathCache[name.ToStdString()];
        else
        {
            sample_path = serializer.DeserializeShowFileExtension() ?
                db.GetSamplePathByFilename(name.BeforeLast('.').ToStdString()) :
                db.GetSamplePathByFilename(name.ToStdString());

            m_PathCache[name.ToStdString()] = sample_path;
        }

        return sample_path.ToStdString();
    }

    wxString cUtils::CalculateAndGetISOStandardTime(wxLongLong length)
    {
        const int min_digits = 2;
        const size_t max_digits = 2;
        wxString iso_length;

        int min = static_cast<int>((length / 60000).GetValue());
        int sec = static_cast<int>(((length % 60000) / 1000).GetValue());
        int ms = static_cast<int>((length % 1000).GetValue());

        iso_length = wxString::Format("%s:%s.%s",
                                      wxString::Format("%0*i", min_digits, min).Left(max_digits),
                                      wxString::Format("%0*i", min_digits, sec).Left(max_digits),
                                      wxString::Format("%0*i", min_digits + 1, ms).Left(max_digits + 1));

        return iso_length;
    }

    wxString cUtils::GetBPMString(float bpm)
    {
        return wxString::Format("%d", static_cast<int>(bpm));
    }

    float cUtils::GetBPM(const std::string& path)
    {
        uint_t buff_size = 1024, hop_size = buff_size / 2, frames = 0, samplerate = 0, read = 0;
        aubio_tempo_t* tempo;
        fvec_t* in, *out;
        aubio_source_t* source = new_aubio_source(path.c_str(), samplerate, hop_size);

        float bpm = 0.0f;

        if (!source)
            return 0.0f;
        else
        {
            try
            {
                if (samplerate == 0)
                    samplerate = aubio_source_get_samplerate(source);

                tempo = new_aubio_tempo("default", buff_size, hop_size, samplerate);

                if (!tempo)
                    return 0.0f;

                in = new_fvec(hop_size);
                out = new_fvec(1);

                do
                {
                    // put some fresh data in input vector
                    aubio_source_do(source, in, &read);

                    // execute tempo
                    aubio_tempo_do(tempo, in, out);

                    // do something with the beats
                    // if (out->data[0] != 0)
                    // {
                    //     SH_LOG_DEBUG("Track: {} Beat at {}s, {}s, frame {}, {} bpm with confidence {}",
                    //                  path, aubio_tempo_get_last_ms(tempo), aubio_tempo_get_last_s(tempo),
                    //                  aubio_tempo_get_last(tempo), aubio_tempo_get_bpm(tempo),
                    //                  aubio_tempo_get_confidence(tempo));
                    // }

                    frames += read;
                    bpm = aubio_tempo_get_bpm(tempo);
                }
                while (read == hop_size);

                // clean up memory
                del_aubio_tempo(tempo);
                del_fvec(in);
                del_fvec(out);
                del_aubio_source(source);
                aubio_cleanup();
            }
            catch (std::exception& e)
            {
                SH_LOG_ERROR("Aubio Error! {}", e.what());
            }
        }

        return bpm;
    }
}
