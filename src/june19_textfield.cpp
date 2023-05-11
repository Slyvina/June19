// Lic:
// src/june19_textfield.cpp
// June 19
// version: 21.10.11
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
#include "june19_textfield.hpp"
#include <QuickString.hpp>

using namespace std;
using namespace TrickyUnits;

namespace june19 {

	void DrawTextfield(j19gadget*g){
		auto deler{ 1 }; if (!g->RecEnabled()) deler = 2;
		auto co{ (SDL_GetTicks() / 500) % 2 };
		string cur{ " " };
		auto act{ g->Active() };
		TQSG_ACol(g->BR / deler, g->BG / deler, g->BB / deler, g->BA);
		TQSG_Rect(g->DrawX(), g->DrawY(), g->W(), g->H());
		TQSG_ACol(g->FR / deler, g->FG / deler, g->FB / deler, g->FA);
		if (act) {
			TQSG_Rect(g->DrawX(), g->DrawY(), g->W(), g->H(),true);
			if (g->RecEnabled()) {
				if (co) cur = "_";
				auto k{ TQSE_GetChar() };
				if (TQSE_KeyDown(SDLK_LCTRL) || TQSE_KeyDown(SDLK_RCTRL)) k = 0;
				switch (k) {
				case '\b':
					if (g->Text.size()) {
						g->Text = TrickyUnits::left(g->Text, g->Text.size() - 1);
						j19callback(g, BackSpace);
					}
					break;
				case '\t':
					break; // Sorry, no tabs allowed. Could btw also serve for next textfield, but that's not worth the trouble, honest!
				case '\r':
					j19callback(g, Enter);
				default:
					//cout << "Keyhit for char: " << k << endl;
					if (k >= 32 && k <= 126) {
						auto txt{ g->Text };
						txt += k;
						//cout << "DEBUG TEXTFIELD TYPE: Received key: " << k << ";\t Text:\"" << txt << "_" << "\";\t TextWidth:" << g->Font()->TextWidth(string(txt + "_").c_str()) << ";\tGadgetWidth:" << (g->W() - 4) << endl;
						if (g->Font()->TextWidth(string(txt + "_").c_str()) <= g->W() - 4) {
							g->Text = txt;
							j19callback(g, Type);
						}
					}
				}
			}
		}
		g->Font()->Draw(g->Text + cur, g->DrawX() + 2, g->DrawY() + (g->H() / 2), 0, 2);
		//cout << "AutoResize: " << g->AutoResize << " H:" << g->H() << " FontSize+4" << (g->FontHeight() + 4) << "\n";
		if (g->AutoResize) {
			if (g->H() < g->FontHeight()+4) g->H(g->H() + 1);
			else if (g->H() > g->FontHeight()+4) g->H(g->H() - 1);			
		}
		if (TQSE_MouseHit(1) && TQSE_MouseX() >= g->DrawX() && TQSE_MouseY() >= g->DrawY() && TQSE_MouseX() <= g->DrawX() + g->W() && TQSE_MouseY() <= g->DrawY() + g->H()) { g->Activate(); }
	}

	static string _error{ "" };
	j19gadget* june19::CreateTextfield(int x, int y, int w, j19gadget* ouwe, std::string defaultvalue) {
		return CreateTextfield(x, y, w, 0, ouwe, defaultvalue);
	}

	j19gadget* CreateTextfield(int x, int y, int w, int h, j19gadget* ouwe, std::string defaultvalue) {
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::Textfield, DrawTextfield);
		}
		_error = "";
		ret->SetKind(j19kind::Textfield);
		ret->Caption = "";
		ret->Text = defaultvalue;
		ret->X(x);
		ret->Y(y);
		ret->W(w);
		ret->H(h);
		ret->SetParent(ouwe);
		ret->BA = 110;
		ret->AutoResize = h<=0;
		return ret;

	}
}