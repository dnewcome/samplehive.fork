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

#include <vector>

#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/gdicmn.h>
#include <wx/log.h>

#include <sndfile.hh>

#include "WaveformViewer.hpp"
#include "Database.hpp"
#include "SettingsDialog.hpp"
#include "Serialize.hpp"
#include "Tags.hpp"
#include "wx/filefn.h"

WaveformViewer::WaveformViewer(wxWindow* parentFrame, wxWindow* window, wxDataViewListCtrl& library, wxMediaCtrl& mediaCtrl,
                               wxTimer& timer, wxInfoBar& infoBar, const std::string& configFilepath, const std::string& databaseFilepath)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE),
      m_ParentFrame(parentFrame), m_Window(window), m_Library(library), m_MediaCtrl(mediaCtrl), m_Timer(timer),
      m_InfoBar(infoBar), m_ConfigFilepath(configFilepath), m_DatabaseFilepath(databaseFilepath)
{
    Bind(wxEVT_PAINT, &WaveformViewer::OnPaint, this);
    Bind(wxEVT_MOTION, &WaveformViewer::OnHoverPlayhead, this);
    Bind(wxEVT_LEFT_DOWN, &WaveformViewer::OnGrabPlayhead, this);
    Bind(wxEVT_LEFT_UP, &WaveformViewer::OnReleasePlayhead, this);
}

WaveformViewer::~WaveformViewer()
{

}

void WaveformViewer::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    const wxSize& size = m_Window->GetClientSize();

    if(!m_WaveformBitmap.IsOk()
       || m_WaveformBitmap.GetWidth() != size.x
       || m_WaveformBitmap.GetHeight() != size.y
       || bBitmapDirty)
    {
        wxLogDebug("Updating waveform bitmap..");

        m_WaveformBitmap = wxBitmap(wxImage(size.x, size.y), 32);
        // m_WaveformBitmap.Create(size.x, size.y, 32);

        UpdateWaveformBitmap();

        bBitmapDirty = false;
    }

    dc.DrawBitmap(m_WaveformBitmap, 0, 0, false);
    // m_WaveformBitmap.SaveFile("waveform.png", wxBITMAP_TYPE_PNG);

    RenderPlayhead(dc);
}

void WaveformViewer::RenderPlayhead(wxDC& dc)
{
    Database db(m_InfoBar);

    int selected_row = m_Library.GetSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = m_Library.GetTextValue(selected_row, 1);
    std::string path = db.GetSamplePathByFilename(m_DatabaseFilepath, selected.BeforeLast('.').ToStdString());

    Tags tags(path);

    int length = tags.GetAudioInfo().length;
    // wxLogDebug("Sample length: %d", length);

    double position = m_MediaCtrl.Tell();
    // wxLogDebug("Current Sample Position: %f", position);

    m_Timer.Start(5, wxTIMER_CONTINUOUS);

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    // wxLogDebug("Drawing playhead at: %f", line_pos);

    m_PlayheadColour = wxColor(255, 0, 0, 255);

    // Draw the triangle
    dc.SetPen(wxPen(m_PlayheadColour, 8, wxPENSTYLE_SOLID));
    dc.DrawLine(line_pos - 5, this->GetSize().GetHeight() - (this->GetSize().GetHeight() - 1),
                line_pos, this->GetSize().GetHeight() - (this->GetSize().GetHeight() - 1) + 5);
    dc.DrawLine(line_pos + 5, this->GetSize().GetHeight() - (this->GetSize().GetHeight() - 1),
                line_pos, this->GetSize().GetHeight() - (this->GetSize().GetHeight()- 1) + 5);

    // Draw the line
    dc.SetPen(wxPen(m_PlayheadColour, 2, wxPENSTYLE_SOLID));
    dc.DrawLine(line_pos, this->GetSize().GetHeight() - (this->GetSize().GetHeight() - 1),
                line_pos, this->GetSize().GetHeight() - 1);
}

