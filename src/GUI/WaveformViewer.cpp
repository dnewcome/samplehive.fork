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

#include "GUI/WaveformViewer.hpp"
#include "Utility/HiveData.hpp"
#include "Utility/Log.hpp"
#include "Utility/Paths.hpp"
#include "Utility/Utils.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Event.hpp"
#include "Utility/Signal.hpp"
#include "Utility/Tags.hpp"

#include <exception>
#include <vector>

#include <wx/brush.h>
#include <wx/dcclient.h>
#include <wx/defs.h>
#include <wx/dcmemory.h>
#include <wx/filefn.h>
#include <wx/gdicmn.h>
#include <wx/pen.h>

#include <sndfile.h>
#include <sndfile.hh>

cWaveformViewer::cWaveformViewer(wxWindow* window, wxMediaCtrl& mediaCtrl)
    : wxPanel(window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE),
      m_Window(window), m_MediaCtrl(mediaCtrl)
{
    this->SetDoubleBuffered(true);

    Bind(wxEVT_PAINT, &cWaveformViewer::OnPaint, this);
    Bind(wxEVT_MOTION, &cWaveformViewer::OnMouseMotion, this);
    Bind(wxEVT_LEFT_DOWN, &cWaveformViewer::OnMouseLeftButtonDown, this);
    Bind(wxEVT_LEFT_UP, &cWaveformViewer::OnMouseLeftButtonUp, this);
    // Bind(wxEVT_KEY_DOWN, &cWaveformViewer::OnControlKeyDown, this);
    Bind(wxEVT_KEY_UP, &cWaveformViewer::OnControlKeyUp, this);

    m_Sizer = new wxBoxSizer(wxVERTICAL);

    this->SetSizer(m_Sizer);
    m_Sizer->Fit(this);
    m_Sizer->SetSizeHints(this);
    m_Sizer->Layout();
}

cWaveformViewer::~cWaveformViewer()
{

}

void cWaveformViewer::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    const wxSize& size = m_Window->GetClientSize();

    if (!m_WaveformBitmap.IsOk()
        || m_WaveformBitmap.GetWidth() != size.x
        || m_WaveformBitmap.GetHeight() != size.y
        || bBitmapDirty)
    {
        SH_LOG_INFO("Updating waveform bitmap..");

        m_WaveformBitmap = wxBitmap(wxImage(size.x, size.y), 32);

        UpdateWaveformBitmap();

        bBitmapDirty = false;
    }

    dc.DrawBitmap(m_WaveformBitmap, 0, 0, false);

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
        dc.DrawRectangle(wxRect(m_AnchorPoint.x, -2, m_CurrentPoint.x - m_AnchorPoint.x,
                                this->GetSize().GetHeight() + 5));

        bAreaSelected = true;
        // SendLoopPoints();
        SampleHive::cSignal::SendLoopPoints(CalculateLoopPoints(), *this);
    }
    else
        bAreaSelected = false;
}

void cWaveformViewer::RenderPlayhead(wxDC& dc)
{
    int selected_row = SampleHive::cHiveData::Get().GetListCtrlSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = SampleHive::cHiveData::Get().GetListCtrlTextValue(selected_row, 1);
    std::string path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selected).Path.ToStdString();

    SampleHive::cTags tags(path);

    int length = tags.GetAudioInfo().length;

    double position = m_MediaCtrl.Tell();

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    m_PlayheadColour = wxColor(255, 0, 0, 255);

    // Draw the triangle
    dc.SetPen(wxPen(m_PlayheadColour, 8, wxPENSTYLE_SOLID));
    dc.DrawLine(line_pos - 5, this->GetSize().GetHeight() - (this->GetSize().GetHeight() - 1),
                line_pos, this->GetSize().GetHeight() - (this->GetSize().GetHeight() - 1) + 5);
    dc.DrawLine(line_pos + 5, this->GetSize().GetHeight() - (this->GetSize().GetHeight() - 1),
                line_pos, this->GetSize().GetHeight() - (this->GetSize().GetHeight()- 1) + 5);

    // Draw the line
    dc.SetPen(wxPen(m_PlayheadColour, 2, wxPENSTYLE_SOLID));
    dc.DrawLine(line_pos, this->GetSize().GetHeight() - (this->GetSize().GetHeight() - 1), line_pos, this->GetSize().GetHeight() - 1);
}

