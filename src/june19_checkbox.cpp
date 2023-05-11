// Lic:
// src/june19_checkbox.cpp
// June 19
// version: 21.04.02
// Copyright (C) 2020, 2021 Jeroen P. Broks
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
#include "june19_checkbox.hpp"

using namespace std;
using namespace TrickyUnits;

namespace june19 {
	static string _error{ "" };

	static void DrawCheckBox(j19gadget* g) {
		auto Enab{ g->RecEnabled() };
		auto Deler{ 1 }; if (!Enab) Deler = 2;
		j19chat("Drawing Checkbox: " << g->Caption << "(" << g->DrawX() << "," << g->DrawY() << ")");
		_error = "";
		auto F{ g->Font() };
		if (!F) { _error = "No font for checkbox"; j19chat("Font issue");  return; }
		if (g->BA) {
			TQSG_ACol(g->BR, g->BG, g->BB, g->BA);
			TQSG_Rect(g->DrawX(), g->DrawY(), g->W(), g->H());
		}
		TQSG_ACol(g->FR / Deler, g->FG / Deler, g->FB / Deler, g->FA);
		auto sqs{ g->FontHeight() };
		TQSG_Rect(g->DrawX(), g->DrawY(), sqs, sqs, !g->checked);
		if (Enab && TQSE_MouseHit(1) && TQSE_MouseX() >= g->DrawX() && TQSE_MouseY() >= g->DrawY() && TQSE_MouseX() <= g->DrawX() + sqs && TQSE_MouseY() <= g->DrawY() + sqs) {
			g->checked = !g->checked;
			auto a{ j19action::Check };
			if (!g->checked) a = j19action::UnCheck;
			if (g->CBAction) g->CBAction(g, a);
		}
		F->Draw(g->Caption, g->DrawX() + (sqs * 1.5), g->DrawY());
	}


    j19gadget* CreateCheckBox(std::string Caption, int x, int y, int w, int h, j19gadget* parent, bool checkedbydefault) {
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::CheckBox, DrawCheckBox);
		}
		_error = "";
		ret->SetKind(j19kind::CheckBox);
		ret->Caption = Caption;
		ret->X(x);
		ret->Y(y);
		ret->W(w);
		ret->H(h);
		ret->SetParent(parent);
		ret->IntFlag = 0;
		ret->checked = checkedbydefault;
		return ret;
	}
}