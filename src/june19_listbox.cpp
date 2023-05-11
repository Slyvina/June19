// Lic:
// src/june19_listbox.cpp
// Listbox - June19
// version: 21.10.11
// Copyright (C) 2021 Jeroen P. Broks
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
#include "june19_listbox.hpp"
#include "june19_scrollbar.hpp"
#include <QuickString.hpp>

#include <TrickyMath.hpp>
using namespace TrickyUnits;

namespace june19 {
	static std::string _error;
	int DoubleClickSpd{ 100 };

	static void SetScroll(j19gadget* g, int i) {
		auto fnt = g->Font();
		int
			Y = 0,
			h = fnt->TextHeight("ABC"),
			maxi = QMAX(0,g->NumItems() - (g->H() / QMAX(1,h)));
		g->ScrollY = QMIN(maxi, i);
		g->ScrollY = QMAX(0, g->ScrollY);
		 std::cout << "Scroll request for listbox $" << (unsigned int)g << "  scroll request=" << i << " maxi=" << maxi << " after correction> " << g->ScrollY << std::endl; // debug only!
	}

	static void DrawListBox(j19gadget* g) {
		static int LastClick{ 0 }; if (LastClick) LastClick--;
		auto deler{ 1 }; if (!g->Enabled) deler = 2;
		TQSG_ACol(g->BR/deler, g->BG/deler, g->BB/deler, g->BA);
		TQSG_Rect(g->DrawX(), g->DrawY(), g->W(), g->H());
		auto fnt = g->Font();
		int Y = 0;
		int h = fnt->TextHeight("ABC");
		if (TQSE_MouseWheelY() != 0 && TQSE_MouseX() > g->DrawX() && TQSE_MouseY() > g->DrawY() && TQSE_MouseX() < g->DrawX() + g->W() && TQSE_MouseY() < g->DrawY() + g->H()) {
			SetScroll(g,g->ScrollY - TQSE_MouseWheelY());
		}
		for (unsigned long long idx = g->ScrollY; idx < g->NumItems() && Y+h<g->H(); ++idx) {
			TQSG_ACol(g->FR/deler, g->FG/deler, g->FB / deler, g->FA);
			auto txt = g->ItemText(idx);
			//std::cout << "show: "<<idx<<txt<<"\n";
			while (txt != "" && fnt->TextWidth(txt.c_str())>=g->W()) txt = left(txt, txt.size() - 1);
			if (g->SelectedItem() == idx) {
				TQSG_ACol(g->FR, g->FG, g->FB, 255);
				TQSG_Rect(g->DrawX(), g->DrawY()+Y, g->W(), h);
				TQSG_ACol(g->BR, g->BG, g->BB, 255);
			}
			//std::cout << "dlbdebug: Enabled=" << (int)g->Enabled << "; MX=" << TQSE_MouseX() << "; MY=" << TQSE_MouseY() << "; DX=" << g->DrawX() << "\n";
			if (g->Enabled && TQSE_MouseX() > g->DrawX() && TQSE_MouseY() > g->DrawY() + Y && TQSE_MouseX() < g->DrawX() + g->W() && TQSE_MouseY() < g->DrawY()+Y+h && TQSE_MouseHit(1)) {
				g->SelectItem(idx);
				//std::cout << "DEBUG LISTBOX: SELECT ITEM> " << idx << std::endl;
				auto a{ j19action::Select };
				if (LastClick) a = j19action::DoubleClick;
				if (g->CBAction) g->CBAction(g, a);
				LastClick = DoubleClickSpd;
			}
			fnt->Draw(txt, g->DrawX(), g->DrawY() + Y);
			Y += h;
		}
	}

	j19gadget* CreateListBox(int x, int y, int w, int h, j19gadget* parent, j19ctype ct,bool scroll) {
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::ListBox, DrawListBox);
		}
		_error = "";
		ret->SetKind(j19kind::ListBox);
		ret->X(x, ct);
		ret->Y(y, ct);
		ret->W(w, ct);
		ret->H(h, ct);
		ret->BA = 255;
		ret->SetParent(parent);
		ret->SetScrollY = SetScroll;
		if (scroll) AttachScrollV(ret);
		return ret;
	}

}