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
    class LoopPointsEvent : public wxCommandEvent
    {
        public:
            LoopPointsEvent(wxEventType eventType, int winId);
            ~LoopPointsEvent();

        public:
            virtual wxEvent* Clone() const { return new LoopPointsEvent(*this); }

        public:
            std::pair<double, double> GetLoopPoints() const { return { m_LoopA, m_LoopB }; };
            void SetLoopPoints(std::pair<double&, double&> loopPoints)
                                  { m_LoopA = loopPoints.first; m_LoopB =  loopPoints.second; };

        private:
            double m_LoopA, m_LoopB;
    };

    wxDECLARE_EVENT(SH_EVT_LOOP_POINTS_UPDATED, LoopPointsEvent);

    // class AddSampleEvent : public wxCommandEvent
    // {
    //     public:
    //         AddSampleEvent(wxEventType eventType, int winId);
    //         ~AddSampleEvent();

    //     public:
    //         virtual wxEvent* Clone() const { return new AddSampleEvent(*this); }

    //     public:
    //         wxArrayString GetArrayString() const { return m_Files; };
    //         void SetArrayString(const wxArrayString& files) { m_Files = files; };

    //     private:
    //         wxArrayString m_Files;
    // };

    // wxDECLARE_EVENT(SH_EVT_STATUS_ADD_SAMPLE, AddSampleEvent);

    // class MediaEvent : public wxCommandEvent
    // {
    //     public:
    //         MediaEvent(wxEventType eventType, int winId);
    //         ~MediaEvent();

    //     public:
    //         virtual wxEvent* Clone() const { return new MediaEvent(*this); }

    //     public:
    //         void SetPath(const wxString& path) { m_Path = path; }
    //         wxString GetPath() const { return m_Path; }

    //     private:
    //         wxString m_Path;
    // };

    // wxDECLARE_EVENT(SH_EVT_MEDIA_STATUS_UPDATED, MediaEvent);

    class StatusBarStatusEvent : public wxCommandEvent
    {
        public:
            StatusBarStatusEvent(wxEventType eventType, int winId);
            ~StatusBarStatusEvent();

        public:
            virtual wxEvent* Clone() const { return new StatusBarStatusEvent(*this); }

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

    wxDECLARE_EVENT(SH_EVT_STATUSBAR_STATUS_PUSH, StatusBarStatusEvent);
    wxDECLARE_EVENT(SH_EVT_STATUSBAR_STATUS_POP, StatusBarStatusEvent);
    wxDECLARE_EVENT(SH_EVT_STATUSBAR_STATUS_SET, StatusBarStatusEvent);

    class InfoBarMessageEvent : public wxCommandEvent
    {
        public:
            InfoBarMessageEvent(wxEventType eventType, int winId);
            ~InfoBarMessageEvent();

        public:
            virtual wxEvent* Clone() const { return new InfoBarMessageEvent(*this); }

        public:
            std::pair<wxString, int> GetInfoBarMessage() const { return { m_Msg, m_Mode }; }
            void SetInfoBarMessage(std::pair<const wxString&, int> info)
                                      { m_Msg = info.first; m_Mode = info.second; }

        private:
            wxString m_Msg;
            int m_Mode;
    };

    wxDECLARE_EVENT(SH_EVT_INFOBAR_MESSAGE_SHOW, InfoBarMessageEvent);

    class TimerEvent : public wxCommandEvent
    {
        public:
            TimerEvent(wxEventType eventType, int winId);
            ~TimerEvent();

        public:
            virtual wxEvent* Clone() const { return new TimerEvent(*this); }
    };

    wxDECLARE_EVENT(SH_EVT_TIMER_STOP, TimerEvent);

    class CallFunctionEvent : public wxCommandEvent
    {
        public:
            CallFunctionEvent(wxEventType eventType, int winId);
            ~CallFunctionEvent();

        public:
            virtual wxEvent* Clone() const { return new CallFunctionEvent(*this); }

        public:
            wxString GetSlection() { return m_Selection; }
            void SetSelection(const wxString& selection) { m_Selection = selection; }

        private:
            wxString m_Selection;
    };

    wxDECLARE_EVENT(SH_EVT_CALL_FUNC_PLAY, CallFunctionEvent);

    class WaveformUpdateEvent : public wxCommandEvent
    {
        public:
            WaveformUpdateEvent(wxEventType eventType, int winId);
            ~WaveformUpdateEvent();

        public:
            virtual wxEvent* Clone() const { return new WaveformUpdateEvent(*this); }
    };

    wxDECLARE_EVENT(SH_EVT_UPDATE_WAVEFORM, WaveformUpdateEvent);

}