void WaveformViewer::UpdateWaveformBitmap()
{
    Database db(m_InfoBar);
    Settings settings(m_ParentFrame, m_ConfigFilepath, m_DatabaseFilepath);
    Serializer serializer(m_ConfigFilepath);

    int selected_row = m_Library.GetSelectedRow();

    if (selected_row < 0)
        return;

    wxString selection = m_Library.GetTextValue(selected_row, 1);

    wxString filepath_with_extension = db.GetSamplePathByFilename(m_DatabaseFilepath, selection.BeforeLast('.').ToStdString());
    wxString filepath_without_extension = db.GetSamplePathByFilename(m_DatabaseFilepath, selection.ToStdString());

    std::string extension = settings.ShouldShowFileExtension() ?
        db.GetSampleFileExtension(m_DatabaseFilepath, selection.ToStdString()) :
        db.GetSampleFileExtension(m_DatabaseFilepath, selection.BeforeLast('.').ToStdString());

    wxString path = selection.Contains(wxString::Format(".%s", extension)) ?
        filepath_with_extension : filepath_without_extension;

    SndfileHandle snd_file(path);

    int channels = snd_file.channels();
    double sample_rate = snd_file.samplerate();
    sf_count_t frames = snd_file.frames();

    std::vector<float> sample;
    sample.resize(frames * channels);

    std::vector<float> waveform;

    snd_file.read(&sample.at(0), frames * channels);

    float display_width = this->GetSize().GetWidth();
    float display_height = this->GetSize().GetHeight();

    double max_value;
    snd_file.command(SFC_CALC_NORM_SIGNAL_MAX, &max_value, sizeof(max_value));

    float normalized_value = max_value > 1.0f ? 1.0f : 1.0f / max_value;

    float samples_per_pixel = static_cast<float>(frames) / (float)display_width;

    if (channels == 2)
    {
        for (int i = 0, j = 0 ; i < frames; i++)
        {
            float sum = (((sample[j] + sample[j + 1]) * 0.5f) * normalized_value) * float(display_height / 2.0f);
            waveform.push_back(sum);
            j += 2;
        }
    }
    else
    {
        waveform.resize(frames);

        for (int i = 0; i < frames; i++)
        {
            waveform[i] = (sample[i] * normalized_value) * float(display_height / 2.0f);
        }
    }

    // Draw code
    wxMemoryDC mdc(m_WaveformBitmap);

    mdc.SetBrush(*wxBLACK);
    mdc.Clear();

    m_WaveformColour = serializer.DeserializeWaveformColour();

    wxLogDebug("Changing waveform colour to: %s", m_WaveformColour.GetAsString());
    mdc.SetPen(wxPen(wxColour(m_WaveformColour), 2, wxPENSTYLE_SOLID));

    wxLogDebug("Drawing bitmap..");

    for(int i = 0; i < waveform.size() - 1; i++)
    {
        mdc.DrawLine((display_width * i) / waveform.size(), waveform[i] + display_height / 2.0f,
                     (display_width * i) / waveform.size(), (waveform[i] / samples_per_pixel) + display_height / 2.0f);
    }

    wxLogDebug("Done drawing bitmap..");
}

void WaveformViewer::OnHoverPlayhead(wxMouseEvent& event)
{
    Database db(m_InfoBar);

    int selected_row = m_Library.GetSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = m_Library.GetTextValue(selected_row, 1);
    std::string path = db.GetSamplePathByFilename(m_DatabaseFilepath, selected.BeforeLast('.').ToStdString());

    Tags tags(path);

    int length = tags.GetAudioInfo().length;
    // wxLogDebug("Sample length: %d", length);

    double position = m_MediaCtrl.Tell();
    // wxLogDebug("Current Sample Position: %f", position);

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    wxPoint pos = event.GetPosition();
    // wxLogDebug("PosX: %d", pos.x);

    double seek_to = ((double)pos.x / panel_width) * length;
    // wxLogDebug("Seeking to: %f", seek_to);

    if (abs(pos.x - line_pos) <= 5 && pos.y <= 5)
    {
        SetCursor(wxCursor(wxCURSOR_CLOSED_HAND));
        wxLogDebug("Cursor on playhead..");
    }
    else
    {
        SetCursor(wxCURSOR_ARROW);
        return;
    }

    wxLogDebug("Mouse at: '(%d, %d)'", pos.x, pos.y);
}

void WaveformViewer::OnGrabPlayhead(wxMouseEvent& event)
{
    Database db(m_InfoBar);
    int selected_row = m_Library.GetSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = m_Library.GetTextValue(selected_row, 1);
    std::string path = db.GetSamplePathByFilename(m_DatabaseFilepath, selected.BeforeLast('.').ToStdString());

    Tags tags(path);

    int length = tags.GetAudioInfo().length;

    double position = m_MediaCtrl.Tell();

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    wxPoint pos = event.GetPosition();

    if (abs(pos.x - line_pos) <= 5 && pos.y <= 5)
    {
        wxWindow::CaptureMouse();

        SetCursor(wxCURSOR_CLOSED_HAND);
        wxLogDebug("Mouse Captured playhead..");
    }
    else
    {
        SetCursor(wxCURSOR_ARROW);
        return;
    }
}

void WaveformViewer::OnReleasePlayhead(wxMouseEvent& event)
{
    Database db(m_InfoBar);

    int selected_row = m_Library.GetSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = m_Library.GetTextValue(selected_row, 1);
    std::string path = db.GetSamplePathByFilename(m_DatabaseFilepath, selected.BeforeLast('.').ToStdString());

    Tags tags(path);

    int length = tags.GetAudioInfo().length;

    double position = m_MediaCtrl.Tell();

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    wxPoint pos = event.GetPosition();

    double seek_to = ((double)pos.x / panel_width) * length;

    wxWindow::ReleaseMouse();
    SetCursor(wxCURSOR_ARROW);
    wxLogDebug("Mouse released playhead..");

    m_MediaCtrl.Seek(seek_to, wxFromCurrent);
    // m_MediaCtrl.Play();
}
