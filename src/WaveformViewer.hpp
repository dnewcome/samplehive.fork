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

#include <wx/dataview.h>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/dc.h>
#include <wx/event.h>
#include <wx/infobar.h>
#include <wx/mediactrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statusbr.h>
#include <wx/timer.h>
#include <wx/window.h>

class WaveformViewer : public wxPanel
{
    public:
        WaveformViewer(wxWindow* parentFrame, wxWindow* window, wxDataViewListCtrl& library,
                       wxMediaCtrl& mediaCtrl, wxInfoBar& infoBar,
                       const std::string& configFilepath, const std::string& databaseFilepath);
        ~WaveformViewer();

    private:
        // -------------------------------------------------------------------
        wxWindow* m_ParentFrame;
        wxWindow* m_Window;

        wxDataViewListCtrl& m_Library;
        wxInfoBar& m_InfoBar;
        wxMediaCtrl& m_MediaCtrl;

        const std::string& m_ConfigFilepath;
        const std::string& m_DatabaseFilepath;

    private:
        // -------------------------------------------------------------------
        wxBitmap m_WaveformBitmap;
        wxColour m_PlayheadColour;
        wxColour m_WaveformColour;

        // -------------------------------------------------------------------
        // Selection area coordinates
        wxPoint m_AnchorPoint;
        wxPoint m_CurrentPoint;

    private:
        // -------------------------------------------------------------------
        bool bBitmapDirty = false;
        bool bSelectRange = false;
        bool bDrawSelectedArea = false;
        bool bAreaSelected = false;

    private:
        // -------------------------------------------------------------------
        void OnPaint(wxPaintEvent& event);
        void RenderPlayhead(wxDC& dc);
        void UpdateWaveformBitmap();

        // -------------------------------------------------------------------
        void OnMouseMotion(wxMouseEvent& event);
        void OnMouseLeftButtonDown(wxMouseEvent& event);
        void OnMouseLeftButtonUp(wxMouseEvent& event);

        // -------------------------------------------------------------------
        void OnControlKeyUp(wxKeyEvent& event);
        void OnControlKeyDown(wxKeyEvent& event);

        // -------------------------------------------------------------------
        // Send custom events
        void SendLoopPoints();
        void SendStatusBarStatus(const wxString& msg, int section);

    public:
        // -------------------------------------------------------------------
        void ResetDC();
};
