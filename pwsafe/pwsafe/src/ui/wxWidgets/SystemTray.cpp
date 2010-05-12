/*
 * Copyright (c) 2003-2010 Rony Shapiro <ronys@users.sourceforge.net>.
 * All rights reserved. Use of the code is allowed under the
 * Artistic License 2.0 terms, as specified in the LICENSE file
 * distributed with this code, or available from
 * http://www.opensource.org/licenses/artistic-license-2.0.php
 */

/** \file SystemTray.cpp
 * 
 */

#include "./passwordsafeframe.h"
#include "./SystemTray.h"
#include "../../corelib/PWSprefs.h"
#include "./wxutils.h"
#include "./SystemTrayMenuId.h"

#include <wx/menu.h>

#include "../graphics/wxWidgets/tray.xpm"
#include "../graphics/wxWidgets/locked_tray.xpm"
#include "../graphics/wxWidgets/unlocked_tray.xpm"
#include "../graphics/toolbar/wxWidgets/copypassword.xpm"
#include "../graphics/toolbar/wxWidgets/copyuser.xpm"
#include "../graphics/toolbar/wxWidgets/copynotes.xpm"
#include "../graphics/toolbar/wxWidgets/clearclipboard.xpm"
#include "../graphics/toolbar/wxWidgets/autotype.xpm"
#include "../graphics/toolbar/wxWidgets/browseurl.xpm"
#include "../graphics/toolbar/wxWidgets/browseurlplus.xpm"
#include "../graphics/toolbar/wxWidgets/sendemail.xpm"
#include "../graphics/toolbar/wxWidgets/delete.xpm"
#include "../graphics/toolbar/wxWidgets/about.xpm"
#include "../graphics/toolbar/wxWidgets/exit.xpm"
#include "../graphics/toolbar/wxWidgets/lock.xpm"
#include "../graphics/toolbar/wxWidgets/unlock.xpm"
#include "../graphics/toolbar/wxWidgets/close.xpm"

BEGIN_EVENT_TABLE( SystemTray, wxTaskBarIcon )
  EVT_MENU( ID_SYSTRAY_RESTORE, SystemTray::OnSysTrayMenuItem )
  EVT_MENU( ID_SYSTRAY_LOCK,    SystemTray::OnSysTrayMenuItem )
  EVT_MENU( ID_SYSTRAY_UNLOCK,  SystemTray::OnSysTrayMenuItem )
  EVT_MENU( wxID_EXIT,          SystemTray::OnSysTrayMenuItem )
  EVT_MENU( wxID_ICONIZE_FRAME, SystemTray::OnSysTrayMenuItem )
  EVT_MENU( ID_CLEARCLIPBOARD,  SystemTray::OnSysTrayMenuItem )
  EVT_MENU( wxID_ABOUT,         SystemTray::OnSysTrayMenuItem )
  EVT_MENU( wxID_CLOSE,         SystemTray::OnSysTrayMenuItem )
  EVT_MENU( ID_MENU_CLEAR_MRU,  SystemTray::OnSysTrayMenuItem )
  EVT_MENU_RANGE(MIN_RUE_COMMAND_ID, MAX_RUE_COMMAND_ID, SystemTray::OnSysTrayMenuItem )
  EVT_TASKBAR_LEFT_DCLICK( SystemTray::OnTaskBarLeftDoubleClick )
END_EVENT_TABLE()

SystemTray::SystemTray(PasswordSafeFrame* frame) : iconClosed(tray_xpm), 
                                                   iconUnlocked(unlocked_tray_xpm), 
                                                   iconLocked(locked_tray_xpm),
                                                   m_frame(frame),
                                                   m_status(TRAY_CLOSED)
{
}

void SystemTray::SetTrayStatus(TrayStatus status)
{
  m_status = status;
 
#if wxCHECK_VERSION(2,9,0)
  if (!wxTaskBarIcon::IsAvailable())
    return;
#endif

  if (PWSprefs::GetInstance()->GetPref(PWSprefs::UseSystemTray)) {
     switch(status) {
       case TRAY_CLOSED:
         SetIcon(iconClosed);
         break;

       case TRAY_UNLOCKED:
         SetIcon(iconUnlocked);
         break;

       case TRAY_LOCKED:
         SetIcon(iconLocked);
         break;

       default:
         break;
     }
  }
}

//virtual 
wxMenu* SystemTray::CreatePopupMenu()
{
  wxMenu* menu = new wxMenu;

  switch (m_status) {
    case TRAY_UNLOCKED:
        menu->Append(ID_SYSTRAY_LOCK, wxT("&Lock Safe"))->SetBitmap(wxBitmap(lock_xpm));
      break;

    case TRAY_LOCKED:
        menu->Append(ID_SYSTRAY_UNLOCK, wxT("&Unlock Safe"))->SetBitmap(wxBitmap(unlock_xpm));
        break;

    case TRAY_CLOSED:
        menu->Append(wxID_NONE, wxT("No Safe Open"));
        break;

    default:
        break;

  }

  if (m_status != TRAY_CLOSED) {
    menu->AppendSeparator();
    menu->Append(wxID_CLOSE, wxT("&Close"))->SetBitmap(wxBitmap(close_xpm));
    menu->AppendSubMenu(GetRecentHistory(), wxT("&Recent Entries History"));
  }
  
  menu->AppendSeparator();
  menu->Append(wxID_ICONIZE_FRAME, wxT("&Minimize"));
  menu->Append(ID_SYSTRAY_RESTORE, wxT("&Restore"));
  menu->AppendSeparator();
  menu->Append(ID_CLEARCLIPBOARD,  wxT("&Clear Clipboard"))->SetBitmap(wxBitmap(clearclipboard_xpm));
  menu->Append(wxID_ABOUT,         wxT("&About Password Safe..."))->SetBitmap(wxBitmap(about_xpm));
  menu->AppendSeparator();
  menu->Append(wxID_EXIT, wxT("&Exit"))->SetBitmap(wxBitmap(exit_xpm));
  
  //let the user iconize even if its already iconized
  if (!m_frame->IsShown())
    menu->Enable(wxID_ICONIZE_FRAME, false);

  return menu;
}

