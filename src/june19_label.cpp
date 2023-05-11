// Lic:
// src/june19_label.cpp
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

#include "june19_core.hpp"
#include "june19_label.hpp"

namespace june19{

	static std::string _error;
	using namespace TrickyUnits;

	static void DrawLabel(j19gadget*g){
		j19chat("Drawing label: " << g->Caption << "(" << g->DrawX() << "," << g->DrawY() << ")");
		_error = "";
		auto F{ g->Font() };
		if (!F) { _error = "No font for label"; j19chat("Font issue");  return; }
		if (g->BA) {
			TQSG_ACol(g->BR, g->BG, g->BB, g->BA);
			TQSG_Rect(g->DrawX(), g->DrawY(), g->W(), g->H());
		}
		TQSG_ACol(g->FR, g->FG, g->FB, g->FA);
		switch (g->IntFlag) {
		case 0:
			F->Draw(g->Caption, g->DrawX(), g->DrawY());
			break;
		case 1:
			F->Draw(g->Caption, g->DrawX()+g->W(), g->DrawY(),1);
			break;
		case 2:
			F->Draw(g->Caption, g->DrawX() + floor(g->W()/2), g->DrawY(), 2);
			break;
		default:
			_error = "Unknown label alignment flag: " + std::to_string(g->IntFlag);
		}
		
	}


	j19gadget* CreateLabel(std::string name, int x, int y, int w, int h, j19gadget* group, int style) {
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::Label,DrawLabel);
		}
		_error = "";
		ret->SetKind(j19kind::Panel);
		ret->Caption = name;
		ret->X(x);
		ret->Y(y);
		ret->W(w);
		ret->H(h);
		ret->SetParent(group);
		ret->IntFlag = style;
		return ret;
	}
	j19gadget* CreateLabelR(std::string name, int x, int y, int w, int h, j19gadget* group, int style) {
		auto ret{ CreateLabel(name,x,y,w,h,group,style) };
		ret->X(x, j19ctype::Percent);
		ret->Y(y, j19ctype::Percent);
		ret->W(w, j19ctype::Percent);
		ret->H(h, j19ctype::Percent);
		return ret;
	}

	static void DrawPanel(j19gadget* g){
		TQSG_ACol(g->BR, g->BG, g->BB, g->BA);
		TQSG_Rect(g->DrawX(), g->DrawY(), g->W(), g->H());
	}
	j19gadget* CreatePanel(int x, int y, int w, int h, j19gadget* group,j19ctype a) {
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::Panel, DrawLabel);
		}
		_error = "";
		ret->SetKind(j19kind::Panel);
		ret->X(x,a);
		ret->Y(y,a);
		ret->W(w,a);
		ret->H(h,a);
		ret->BA = 255;
		ret->SetParent(group);
		return ret;
	}
}