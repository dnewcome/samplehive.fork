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

#include <wx/brush.h>
#include <wx/dcclient.h>
#include <wx/defs.h>
#include <wx/dcmemory.h>
#include <wx/filefn.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/pen.h>

#include <sndfile.hh>

#include "WaveformViewer.hpp"
#include "Database.hpp"
#include "SettingsDialog.hpp"
#include "Serialize.hpp"
#include "Tags.hpp"
#include "SH_Event.hpp"

WaveformViewer::WaveformViewer(wxWindow* parentFrame, wxWindow* window, wxDataViewListCtrl& library,
                               wxMediaCtrl& mediaCtrl, wxTimer& timer, wxInfoBar& infoBar,
                               const std::string& configFilepath, const std::string& databaseFilepath)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE),
      m_ParentFrame(parentFrame), m_Window(window), m_Library(library), m_InfoBar(infoBar), m_MediaCtrl(mediaCtrl),
      m_Timer(timer), m_ConfigFilepath(configFilepath), m_DatabaseFilepath(databaseFilepath)
{
    this->SetDoubleBuffered(true);

    Bind(wxEVT_PAINT, &WaveformViewer::OnPaint, this);
    Bind(wxEVT_MOTION, &WaveformViewer::OnMouseMotion, this);
    Bind(wxEVT_LEFT_DOWN, &WaveformViewer::OnMouseLeftButtonDown, this);
    Bind(wxEVT_LEFT_UP, &WaveformViewer::OnMouseLeftButtonUp, this);
    // Bind(wxEVT_KEY_DOWN, &WaveformViewer::OnControlKeyDown, this);
    Bind(wxEVT_KEY_UP, &WaveformViewer::OnControlKeyUp, this);
}

WaveformViewer::~WaveformViewer()
{

}

void WaveformViewer::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    const wxSize& size = m_Window->GetClientSize();

    if (!m_WaveformBitmap.IsOk()
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

    // Draw selection range
    if (bSelectRange)
    {
        wxRect rect(m_CurrentPoint, m_AnchorPoint);

        dc.SetPen(wxPen(wxColour(200, 200, 200), 2, wxPENSTYLE_SOLID));
        dc.SetBrush(wxBrush(wxColour(200, 200, 200, 80), wxBRUSHSTYLE_SOLID));
        dc.DrawRectangle(rect);
    }

    // Draw selected area
    if (!bSelectRange && bDrawSelectedArea && !bBitmapDirty)
    {
        dc.SetPen(wxPen(wxColour(200, 200, 200, 255), 4, wxPENSTYLE_SOLID));
        dc.SetBrush(wxBrush(wxColour(200, 200, 200, 80), wxBRUSHSTYLE_SOLID));
        dc.DrawRectangle(wxRect(m_AnchorPoint.x, -2, m_CurrentPoint.x - m_AnchorPoint.x, this->GetSize().GetHeight() + 5));

        bAreaSelected = true;
        SendLoopPoints();
    }
    else
        bAreaSelected = false;
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
    wxLogDebug("Sample length: %d", length);

    double position = m_MediaCtrl.Tell();
    wxLogDebug("Current Sample Position: %f", position);

    m_Timer.Start(5, wxTIMER_CONTINUOUS);

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    wxLogDebug("Drawing playhead at: %f", line_pos);

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

    mdc.SetPen(wxPen(wxColour(m_WaveformColour), 2, wxPENSTYLE_SOLID));

    wxLogDebug("Drawing bitmap..");

    for (int i = 0; i < waveform.size() - 1; i++)
    {
        mdc.DrawLine((display_width * i) / waveform.size(), waveform[i] + display_height / 2.0f,
                     (display_width * i) / waveform.size(), (waveform[i] / samples_per_pixel) + display_height / 2.0f);
    }

    wxLogDebug("Done drawing bitmap..");
}

void WaveformViewer::OnControlKeyDown(wxKeyEvent &event)
{
    switch (event.GetKeyCode())
    {
        case WXK_CONTROL:
            SetCursor(wxCURSOR_IBEAM);
            break;
        default:
            SetCursor(wxCURSOR_ARROW);
            break;
    }

    event.Skip();
}