wxMenu* SystemTray::GetRecentHistory()
{
  //Must be on the heap.  wxWidgets will delete it
  wxMenu* menu = new wxMenu;

  menu->Append(ID_MENU_CLEAR_MRU, wxT("&Clear Recent History"));
  menu->Append(wxID_NONE, wxT("Note: Entry format is »Group»Title»Username»"));
  menu->Append(wxID_NONE, wxT("Note: Empty fields are shown as »*»"));
  menu->AppendSeparator();

  std::vector<RUEntryData> menulist;
  m_frame->GetAllMenuItemStrings(menulist);

  for (size_t idx = 0; idx < menulist.size(); ++idx) {
    if (menulist[idx].pci && !menulist[idx].string.empty()) {
      menu->AppendSubMenu(SetupRecentEntryMenu(menulist[idx].pci, idx), towxstring(menulist[idx].string));
    }
  }

  return menu;
}

wxMenu* SystemTray::SetupRecentEntryMenu(const CItemData* pci, size_t idx)
{
  wxASSERT(pci);

  wxMenu* menu = new wxMenu;

  menu->Append(MakeCommandId(RUE_COPYPASSWORD, idx), wxT("&Copy Password to clipboard"))->SetBitmap(wxBitmap(copypassword_xpm));

  if (!pci->IsUserEmpty())
    menu->Append(MakeCommandId(RUE_COPYUSERNAME, idx), wxT("Copy &Username to clipboard"))->SetBitmap(wxBitmap(copyuser_xpm));

  if (!pci->IsNotesEmpty())
    menu->Append(MakeCommandId(RUE_COPYNOTES, idx), wxT("Copy &Notes to clipboard"))->SetBitmap(wxBitmap(copynotes_xpm));

  menu->Append(MakeCommandId(RUE_AUTOTYPE, idx), wxT("Perform Auto&Type"))->SetBitmap(wxBitmap(autotype_xpm));

  if (!pci->IsURLEmpty() && !pci->IsURLEmail())
    menu->Append(MakeCommandId(RUE_COPYURL, idx), wxT("Copy URL to clipboard"));

  if (!pci->IsEmailEmpty() || (!pci->IsURLEmpty() && pci->IsURLEmail()))
    menu->Append(MakeCommandId(RUE_COPYEMAIL, idx), wxT("Copy email to clipboard"));

  if (!pci->IsURLEmpty() && !pci->IsURLEmail()) {
    menu->Append(MakeCommandId(RUE_BROWSE, idx), wxT("&Browse to URL"))->SetBitmap(wxBitmap(browseurl_xpm));
    menu->Append(MakeCommandId(RUE_BROWSEAUTOTYPE, idx), wxT("Browse to URL + &Autotype"))->SetBitmap(wxBitmap(browseurlplus_xpm));
  }

  if (!pci->IsEmailEmpty() || (!pci->IsURLEmpty() && pci->IsURLEmail()))
    menu->Append(MakeCommandId(RUE_SENDEMAIL, idx), wxT("&Send email"))->SetBitmap(wxBitmap(sendemail_xpm));

  if (!pci->IsRunCommandEmpty())
    menu->Append(MakeCommandId(RUE_RUNCOMMAND, idx), wxT("&Run Command"));

  menu->Append(MakeCommandId(RUE_DELETERUEENTRY, idx), wxT("&Delete from Recent Entry List"))->SetBitmap(wxBitmap(delete_xpm));

  return menu;
}

void SystemTray::OnSysTrayMenuItem(wxCommandEvent& evt)
{
  const int id = evt.GetId();
  if (IsRUECommand(id)) {
    RUEOperation opn = GetRUEOperation(id);
    if (opn == RUE_DELETERUEENTRY) {
      m_frame->DeleteRUEntry(GetRUEIndex(id));
    }
    else {
      wxCommandEvent cmd(evt.GetEventType(), GetFrameCommandId(opn));
      cmd.SetExtraLong(GetRUEIndex(id));
      m_frame->ProcessEvent(cmd);
    }
  }
  else {
    switch(id) {

      case ID_SYSTRAY_RESTORE:
        m_frame->UnlockSafe(true); // true => restore UI
        break;

      case ID_SYSTRAY_LOCK:
        m_frame->HideUI(true);
        break;

      case ID_SYSTRAY_UNLOCK:
        m_frame->UnlockSafe(false); // false => don't restore UI
        break;

      case wxID_EXIT:
      case wxID_ICONIZE_FRAME:
      case ID_CLEARCLIPBOARD:
      case wxID_ABOUT:
      case wxID_CLOSE:
      case ID_MENU_CLEAR_MRU:
        m_frame->ProcessEvent(evt);
        break;

      default:
        break;
    }
  }
}

void SystemTray::OnTaskBarLeftDoubleClick(wxTaskBarIconEvent& /*evt*/)
{
  m_frame->UnlockSafe(true); //true => restore UI
}
