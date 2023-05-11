// Lic:
// src/june19_tabber.cpp
// June 19
// version: 20.11.30
// Copyright (C) 2020 Jeroen P. Broks
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic

#include "june19_tabber.hpp"

using namespace std;
using namespace TrickyUnits;

namespace june19 {


	static string _error;	

	static void TabDraw(j19gadget* g) {
		// TODO: Actual code
		int rx{ 0 }, ry{ 0 }, rw{ g->W() }, rh{ g->H() };
		auto f{ g->Font() };
		auto fs{ g->FontHeight() };		
		g->EventSelectItem = false;
		if (!f) { _error = "No Font"; return; }
		for (int i = 0; i < g->NumItems(); i++) {
			if (g->SelectedItem() < 0) g->SelectItem((i)); // In tabbers there MUST always be a selected item, if there's none, we'll make one!
			auto item{ g->__ITEM(i) };
			if (!item->Kid()) item->CreateKid();
			int w{ 4 + f->TextWidth(item->Caption.c_str()) };
			int h{ 4 + fs }; 
			int x{ rx }; rx += w; 
			if (x && rx > g->W()) { rx = 0; ry += fs + 4; }
			int y{ ry }; rh = g->H() - (y+h);
			if (TQSE_MouseHit(1) && TQSE_MouseX() > x + g->DrawX() && TQSE_MouseX() + x + w + g->DrawX() && TQSE_MouseY() > y + g->DrawY() && TQSE_MouseY() < y + h + g->DrawY()) {
				g->EventSelectItem = true;
				g->SelectItem(i);
				if (g->CBAction) g->CBAction(g, j19action::Select);
			}
			TQSG_ACol(g->BR, g->BG, g->BB, g->BA);
			TQSG_Rect(x+g->DrawX(), y+g->DrawY(), w, h);
			TQSG_ACol(g->FR, g->FG, g->FB, g->FA);
			TQSG_Rect(x+g->DrawX(), y+g->DrawY(), w, h, i != g->SelectedItem());
			if (i == g->SelectedItem()) TQSG_ACol(g->BR, g->BG, g->BB, 255); // Ignore Alpha on that one
			f->Draw(item->Caption, x + (w / 2)+g->DrawX(), y + (h / 2)+g->DrawY(), 2, 2);
			
		}
		// The currently Active Panel
		for (int i = 0; i < g->NumItems(); i++) {
			auto Tab{ GetTab(g,i) }; if (!Tab) { _error = "Something went wrong when retrieving the active tab for drawing"; return; }
			Tab->X(2);
			Tab->Y(ry + fs + 2);
			Tab->W(g->W() - 4);
			Tab->H(rh - 4);
			TQSG_ACol(g->FR, g->FG, g->FB, g->FA);
			if (i == g->SelectedItem()) TQSG_Rect(Tab->DrawX(), Tab->DrawY(), Tab->W(), Tab->H(), true);
			Tab->Visible = i == g->SelectedItem();
			//Tab->Draw();
		}
	}

	j19gadget* CreateTabber(int x, int y, int w, int h, j19gadget* group) {
		static auto init{ j19gadget::RegDraw(j19kind::Tabber, TabDraw) };
		auto ret{ new j19gadget() };
		_error = "";
		ret->SetKind(j19kind::Tabber);
		ret->X(x);
		ret->Y(y);
		ret->W(w);
		ret->H(h);
		ret->SetParent(group);
		ret->IntFlag = 0;
		return ret;
	}

	j19gadget* GetTab(j19gadget*g, size_t idx) {
		if (g->GetKind() != j19kind::Tabber) { _error = "Can only get tabs from tabbers"; return nullptr; }
		if (idx >= g->NumItems()) { _error = "Tab index out of range ("+to_string(idx)+"/"+to_string(g->NumItems())+")"; return nullptr; }
		auto Tab{ g->__ITEM(idx) };
		if (!Tab->Kid()) Tab->CreateKid();
		return Tab->Kid();
	}

	j19gadget* GetTab(j19gadget*g) {
		if (g->SelectedItem() < 0) { _error = "No tab yet selected! This is required before you do this!"; return nullptr; }
		return GetTab(g,g->SelectedItem());
	}

	j19gadget* AddTab(j19gadget* parent, std::string Caption) {
		if (parent->GetKind() != j19kind::Tabber) { _error = "Can only add tabs to tabbers"; return nullptr; }
		parent->AddItem(Caption);
		auto Tab{ GetTab(parent,parent->NumItems() - 1) };
		if (!Tab) { _error = "BIG ERROR! Tab creation failed! ("+Caption+")\n"+_error; cout << "\7" << _error << endl; return nullptr; }
		Tab->X(0);
		Tab->Y(0);
		Tab->W(parent->W());
		Tab->H(parent->H());

		return Tab;
	}

	std::string GetTabError() {
		return _error;
	}
}