void cWaveformViewer::UpdateWaveformBitmap()
{
    SampleHive::cSerializer serializer;

    int selected_row = SampleHive::cHiveData::Get().GetListCtrlSelectedRow();

    if (selected_row < 0)
        return;

    wxString selection = SampleHive::cHiveData::Get().GetListCtrlTextValue(selected_row, 1);

    wxString path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selection).Path;

    SndfileHandle snd_file(path.ToStdString().c_str());

    int channels = snd_file.channels();
    double sample_rate = snd_file.samplerate();
    sf_count_t frames = snd_file.frames();

    std::vector<float> sample;
    sample.resize(frames * channels);

    std::vector<float> waveform;

    try
    {
        // TODO, FIXME: Don't reload file on every window resize
        snd_file.read(&sample.at(0), frames * channels);

        float display_width = this->GetSize().GetWidth();
        float display_height = this->GetSize().GetHeight();

        SH_LOG_INFO("Calculating Waveform bars RMS..");

        float chunk_size = (float)(frames) / (float)display_width;
        int number_of_chunks = static_cast<int>(static_cast<float>(frames) / chunk_size);

        // Start with low non-zero value
        float normalize = 0.00001;

        for (int i = 0; i < number_of_chunks; i++)
        {
            double sum = 0, mono = 0;

            int start_point = static_cast<int>(i * chunk_size * channels);

            // Iterate on the chunk, get the square of sum of monos
            for (int j = 0; j < chunk_size; j++)
            {
                if (channels == 2)
                    mono = 0.5f * (sample[start_point + (2 * j)] + sample[start_point + (2 * j) + 1]);
                else
                    mono = sample[start_point + j];

                sum += mono * mono; // Square
            }

            sum /= chunk_size;      // Mean
            sum = pow(sum, 0.5);    // Root

            // We might bleed a bit on the end and get some near infs, dunno
            // what is causing astronomically big numbers from sample[]
            if ((sum < 200.0) && (sum > normalize))
                normalize = sum;

            waveform.push_back(sum);
        }

        // Actually normalize
        for (int i = 0; i < waveform.size(); i++)
            waveform[i] /= normalize;

        // Draw code
        wxMemoryDC mdc(m_WaveformBitmap);

        mdc.SetBackground(wxBrush(wxColour(0, 0, 0, 150), wxBRUSHSTYLE_SOLID));
        mdc.Clear();

        m_WaveformColour = serializer.DeserializeWaveformColour();

        mdc.SetPen(wxPen(wxColour(m_WaveformColour), 2, wxPENSTYLE_SOLID));

        SH_LOG_DEBUG("Drawing bitmap..");

        for (int i = 0; i < waveform.size() - 1; i++)
        {
            float half_display_height = static_cast<float>(display_height) / 2.0f;

            // X is percentage of i relative to waveform.size() multiplied by
            // the width, Y is the half height times the value up or down
            float X = display_width * ((float)i / waveform.size());
            float Y = waveform[i] * half_display_height;

            mdc.DrawLine(X, half_display_height + Y, X, half_display_height - Y);
        }

        SH_LOG_DEBUG("Done drawing bitmap..");
    }
    catch (std::exception& e)
    {
        SH_LOG_ERROR("Error! SNDFILE {}", e.what());
        SH_LOG_ERROR("Error! SNDFILE {}", sf_strerror(snd_file.rawHandle()));
    }
}

void cWaveformViewer::OnControlKeyDown(wxKeyEvent &event)
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

void cWaveformViewer::OnControlKeyUp(wxKeyEvent &event)
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

