#include "Utility/Signal.hpp"
#include "Utility/Log.hpp"
#include "Utility/SH_Event.hpp"

namespace SampleHive {

    void Signal::SendInfoBarMessage(const wxString& msg, int mode, wxWindow& window, bool isDialog)
    {
        SampleHive::InfoBarMessageEvent event(SampleHive::SH_EVT_INFOBAR_MESSAGE_SHOW, window.GetId());
        event.SetEventObject(&window);

        event.SetInfoBarMessage({ msg, mode });

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void Signal::SendPushStatusBarStatus(const wxString& msg, int section, wxWindow& window, bool isDialog)
    {
        SampleHive::StatusBarStatusEvent event(SampleHive::SH_EVT_STATUSBAR_STATUS_PUSH, window.GetId());
        event.SetEventObject(&window);

        event.SetPushMessageAndSection({ msg, section });

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void Signal::SendPopStatusBarStatus(int section, wxWindow& window, bool isDialog)
    {
        SampleHive::StatusBarStatusEvent event(SampleHive::SH_EVT_STATUSBAR_STATUS_POP, window.GetId());
        event.SetEventObject(&window);

        event.SetPopMessageSection(section);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void Signal::SendSetStatusBarStatus(const wxString& text, int section, wxWindow& window, bool isDialog)
    {
        SampleHive::StatusBarStatusEvent event(SampleHive::SH_EVT_STATUSBAR_STATUS_SET, window.GetId());
        event.SetEventObject(&window);

        event.SetStatusTextAndSection({ text, section });

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void Signal::SendCallFunctionPlay(const wxString& selection, wxWindow& window, bool isDialog)
    {
        SampleHive::CallFunctionEvent event(SampleHive::SH_EVT_CALL_FUNC_PLAY, window.GetId());
        event.SetEventObject(&window);

        event.SetSelection(selection);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void Signal::SendTimerStopStatus(wxWindow& window, bool isDialog)
    {
        SampleHive::TimerEvent event(SampleHive::SH_EVT_TIMER_STOP, window.GetId());
        event.SetEventObject(&window);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

    void Signal::SendLoopPoints(std::pair<double, double> loopPoint, wxWindow& window, bool isDialog)
    {
        SampleHive::LoopPointsEvent event(SampleHive::SH_EVT_LOOP_POINTS_UPDATED, window.GetId());
        event.SetEventObject(&window);

        event.SetLoopPoints({loopPoint.first, loopPoint.second});

        window.HandleWindowEvent(event);
    }

    void Signal::SendWaveformUpdateStatus(wxWindow& window, bool isDialog)
    {
        SampleHive::WaveformUpdateEvent event(SampleHive::SH_EVT_UPDATE_WAVEFORM, window.GetId());
        event.SetEventObject(&window);

        if (isDialog)
            window.GetParent()->GetEventHandler()->ProcessEvent(event);
        else
            window.HandleWindowEvent(event);
    }

}
