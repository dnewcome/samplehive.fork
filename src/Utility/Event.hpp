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

#include <utility>

#include <wx/event.h>

namespace SampleHive
{
    class cLoopPointsEvent : public wxCommandEvent
    {
        public:
            cLoopPointsEvent(wxEventType eventType, int winId);
            ~cLoopPointsEvent();

        public:
            virtual wxEvent* Clone() const { return new cLoopPointsEvent(*this); }

        public:
            std::pair<double, double> GetLoopPoints() const { return { m_LoopA, m_LoopB }; };
            void SetLoopPoints(std::pair<double&, double&> loopPoints)
                                  { m_LoopA = loopPoints.first; m_LoopB =  loopPoints.second; };

        private:
            double m_LoopA, m_LoopB;
    };

    wxDECLARE_EVENT(SH_EVT_LOOP_POINTS_UPDATED, cLoopPointsEvent);
    wxDECLARE_EVENT(SH_EVT_LOOP_POINTS_CLEAR, cLoopPointsEvent);
    wxDECLARE_EVENT(SH_EVT_LOOP_AB_BUTTON_VALUE_CHANGE, cLoopPointsEvent);

    // class cMediaEvent : public wxCommandEvent
    // {
    //     public:
    //         cMediaEvent(wxEventType eventType, int winId);
    //         ~cMediaEvent();

    //     public:
    //         virtual wxEvent* Clone() const { return new cMediaEvent(*this); }

    //     public:
    //         void SetPath(const wxString& path) { m_Path = path; }
    //         wxString GetPath() const { return m_Path; }

    //     private:
    //         wxString m_Path;
    // };

    // wxDECLARE_EVENT(SH_EVT_MEDIA_STATUS_UPDATED, cMediaEvent);

    class cStatusBarStatusEvent : public wxCommandEvent
    {
        public:
            cStatusBarStatusEvent(wxEventType eventType, int winId);
            ~cStatusBarStatusEvent();

        public:
            virtual wxEvent* Clone() const { return new cStatusBarStatusEvent(*this); }

        public:
            std::pair<wxString, int> GetPushMessageAndSection() const { return { m_Msg, m_PushSection }; }
            void SetPushMessageAndSection(std::pair<const wxString&, int> status)
                                             { m_Msg = status.first; m_PushSection = status.second; }

            std::pair<wxString, int> GetStatusTextAndSection() const { return { m_Text, m_SetSection }; }
            void SetStatusTextAndSection(std::pair<const wxString&, int> status)
                                            { m_Text = status.first, m_SetSection = status.second; }

            int GetPopMessageSection() const { return m_PopSection; }
            void SetPopMessageSection(int section) { m_PopSection = section; }

        private:
            wxString m_Msg, m_Text;
            int m_PushSection, m_PopSection, m_SetSection;
    };

    wxDECLARE_EVENT(SH_EVT_STATUSBAR_STATUS_PUSH, cStatusBarStatusEvent);
    wxDECLARE_EVENT(SH_EVT_STATUSBAR_STATUS_POP, cStatusBarStatusEvent);
    wxDECLARE_EVENT(SH_EVT_STATUSBAR_STATUS_SET, cStatusBarStatusEvent);

    class cInfoBarMessageEvent : public wxCommandEvent
    {
        public:
            cInfoBarMessageEvent(wxEventType eventType, int winId);
            ~cInfoBarMessageEvent();

        public:
            virtual wxEvent* Clone() const { return new cInfoBarMessageEvent(*this); }

        public:
            std::pair<wxString, int> GetInfoBarMessage() const { return { m_Msg, m_Mode }; }
            void SetInfoBarMessage(std::pair<const wxString&, int> info)
                                      { m_Msg = info.first; m_Mode = info.second; }

        private:
            wxString m_Msg;
            int m_Mode;
    };

    wxDECLARE_EVENT(SH_EVT_INFOBAR_MESSAGE_SHOW, cInfoBarMessageEvent);

    class cTimerEvent : public wxCommandEvent
    {
        public:
            cTimerEvent(wxEventType eventType, int winId);
            ~cTimerEvent();

        public:
            virtual wxEvent* Clone() const { return new cTimerEvent(*this); }
    };

    wxDECLARE_EVENT(SH_EVT_TIMER_STOP, cTimerEvent);

    class cCallFunctionEvent : public wxCommandEvent
    {
        public:
            cCallFunctionEvent(wxEventType eventType, int winId);
            ~cCallFunctionEvent();

        public:
            virtual wxEvent* Clone() const { return new cCallFunctionEvent(*this); }

        public:
            wxString GetSlection() const { return m_Selection; }
            bool GetAutoplayValue() const { return m_bCheckAutoplay; }
            void SetSelection(const wxString& selection) { m_Selection = selection; }
            void SetAutoplayValue(bool autoplay) { m_bCheckAutoplay = autoplay; }

        private:
            wxString m_Selection;
            bool m_bCheckAutoplay;
    };

    wxDECLARE_EVENT(SH_EVT_CALL_FUNC_PLAY, cCallFunctionEvent);

    class cWaveformUpdateEvent : public wxCommandEvent
    {
        public:
            cWaveformUpdateEvent(wxEventType eventType, int winId);
            ~cWaveformUpdateEvent();

        public:
            virtual wxEvent* Clone() const { return new cWaveformUpdateEvent(*this); }
    };

    wxDECLARE_EVENT(SH_EVT_UPDATE_WAVEFORM, cWaveformUpdateEvent);

}