void cWaveformViewer::OnMouseMotion(wxMouseEvent& event)
{
    int selected_row = SampleHive::cHiveData::Get().GetListCtrlSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = SampleHive::cHiveData::Get().GetListCtrlTextValue(selected_row, 1);
    std::string path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selected).Path.ToStdString();

    SampleHive::cTags tags(path);

    int length = tags.GetAudioInfo().length;

    double position = m_MediaCtrl.Tell();

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    wxPoint pos = event.GetPosition();

    double seek_to = ((double)pos.x / panel_width) * length;

    if (abs(pos.x - line_pos) <= 5 && pos.y <= 5)
    {
        SetCursor(wxCursor(wxCURSOR_HAND));
        SH_LOG_DEBUG("Cursor on playhead..");
    }
    else if (bSelectRange)
    {
        m_CurrentPoint = wxPoint(pos.x , pos.y);

        Refresh();

        SH_LOG_INFO("CTRL pressed, pressing LMB will draw selection range at {}, {}", pos.x, pos.y);
    }
    else
        return;
}

void cWaveformViewer::OnMouseLeftButtonDown(wxMouseEvent& event)
{
    int selected_row = SampleHive::cHiveData::Get().GetListCtrlSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = SampleHive::cHiveData::Get().GetListCtrlTextValue(selected_row, 1);
    std::string path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selected).Path.ToStdString();

    SampleHive::cTags tags(path);

    int length = tags.GetAudioInfo().length;

    double position = m_MediaCtrl.Tell();

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    wxPoint pos = event.GetPosition();

    if (abs(pos.x - line_pos) <= 5 && pos.y <= 5)
    {
        SetCursor(wxCURSOR_CLOSED_HAND);
        CaptureMouse();

        SH_LOG_DEBUG("Mouse Captured playhead..");
    }
    else if (event.ControlDown())
    {
        SH_LOG_DEBUG("LMB pressed");

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

void cWaveformViewer::OnMouseLeftButtonUp(wxMouseEvent& event)
{
    int selected_row = SampleHive::cHiveData::Get().GetListCtrlSelectedRow();

    if (selected_row < 0)
        return;

    wxString selected = SampleHive::cHiveData::Get().GetListCtrlTextValue(selected_row, 1);
    std::string path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selected).Path.ToStdString();

    SampleHive::cTags tags(path);

    int length = tags.GetAudioInfo().length;

    double position = m_MediaCtrl.Tell();

    int panel_width = this->GetSize().GetWidth();
    double line_pos = panel_width * (position / length);

    wxPoint pos = event.GetPosition();

    double seek_to = ((double)pos.x / panel_width) * length;

    if (!wxWindow::HasCapture())
    {
        SH_LOG_INFO("Window doesn't have capture skipping..");
        return;
    }

    if (bSelectRange)
    {
        SH_LOG_DEBUG("LMB released");

        m_CurrentPoint = wxPoint(pos.x, pos.y);

        ReleaseMouse();
        SetCursor(wxCURSOR_ARROW);

        Refresh();

        bSelectRange = false;

        if (!bSelectRange)
            bDrawSelectedArea = true;
    }
    else
    {
        ReleaseMouse();
        SetCursor(wxCURSOR_ARROW);

        m_MediaCtrl.Seek(seek_to, wxFromStart);
        SampleHive::cSignal::SendPushStatusBarStatus(wxString::Format(_("Now playing: %s"), selected), 1, *this);
        SampleHive::cSignal::SendCallFunctionPlay(selected, false, *this);
    }
}

void cWaveformViewer::ResetBitmapDC()
{
    bBitmapDirty = true;
    bSelectRange = false;
    bDrawSelectedArea = false;

    Refresh();
}

std::pair<double, double> cWaveformViewer::CalculateLoopPoints()
{
    int selected_row = SampleHive::cHiveData::Get().GetListCtrlSelectedRow();

    if (selected_row < 0)
        return { 0.0, 0.0 };

    wxString selected = SampleHive::cHiveData::Get().GetListCtrlTextValue(selected_row, 1);
    std::string path = SampleHive::cUtils::Get().GetFilenamePathAndExtension(selected).Path.ToStdString();

    SampleHive::cTags tags(path);

    int length = tags.GetAudioInfo().length;

    int panel_width = this->GetSize().GetWidth();

    int a = m_AnchorPoint.x, b = m_CurrentPoint.x;

    double loopA = ((double)a / panel_width) * length;
    double loopB = ((double)b / panel_width) * length;

    return { loopA, loopB };
}
