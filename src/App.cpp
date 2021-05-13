#include "App.hpp"
#include "ControlID_Enums.hpp"

#include <wx/bitmap.h>
#include <wx/defs.h>
#include <wx/eventfilter.h>
#include <wx/gdicmn.h>
#include <wx/gtk/window.h>
#include <wx/log.h>
#include <wx/object.h>
#include <wx/rtti.h>
#include <wx/splash.h>

wxIMPLEMENT_APP(App);

App::App()
{

}

App::~App()
{

}

bool App::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    m_Frame = new MainFrame();

    wxBitmap bitmap;
    wxSplashScreen* splash;

    if (bitmap.LoadFile("../assets/logo/logo-hive_768x432.png", wxBITMAP_TYPE_PNG))
    {
        splash = new wxSplashScreen(bitmap,
                                    wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                                    2000, NULL, -1, wxDefaultPosition, wxDefaultSize,
                                    wxBORDER_SIMPLE | wxSTAY_ON_TOP);
    }

    wxYield();

    m_Frame->Show(true);
    return true;
}

int App::FilterEvent(wxEvent& event)
{
    if(event.GetEventType() == wxEVT_KEY_DOWN)
    {
        wxWindow* focusedWindow = wxWindow::FindFocus();
        if(focusedWindow != NULL)
        {
            const wxString& className = focusedWindow->GetClassInfo()->GetClassName();

            wxLogDebug("Focused window: %s", className);

            // if SearchCtrl or TreeCtrl has focus, let all keys through
            if(className == wxT("wxSearchCtrl"))
            {
                wxLogDebug("SearchCtrl focused skipping key filtering");
                return wxEventFilter::Event_Skip;
            }
            else if(className == wxT("wxTreeCtrl"))
            {
                wxLogDebug("TreeCtrl focused skipping key filtering");
                return wxEventFilter::Event_Skip;
            }
        }

        wxKeyEvent* keyEvent = wxDynamicCast(&event, wxKeyEvent);

        if(keyEvent != NULL)
        {
            switch (keyEvent->GetKeyCode())
            {
                case WXK_SPACE:
                {
                    wxCommandEvent playEvent(wxEVT_BUTTON);

                    playEvent.SetId(BC_Play);
                    playEvent.SetEventObject(m_Frame->m_PlayButton);

                    m_Frame->OnClickPlay(playEvent);

                    wxLogDebug("Space pressed");

                    return wxEventFilter::Event_Processed;

                    break;
                }
                case 'A':
                {
                    wxCommandEvent autoplayEvent(wxEVT_CHECKBOX);
                    autoplayEvent.SetId(BC_Autoplay);

                    autoplayEvent.SetEventObject(m_Frame->m_AutoPlayCheck);

                    if (m_Frame->m_AutoPlayCheck->GetValue())
                        m_Frame->m_AutoPlayCheck->SetValue(false);
                    else
                        m_Frame->m_AutoPlayCheck->SetValue(true);

                    m_Frame->OnCheckAutoplay(autoplayEvent);

                    wxLogDebug("A pressed");

                    return wxEventFilter::Event_Processed;

                    break;
                }
                case 'L':
                {
                    wxCommandEvent loopEvent(wxEVT_TOGGLEBUTTON);

                    loopEvent.SetId(BC_Loop);
                    loopEvent.SetEventObject(m_Frame->m_LoopButton);

                    if (m_Frame->m_LoopButton->GetValue())
                        m_Frame->m_LoopButton->SetValue(false);
                    else
                        m_Frame->m_LoopButton->SetValue(true);

                    m_Frame->OnClickLoop(loopEvent);

                    wxLogDebug("L pressed");

                    return wxEventFilter::Event_Processed;

                    break;
                }
                case 'M':
                {
                    wxCommandEvent muteEvent(wxEVT_TOGGLEBUTTON);

                    muteEvent.SetId(BC_Mute);
                    muteEvent.SetEventObject(m_Frame->m_MuteButton);

                    if (m_Frame->m_MuteButton->GetValue())
                        m_Frame->m_MuteButton->SetValue(false);
                    else
                        m_Frame->m_MuteButton->SetValue(true);

                    m_Frame->OnClickMute(muteEvent);

                    wxLogDebug("M pressed");

                    return wxEventFilter::Event_Processed;

                    break;
                }
                case 'O':
                {
                    wxCommandEvent settingsEvent(wxEVT_BUTTON);

                    settingsEvent.SetId(BC_Settings);
                    settingsEvent.SetEventObject(m_Frame->m_SettingsButton);

                    m_Frame->OnClickSettings(settingsEvent);

                    wxLogDebug("O pressed");

                    return wxEventFilter::Event_Processed;

                    break;
                }
                case 'P':
                {
                    wxCommandEvent playEvent(wxEVT_BUTTON);

                    playEvent.SetId(BC_Play);
                    playEvent.SetEventObject(m_Frame->m_PlayButton);

                    m_Frame->OnClickPlay(playEvent);

                    wxLogDebug("P pressed");

                    return wxEventFilter::Event_Processed;

                    break;
                }
                case 'S':
                {
                    wxCommandEvent stopEvent(wxEVT_BUTTON);

                    stopEvent.SetId(BC_Stop);
                    stopEvent.SetEventObject(m_Frame->m_StopButton);

                    m_Frame->OnClickStop(stopEvent);

                    wxLogDebug("S pressed");

                    return wxEventFilter::Event_Processed;

                    break;
                }
            }
        }
    }

    return -1;
}
