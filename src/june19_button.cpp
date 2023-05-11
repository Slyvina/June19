// Lic:
// src/june19_button.cpp
// Button - June19
// version: 21.04.02
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
#include "june19_button.hpp"

using namespace TrickyUnits;

namespace june19{

	static std::string _error{ "" };

	void DrawButton(j19gadget*g){		
		// std::cout << g->Y() << " -> " << g->H() << " -> " << g->DrawY() << "\t Down:"<<g->Down<<std::endl;
		j19byte Deler{ 1 };
		auto Enabled{ g->RecEnabled() };
		auto showcapt{ true };
		auto hover{ TQSE_MouseX() > g->DrawX() && TQSE_MouseY() > g->DrawY() && TQSE_MouseX() < g->DrawX() + g->W() && TQSE_MouseY() < g->DrawY() + g->H() };
		if (!Enabled) Deler = 2;
		if (g->AutoResize) {
			auto tw{ g->Font()->TextWidth(g->Caption.c_str()) };
			auto th{ g->Font()->TextHeight(g->Caption.c_str()) };
			auto bw{ tw + 12 };
			auto bh{ th + 8 };
			if (g->W() < bw) { g->W(g->W() + 1); showcapt = false; } else if (g->W() > bw) { g->W(g->W() - 1); showcapt = false; }
			if (g->H() < bh) { g->H(g->H() + 1); showcapt = false; } else if (g->H() > bh) { g->H(g->H() - 1); showcapt = false; }
		}
		g->checked = false;
		if (!Enabled) g->Down = false;
		if (Enabled && hover) {
			if (TQSE_MouseHit(1)) g->Down = true;
		}
		if (TQSE_MouseReleased(1)) {
			g->Down = false;
			if (hover && Enabled) {
				g->checked = true;
				j19callback(g, Click);
			}
		}
		if (g->W() < 2 || g->H() < 2) return; // Too small to do anything!
		auto lix{ g->DrawX() }, rex{ g->DrawX() + g->W() };
		auto toy{ g->DrawY() }, boy{ g->DrawY() + g->H() };
		TQSG_ACol(g->BR / Deler, g->BG / Deler, g->BB / Deler, g->BA);
		TQSG_Rect(lix, toy, g->W(), g->H());
		// Bottom + Right
		if (g->Down) { TQSG_ACol(g->FR / Deler, g->FG / Deler, g->FB / Deler, 255); } else { TQSG_ACol((g->FR/2) / Deler, (g->FG/2) / Deler, (g->FB/2) / Deler, 255); }
		// Bottom
		TQSG_Line(lix + 1, boy, rex, boy);
		TQSG_Line(lix + 2, boy-1, rex, boy-1);
		// Right
		TQSG_Line(rex, boy, rex, toy + 1);
		TQSG_Line(rex-1, boy, rex-1, toy + 2);
		// Top + Left 
		if (!g->Down) { TQSG_ACol(g->FR / Deler, g->FG / Deler, g->FB / Deler, g->FA); } else { TQSG_ACol((g->FR / 2) / Deler, (g->FG / 2) / Deler, (g->FB / 2) / Deler, 255); }
		// Top
		TQSG_Line(lix, toy, rex - 1, toy);
		TQSG_Line(lix, toy+1, rex - 2, toy+1);
		// Left
		TQSG_Line(lix, toy, lix, boy - 1);
		TQSG_Line(lix + 1, toy, lix + 1, boy - 2);
		if (showcapt) {
			TQSG_ACol(g->FR / Deler, g->FG  / Deler, g->FB  / Deler, 255);
			g->Font()->Draw(g->Caption, lix + (g->W() / 2), toy + (g->H() / 2), 2, 2);
		}

	}

	j19gadget* CreateButton(std::string Caption, int x, int y, j19gadget* Parent) {
		return CreateButton(Caption,x,y,0,0,Parent);
	}

	j19gadget* CreateButton(std::string Caption, int x, int y, int w, int h, j19gadget* Parent) {
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::Button, DrawButton);
		}
		_error = "";
		ret->SetKind(j19kind::Button);
		ret->Caption = Caption;		
		ret->X(x);
		ret->Y(y);
		ret->W(w);
		ret->H(h);
		ret->SetParent(Parent);
		ret->BA = 110;
		ret->AutoResize = h <= 0 || x<=0;
		return ret;
	}
}