void WaveformViewer::OnControlKeyUp(wxKeyEvent &event)
{
    switch (event.GetKeyCode())
    {
        case WXK_CONTROL:
            if (bSelectRange)
            {
                SetCursor(wxCURSOR_ARROW);
                bSelectRange = false;
                bDrawSelectedArea = false;
                ReleaseMouse();
                return;
            }
            break;
        default:
            break;
    }

    event.Skip();
}

void WaveformViewer::OnMouseMotion(wxMouseEvent& event)
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

    if (abs(pos.x - line_pos) <= 5 && pos.y <= 5)
    {
        SetCursor(wxCursor(wxCURSOR_HAND));
        wxLogDebug("Cursor on playhead..");
    }
    else if (bSelectRange)
    {
        m_CurrentPoint = wxPoint(pos.x , pos.y);

        wxLogDebug("CTRL pressed, pressing LMB will draw selection range at %d, %d", pos.x, pos.y);
    }
    else
        return;
}

void WaveformViewer::OnMouseLeftButtonDown(wxMouseEvent& event)
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
        SetCursor(wxCURSOR_CLOSED_HAND);
        CaptureMouse();

        wxLogDebug("Mouse Captured playhead..");
    }
    else if (event.ControlDown())
    {
        wxLogDebug("LMB pressed");

        SetCursor(wxCURSOR_CLOSED_HAND);
        CaptureMouse();

        bSelectRange = true;

        m_AnchorPoint = wxPoint(pos.x, pos.y);
        m_CurrentPoint = m_AnchorPoint;
    }
    else
    {
        SetCursor(wxCURSOR_ARROW);
        return;
    }

    event.Skip();
}

void WaveformViewer::OnMouseLeftButtonUp(wxMouseEvent& event)
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

    if (!wxWindow::HasCapture())
    {
        wxLogDebug("Window doesn't have capture skipping..");
        return;
    }

    if (bSelectRange)
    {
        wxLogDebug("LMB released");

        m_CurrentPoint = wxPoint(pos.x, pos.y);

        ReleaseMouse();
        SetCursor(wxCURSOR_ARROW);

        bSelectRange = false;

        if (!bSelectRange)
            bDrawSelectedArea = true;
    }
    else
    {
        ReleaseMouse();
        SetCursor(wxCURSOR_ARROW);

        m_MediaCtrl.Seek(seek_to, wxFromStart);
        SendStatusBarStatus(wxString::Format(_("Now playing: %s"), selected), 1);
        m_MediaCtrl.Play();
    }
}

void WaveformViewer::ResetDC()
{
    bBitmapDirty = true;
    bSelectRange = false;
    bDrawSelectedArea = false;

    Refresh();
}

void WaveformViewer::SendLoopPoints()
{
    wxLogDebug("%s Called", __FUNCTION__);

    SampleHive::SH_LoopPointsEvent event(SampleHive::SH_EVT_LOOP_POINTS_UPDATED, this->GetId());
    event.SetEventObject(this);

    Database db(m_InfoBar);

    int selected_row = m_Library.GetSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = m_Library.GetTextValue(selected_row, 1);
    std::string path = db.GetSamplePathByFilename(m_DatabaseFilepath, selected.BeforeLast('.').ToStdString());

    Tags tags(path);

    int length = tags.GetAudioInfo().length;

    int panel_width = this->GetSize().GetWidth();

    int a = m_AnchorPoint.x, b = m_CurrentPoint.x;

    double loopA = ((double)a / panel_width) * length;
    double loopB = ((double)b / panel_width) * length;

    event.SetLoopPoints({ loopA, loopB });

    HandleWindowEvent(event);

    wxLogDebug("%s processed event, sending loop points..", __FUNCTION__);
}

void WaveformViewer::SendStatusBarStatus(const wxString& msg, int section)
{
    SampleHive::SH_SetStatusBarMessageEvent event(SampleHive::SH_EVT_STATUSBAR_MESSAGE_UPDATED, this->GetId());
    event.SetEventObject(this);

    event.SetMessageAndSection({ msg, section });

    HandleWindowEvent(event);
}
