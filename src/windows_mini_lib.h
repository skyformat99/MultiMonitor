#ifndef WINDOWS_MINI_LIB_H
#define WINDOWS_MINI_LIB_H

#include <map>
#include <vector>
#include <string>

namespace windows_mini_lib {
	typedef std::vector<std::string> STRINGS;
	struct ColumnSortedState {
		bool visual_arrow_flag;
		std::map <unsigned short, unsigned short> map_sorted_row_no;
		ColumnSortedState() : visual_arrow_flag(false) {
		}
		void SetVisualArrowFlag() { visual_arrow_flag = !visual_arrow_flag; }
	};

	inline 
	bool ExtractFirstNumericFromCString(CString& cstring_param, int& result) {
		std::string string_param = cstring_param.GetBuffer(cstring_param.GetLength());
		auto first_index = string_param.find_first_of("0123456789");
		if (first_index != std::string::npos) {
			auto last_index = string_param.find_first_not_of("0123456789", first_index);
			if (last_index != std::string::npos)
				result = atoi(std::string(string_param.substr(first_index, last_index - first_index)).c_str());
			else
				result = atoi(std::string(string_param.substr(first_index)).c_str());
			return true;
		}
		return false;
	}

	inline
	void SetListCtrlStyle(CListCtrl* list_ctrl) {
		LONG lStyle = GetWindowLong(list_ctrl->m_hWnd, GWL_STYLE);				// Get the style of the current window.
		lStyle &= ~LVS_TYPEMASK;												// 清除显示方式位
		lStyle |= LVS_REPORT;													// Set style
		SetWindowLong(list_ctrl->m_hWnd, GWL_STYLE, lStyle);					// Set style
		DWORD dwStyle = list_ctrl->GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_VSCROLL;
		list_ctrl->SetExtendedStyle(dwStyle);									// Set extended style
	}

	inline
	std::string GetCurrentTime() {
		SYSTEMTIME system_time;
		GetLocalTime(&system_time);
		char tmp[256] = { 0 };
		sprintf(tmp, "%4d/%02d/%02d %02d:%02d:%02d",
			system_time.wYear, system_time.wMonth, system_time.wDay,
			system_time.wHour, system_time.wMinute, system_time.wSecond);
		return std::string(tmp);
	}

	inline
	bool CopyContentToClipboard(HWND window_handle, CString copy_content) {
		if (!OpenClipboard(window_handle) || !EmptyClipboard()) {
			return false;
		}
		std::size_t sizeof_content = (copy_content.GetLength() + 1) * sizeof (TCHAR);
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, sizeof_content);
		memcpy_s(GlobalLock(hData), sizeof_content, copy_content.LockBuffer(), sizeof_content);
		GlobalUnlock(hData);
		copy_content.UnlockBuffer();
		UINT uiFormat = (sizeof (TCHAR) == sizeof (WCHAR)) ? CF_UNICODETEXT : CF_TEXT;
		if (!::SetClipboardData(uiFormat, hData)) {
			CloseClipboard();
			return false;
		}
		CloseClipboard();
		return true;
	}

	// User doestn't need to check the parser result cause the size of the vector returned is at least 1.
	inline
	STRINGS StringParser(std::string& str, std::string parser_flag) {
		STRINGS results;
		std::size_t begin_index = 0;
		std::size_t end_index = 0;
		while ((begin_index = str.find_first_not_of(parser_flag, begin_index)) != std::string::npos) {
			if ((end_index = str.find_first_of(parser_flag, begin_index)) != std::string::npos) {
				results.push_back(std::string(str.substr(begin_index, end_index - begin_index)));
				begin_index = end_index;
			} else {
				results.push_back(std::string(str.substr(begin_index)));
				break;
			}
		}
		if (results.empty())
			results.push_back(str);
		return results;
	}
	
	template<typename Tp> 
	inline
	int OutFromBuf(Tp & data, const char * pBuf) {
		memcpy(&data, pBuf, sizeof(data));
		return sizeof(data);
	}
	
	inline 
	int OutStrFromBuf(std::string& data, const char* pBuf) {
		int read_size = 0;
		unsigned short string_size = 0;
		read_size += OutFromBuf(string_size, pBuf);
		if (string_size != 0) {
			vector <char> tmp;
			tmp.insert(tmp.end(), pBuf + read_size, pBuf + read_size + string_size);
			read_size += string_size;
			tmp.push_back(0);
			data = &*tmp.begin();
		}
		return read_size;
	}
	// This function adjusts the position of the ctrl out of proportion.
	// I didn't come out with a better solution right now.
	//inline
	//void adjustCtrlPosition(CWnd* pWnd, int x, int y, const CRect& originalPosition) {
	//	if (pWnd) {
	//		CRect rect;
	//		pWnd->GetWindowRect(&rect);
	//		pWnd->ScreenToClient(&rect);				// Transform the size of the control into the region coordinate in the dialog.
	//		// Adjust the control to the new position
	//		rect.left = rect.left * x / originalPosition.Width();
	//		rect.top = rect.top * y / originalPosition.Height();
	//		rect.bottom = rect.bottom * y / originalPosition.Height();
	//		rect.right = rect.right * x / originalPosition.Width();
	//		pWnd->MoveWindow(rect);
	//	}
	//}
	inline
	void autoAdjustColumnWidth(CListCtrl* listCtrl) {
		// Turn drawing off.
		listCtrl->SetRedraw(FALSE);
		CHeaderCtrl* pHeader = listCtrl->GetHeaderCtrl();
		int columnCount = pHeader->GetItemCount();
		for (int i = 0; i < columnCount; ++i) {
			// It seems LVSCW_AUTOSIZE_USEHEADER can adjust the width of the column based on both the content of
			// the header and that of the cell.
			/*listCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE);
			int columnWidth = listCtrl->GetColumnWidth(i);*/
			listCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
			/*int headerWidth = listCtrl->GetColumnWidth(i);
			listCtrl->SetColumnWidth(i, max(columnWidth, headerWidth));*/
		}
		// Turn drawing back on and update the window.
		listCtrl->SetRedraw(TRUE);
		// Invalidate the entire control, forcing painting.
		listCtrl->Invalidate();
		listCtrl->UpdateWindow();
	}
};
#endif 