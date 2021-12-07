#ifndef __AUITABART_H__
#define __AUITABART_H__

#include <wx/aui/auibook.h>

class AuiTabArt : public wxAuiGenericTabArt {
public:
	wxSize GetTabSize(
		wxDC& dc,
		wxWindow* wnd,
		const wxString& caption,
		const wxBitmapBundle& bitmap,
		bool active,
		int closeButtonState,
		int* xExtent) wxOVERRIDE {
		return wxAuiGenericTabArt::GetTabSize(dc, wnd, caption, bitmap, active, closeButtonState,
		                                      xExtent) +
		       wxSize(0, 2);
	}
};

#endif // __AUITABART_H__
