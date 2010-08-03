#ifndef __ADVANCEDDSELECTIONDLG_H__
#define __ADVANCEDDSELECTIONDLG_H__


#include "../../corelib/ItemData.h"

struct _subgroups {
  const charT* name;
  CItemData::FieldType type;
};
extern struct _subgroups subgroups[];

struct _subgroupFunctions {
  const charT* name;
  PWSMatch::MatchRule function;
};
extern struct _subgroupFunctions subgroupFunctions[];


/*!
 * SelectionCriteria class declaration
 */

struct SelectionCriteria 
{
  SelectionCriteria() : m_fCaseSensitive(false),
                        m_subgroupText(_("*")),
                        m_fUseSubgroups(false),
                        m_subgroupObject(0),            // index into subgroups array defined in .cpp
                        m_subgroupFunction(0),          // index into subgroupFunctions array defined in .cpp
                        m_fDirty(false)
  {
    m_bsFields.set();
  }
  
  SelectionCriteria(const SelectionCriteria& other):  m_fCaseSensitive(other.m_fCaseSensitive),
                                                      m_bsFields(other.m_bsFields),
                                                      m_subgroupText(other.m_subgroupText),
                                                      m_fUseSubgroups(other.m_fUseSubgroups),
                                                      m_subgroupObject(other.m_subgroupObject),
                                                      m_subgroupFunction(other.m_subgroupFunction),
                                                      m_fDirty(false)
  {}
  
  bool                  m_fCaseSensitive;
  CItemData::FieldBits  m_bsFields;
  wxString              m_subgroupText;
  bool                  m_fUseSubgroups;
  int                   m_subgroupObject;
  int                   m_subgroupFunction;
  bool                  m_fDirty;

  bool IsDirty(void) const { return m_fDirty; }
  void Clean(void) { m_fDirty = false; }
  
  SelectionCriteria& operator=(const SelectionCriteria& data) {
    m_fCaseSensitive    = data.m_fCaseSensitive;
    m_bsFields          = data.m_bsFields;
    m_subgroupText      = data.m_subgroupText;
    m_fUseSubgroups     = data.m_fUseSubgroups;
    m_subgroupObject    = data.m_subgroupObject;
    m_subgroupFunction  = data.m_subgroupFunction;
    
    m_fDirty = true;
    
    return *this;
  }
};

inline bool operator!=(const SelectionCriteria& a, const SelectionCriteria& b)
{
  return a.m_bsFields         != b.m_bsFields         || 
         a.m_fCaseSensitive   != b.m_fCaseSensitive   ||
         a.m_fUseSubgroups    != b.m_fUseSubgroups    ||
         a.m_subgroupFunction != b.m_subgroupFunction ||
         a.m_subgroupText     != b.m_subgroupText     ||
         a.m_subgroupObject   != b.m_subgroupObject; 
}



/*!
 * AdvancedSelectionDlg class declaration
 */

class AdvancedSelectionDlg: public wxDialog
{
  DECLARE_CLASS(AdvancedSelectionDlg)
  DECLARE_EVENT_TABLE()

  DECLARE_NO_COPY_CLASS(AdvancedSelectionDlg)

public:
  AdvancedSelectionDlg(wxWindow* wnd, const SelectionCriteria& existingCriteria);

  void OnOk( wxCommandEvent& evt );
  void OnSelectSome( wxCommandEvent& evt );
  void OnSelectAll( wxCommandEvent& evt );
  void OnRemoveSome( wxCommandEvent& evt );
  void OnRemoveAll( wxCommandEvent& evt );

private:
  void CreateControls(wxWindow* parentWnd);
  
public:
  SelectionCriteria m_criteria;
};

#endif
