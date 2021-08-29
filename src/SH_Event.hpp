#pragma once

#include <utility>

// #include "wx/arrstr.h"
#include "wx/event.h"

namespace SampleHive
{
    class SH_LoopPointsEvent : public wxCommandEvent
    {
        public:
            SH_LoopPointsEvent(wxEventType eventType, int winId);
            ~SH_LoopPointsEvent();

        public:
            virtual wxEvent* Clone() const { return new SH_LoopPointsEvent(*this); }

        public:
            std::pair<double, double> GetLoopPoints() const { return { m_LoopA, m_LoopB }; };
            void SetLoopPoints(std::pair<double&, double&> loopPoints)
                                  { m_LoopA = loopPoints.first; m_LoopB =  loopPoints.second; };

        private:
            double m_LoopA, m_LoopB;
    };

    wxDECLARE_EVENT(SH_EVT_LOOP_POINTS_UPDATED, SH_LoopPointsEvent);

    // class SH_AddSampleEvent : public wxCommandEvent
    // {
    //     public:
    //         SH_AddSampleEvent(wxEventType eventType, int winId);
    //         ~SH_AddSampleEvent();

    //     public:
    //         virtual wxEvent* Clone() const { return new SH_AddSampleEvent(*this); }

    //     public:
    //         wxArrayString GetArrayString() const { return m_Files; };
    //         void SetArrayString(const wxArrayString& files) { m_Files = files; };

    //     private:
    //         wxArrayString m_Files;
    // };

    // wxDECLARE_EVENT(SH_EVT_STATUS_ADD_SAMPLE, SH_AddSampleEvent);

    // class SH_MediaEvent : public wxCommandEvent
    // {
    //     public:
    //         SH_MediaEvent(wxEventType eventType, int winId);
    //         ~SH_MediaEvent();

    //     public:
    //         virtual wxEvent* Clone() const { return new SH_MediaEvent(*this); }

    //     public:
    //         void SetPath(const wxString& path) { m_Path = path; }
    //         wxString GetPath() const { return m_Path; }

    //     private:
    //         wxString m_Path;
    // };

    // wxDECLARE_EVENT(SH_EVT_MEDIA_STATUS_UPDATED, SH_MediaEvent);

    class SH_SetStatusBarMessageEvent : public wxCommandEvent
    {
        public:
            SH_SetStatusBarMessageEvent(wxEventType eventType, int winId);
            ~SH_SetStatusBarMessageEvent();

        public:
            virtual wxEvent* Clone() const { return new SH_SetStatusBarMessageEvent(*this); }

        public:
            std::pair<wxString, int> GetMessageAndSection() const { return {m_Msg, m_Section }; }
            void SetMessageAndSection(std::pair<const wxString&, int> status) { m_Msg = status.first; m_Section = status.second; }

        private:
            wxString m_Msg;
            int m_Section;
    };

    wxDECLARE_EVENT(SH_EVT_STATUSBAR_MESSAGE_UPDATED, SH_SetStatusBarMessageEvent);
}
