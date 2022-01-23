/* SampleHive
 * Copyright (C) 2021  Apoorv Singh
 * A simple, modern audio sample browser/manager for GNU/Linux.
 *
 * This file is a part of SampleHive
 *
 * SampleHive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SampleHive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "wx/dataview.h"
#include "wx/string.h"
#include "wx/treectrl.h"
#include "wx/treebase.h"
#include "wx/variant.h"
#include "wx/vector.h"

#include <string>

namespace SampleHive {

    class cHiveData
    {
        private:
            cHiveData() = default;

        public:
            cHiveData(const cHiveData&) = delete;
            cHiveData& operator=(const cHiveData) = delete;

        public:
            static cHiveData& Get()
            {
                static cHiveData s_HiveData;
                return s_HiveData;
            }

        public:
            // ===============================================================
            // HivesPanel functions
            void InitHiveData(wxDataViewListCtrl& listCtrl, wxDataViewTreeCtrl& hives, wxDataViewItem favoriteHive,
                              wxTreeCtrl& trash, wxTreeItemId trashRoot)
            {
                m_pListCtrl = &listCtrl;
                m_FavoriteHive = favoriteHive;
                m_pHives = &hives;
                m_TrashRoot = trashRoot;
                m_pTrash = &trash;
            }

            inline wxDataViewTreeCtrl& GetHivesObj() { return *m_pHives; }
            inline wxDataViewItem& GetFavoritesHive() { return m_FavoriteHive; }

            wxString GetHiveItemText(bool ofFavHive = false, wxDataViewItem hive = wxDataViewItem(0))
            {
                wxString item_text;

                if (ofFavHive)
                    item_text = m_pHives->GetItemText(m_FavoriteHive);
                else
                    item_text = m_pHives->GetItemText(hive);

                return item_text;
            }

            inline wxDataViewItem GetHiveItemSelection() { return m_pHives->GetSelection(); }
            inline bool IsHiveItemContainer(wxDataViewItem& hiveItem) { return m_pHives->IsContainer(hiveItem); }
            inline int GetHiveChildCount(wxDataViewItem& root) { return m_pHives->GetChildCount(root); }
            inline wxDataViewItem GetHiveNthChild(wxDataViewItem& root, int pos) { return m_pHives->GetNthChild(root, pos); }
            inline void HiveAppendItem(wxDataViewItem& hiveItem, wxString name) { m_pHives->AppendItem(hiveItem, name); }
            inline void HiveDeleteItem(wxDataViewItem& hiveItem) { m_pHives->DeleteItem(hiveItem); }

            // ===============================================================
            // TrashPanel functions
            inline wxTreeCtrl& GetTrashObj() { return *m_pTrash; }
            inline wxTreeItemId& GetTrashRoot() { return m_TrashRoot; }
            inline void TrashAppendItem(const wxTreeItemId& parent, const wxString& text) { m_pTrash->AppendItem(parent, text); }

            // ===============================================================
            // ListCtrl functions
            inline wxDataViewListCtrl& GetListCtrlObj() { return *m_pListCtrl; }
            inline int GetListCtrlSelections(wxDataViewItemArray& items) { return m_pListCtrl->GetSelections(items); }
            inline int GetListCtrlRowFromItem(wxDataViewItemArray& items, int index) { return m_pListCtrl->ItemToRow(items[index]); }
            inline int GetListCtrlSelectedRow() { return m_pListCtrl->GetSelectedRow(); }
            inline wxDataViewItem GetListCtrlItemFromRow(int row) { return m_pListCtrl->RowToItem(row); }
            inline wxString GetListCtrlTextValue(unsigned int row, unsigned int col) { return m_pListCtrl->GetTextValue(row, col); }
            inline int GetListCtrlItemCount() { return m_pListCtrl->GetItemCount(); }
            inline void ListCtrlAppendItem(const wxVector<wxVariant>& values) { m_pListCtrl->AppendItem(values); }
            inline void ListCtrlSetVariant(const wxVariant& variant, unsigned int row, unsigned int col)
                                     { m_pListCtrl->SetValue(variant, row, col); }
            inline void ListCtrlUnselectAllItems() { m_pListCtrl->UnselectAll(); }
            inline void ListCtrlSelectRow(int row) { m_pListCtrl->SelectRow(row); }
            inline void ListCtrlEnsureVisible(const wxDataViewItem& item) { m_pListCtrl->EnsureVisible(item); }
            inline void ListCtrlDeleteItem(unsigned int row) { m_pListCtrl->DeleteItem(row); }
            inline void ListCtrlDeleteAllItems() { m_pListCtrl->DeleteAllItems(); }

        private:
            wxDataViewListCtrl* m_pListCtrl = nullptr;
            wxDataViewItem m_FavoriteHive;
            wxDataViewTreeCtrl* m_pHives = nullptr;
            wxTreeCtrl* m_pTrash = nullptr;
            wxTreeItemId m_TrashRoot;
    };

}
