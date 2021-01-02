/*
 * Copyright (c) 2003-2021 Rony Shapiro <ronys@pwsafe.org>.
 * All rights reserved. Use of the code is allowed under the
 * Artistic License 2.0 terms, as specified in the LICENSE file
 * distributed with this code, or available from
 * http://www.opensource.org/licenses/artistic-license-2.0.php
 */

/** \file SafeCombinationChangeDlg.h
* 
*/

#ifndef _SAFECOMBINATIONCHANGEDLG_H_
#define _SAFECOMBINATIONCHANGEDLG_H_

/*!
 * Includes
 */

////@begin includes
#include "wx/valgen.h"
////@end includes
#include "core/PWScore.h"
#ifndef NO_YUBI
#include "YubiMixin.h"
#endif

/*!
 * Forward declarations
 */

////@begin forward declarations
class SafeCombinationCtrl;
////@end forward declarations
class wxTimer;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_SAFECOMBINATIONCHANGEDLG 10074
#define ID_OLDPASSWD 10075
#if WXWIN_COMPATIBILITY_2_6
#define SYMBOL_SAFECOMBINATIONCHANGEDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL|wxTAB_TRAVERSAL
#else
#define SYMBOL_SAFECOMBINATIONCHANGEDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#endif
#define ID_YUBIBTN 10229
#define ID_NEWPASSWD 10076
#define ID_YUBIBTN2 10000
#define ID_CONFIRM 10077
#define ID_YUBISTATUS 10230
#define SYMBOL_SAFECOMBINATIONCHANGEDLG_TITLE _("Change Safe Combination")
#define SYMBOL_SAFECOMBINATIONCHANGEDLG_IDNAME ID_SAFECOMBINATIONCHANGEDLG
#define SYMBOL_SAFECOMBINATIONCHANGEDLG_SIZE wxSize(400, 300)
#define SYMBOL_SAFECOMBINATIONCHANGEDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * SafeCombinationChangeDlg class declaration
 */

class SafeCombinationChangeDlg : public wxDialog
{
  DECLARE_CLASS( SafeCombinationChangeDlg )
  DECLARE_EVENT_TABLE()

public:
  /// Constructors
  SafeCombinationChangeDlg(wxWindow* parent, PWScore &core,
                         wxWindowID id = SYMBOL_SAFECOMBINATIONCHANGEDLG_IDNAME, const wxString& caption = SYMBOL_SAFECOMBINATIONCHANGEDLG_TITLE, const wxPoint& pos = SYMBOL_SAFECOMBINATIONCHANGEDLG_POSITION, const wxSize& size = SYMBOL_SAFECOMBINATIONCHANGEDLG_SIZE, long style = SYMBOL_SAFECOMBINATIONCHANGEDLG_STYLE );

  /// Creation
  bool Create(wxWindow* parent, wxWindowID id = SYMBOL_SAFECOMBINATIONCHANGEDLG_IDNAME, const wxString& caption = SYMBOL_SAFECOMBINATIONCHANGEDLG_TITLE, const wxPoint& pos = SYMBOL_SAFECOMBINATIONCHANGEDLG_POSITION, const wxSize& size = SYMBOL_SAFECOMBINATIONCHANGEDLG_SIZE, long style = SYMBOL_SAFECOMBINATIONCHANGEDLG_STYLE );

  /// Destructor
  ~SafeCombinationChangeDlg();

  /// Initialises member variables
  void Init();

  /// Creates the controls and sizers
  void CreateControls();

////@begin SafeCombinationChangeDlg event handler declarations

#ifndef NO_YUBI
  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_YUBIBTN
  void OnYubibtnClick( wxCommandEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_YUBIBTN2
  void OnYubibtn2Click( wxCommandEvent& event );
#endif

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
  void OnOkClick( wxCommandEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
  void OnCancelClick( wxCommandEvent& event );

////@end SafeCombinationChangeDlg event handler declarations
  void OnPollingTimer(wxTimerEvent& timerEvent);

////@begin SafeCombinationChangeDlg member function declarations

  StringX GetConfirm() const { return m_confirm ; }
  void SetConfirm(StringX value) { m_confirm = value ; }

  StringX GetNewpasswd() const { return m_newpasswd ; }
  void SetNewpasswd(StringX value) { m_newpasswd = value ; }

  StringX GetOldpasswd() const { return m_oldpasswd ; }
  void SetOldpasswd(StringX value) { m_oldpasswd = value ; }

  /// Retrieves bitmap resources
  wxBitmap GetBitmapResource( const wxString& name );

  /// Retrieves icon resources
  wxIcon GetIconResource( const wxString& name );
////@end SafeCombinationChangeDlg member function declarations

  /// Should we show tooltips?
  static bool ShowToolTips();

////@begin SafeCombinationChangeDlg member variables
  SafeCombinationCtrl* m_oldPasswdEntry;
  SafeCombinationCtrl* m_newPasswdEntry;
#ifndef NO_YUBI
  wxBitmapButton* m_YubiBtn;
  wxBitmapButton* m_YubiBtn2;
  wxStaticText* m_yubiStatusCtrl;
#endif
  SafeCombinationCtrl* m_confirmEntry;
private:
  StringX m_confirm;
  StringX m_newpasswd;
  StringX m_oldpasswd;
////@end SafeCombinationChangeDlg member variables
  StringX m_oldresponse;
  PWScore &m_core;

#ifndef NO_YUBI
  // try having 2 mixin objects to handle things:
  YubiMixin m_yubiMixin1, m_yubiMixin2;
  wxTimer* m_pollingTimer; // for Yubi
#endif
};

#endif // _SAFECOMBINATIONCHANGEDLG_H_
