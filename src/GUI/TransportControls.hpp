#pragma once

#include "wx/bmpbuttn.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/dataview.h"
#include "wx/event.h"
#include "wx/mediactrl.h"
#include "wx/settings.h"
#include "wx/sizer.h"
#include "wx/slider.h"
#include "wx/stattext.h"
#include "wx/tglbtn.h"
#include "wx/panel.h"
#include "wx/timer.h"
#include "wx/window.h"

class TransportControls : public wxPanel
{
    public:
        // -------------------------------------------------------------------
        TransportControls(wxWindow* window, wxDataViewListCtrl& library, wxMediaCtrl& mediaCtrl, wxTimer& timer);
        ~TransportControls();

    public:
        // -------------------------------------------------------------------
        bool IsLoopABOn() const { return m_pLoopABButton->GetValue(); }

        // -------------------------------------------------------------------
        inline bool CanAutoplay() const { return m_bAutoplay; };
        inline bool CanLoop() const { return m_bLoop; };
        inline bool IsMuted() const { return m_bMuted; };
        inline bool IsStopped() const { return m_bStopped; };

        // -------------------------------------------------------------------
        void SetLoopABValue(bool value) { m_pLoopABButton->SetValue(value); }

        // -------------------------------------------------------------------
        void SetCanAutoplay(bool autoplay) { m_bAutoplay = autoplay; }
        void SetCanLoop(bool loop) { m_bLoop = loop; }
        void SetIsMuted(bool muted) { m_bMuted = muted; }
        void SetIsStopped(bool stopped) { m_bStopped = stopped; }

        // -------------------------------------------------------------------
        inline wxString GetSamplePositionText() const { return m_pSamplePosition->GetLabelText(); }

        // -------------------------------------------------------------------
        void SetSamplePositionText(const wxString& text) { m_pSamplePosition->SetLabel(text); }

    private:
        // -------------------------------------------------------------------
        // Event handlers
        void OnClickPlay(wxCommandEvent& event);
        void OnClickLoop(wxCommandEvent& event);
        void OnClickStop(wxCommandEvent& event);
        void OnClickMute(wxCommandEvent& event);
        void OnClickSettings(wxCommandEvent& event);
        void OnCheckAutoplay(wxCommandEvent& event);
        void OnSlideVolume(wxScrollEvent& event);
        void OnReleaseVolumeSlider(wxScrollEvent& event);

    private:
        // -------------------------------------------------------------------
        // Custom events for sending infomation to MainFrame
        void SendPushStatusBarStatus(const wxString& msg, int section);
        void SendSetStatusBarStatus(const wxString& msg, int section);
        void SendPopStatusBarStatus(int section);
        void SendCallFunctionPlay(const wxString& selection);
        void SendTimerStopStatus();

    private:
        // -------------------------------------------------------------------
        // Load control values from config file
        void LoadConfigFile();

    private:
        // -------------------------------------------------------------------
        wxDataViewListCtrl& m_Library;
        wxMediaCtrl& m_MediaCtrl;
        wxTimer& m_Timer;

    private:
        // -------------------------------------------------------------------
        bool m_bAutoplay = false;
        bool m_bLoop = false;
        bool m_bMuted = false;
        bool m_bStopped = false;

    private:
        // -------------------------------------------------------------------
        wxBitmapButton* m_pPlayButton = nullptr;
        wxBitmapToggleButton* m_pLoopButton = nullptr;
        wxBitmapButton* m_pStopButton = nullptr;
        wxButton* m_pSettingsButton = nullptr;
        wxBitmapToggleButton* m_pMuteButton = nullptr;
        wxBitmapToggleButton* m_pLoopABButton = nullptr;
        wxStaticText* m_pSamplePosition = nullptr;
        wxSlider* m_pVolumeSlider = nullptr;
        wxCheckBox* m_pAutoPlayCheck = nullptr;

        // -------------------------------------------------------------------
        wxBoxSizer* m_pMainSizer = nullptr;

    private:
        // -------------------------------------------------------------------
        wxSystemAppearance m_Theme = wxSystemSettings::GetAppearance();
};
