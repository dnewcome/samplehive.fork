#include "SH_Event.hpp"

namespace SampleHive
{
    SH_LoopPointsEvent::SH_LoopPointsEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    SH_LoopPointsEvent::~SH_LoopPointsEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_LOOP_POINTS_UPDATED, SH_LoopPointsEvent);

    // SH_AddSampleEvent::SH_AddSampleEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // SH_AddSampleEvent::~SH_AddSampleEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_STATUS_ADD_SAMPLE, SH_AddSampleEvent);

    // SH_MediaEvent::SH_MediaEvent(wxEventType eventType, int winId)
    //     : wxCommandEvent(eventType, winId)
    // {

    // }

    // SH_MediaEvent::~SH_MediaEvent()
    // {

    // }

    // wxDEFINE_EVENT(SH_EVT_MEDIA_STATUS_UPDATED, SH_MediaEvent);

    SH_SetStatusBarMessageEvent::SH_SetStatusBarMessageEvent(wxEventType eventType, int winId)
        : wxCommandEvent(eventType, winId)
    {

    }

    SH_SetStatusBarMessageEvent::~SH_SetStatusBarMessageEvent()
    {

    }

    wxDEFINE_EVENT(SH_EVT_STATUSBAR_MESSAGE_UPDATED, SH_SetStatusBarMessageEvent);
}
