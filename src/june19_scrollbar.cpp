// Lic:
// src/june19_scrollbar.cpp
// June 19 - Scrollbars
// version: 23.05.11
// Copyright (C) 2020, 2021, 2023 Jeroen P. Broks
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
#include <june19_scrollbar.hpp>

namespace Slyvina {
namespace June19 {

	using namespace Units;
	using namespace TQSG;
	using namespace TQSE;

	static void SEr(std::string msg) {
		std::cout << "June 19 - Scrollbar Error: " << msg << "\x7\n";
	}


	static void ActionScrollV(j19gadget* source, j19action action) {

	}

	static void DrawScrollV(j19gadget* g) {
		auto prnt{ g->GetParent() };
		auto
			X{ prnt->DrawX() + prnt->W() },
			Y{ prnt->DrawY() };
		SetColor(prnt->FR, prnt->FG, prnt->FB, 255);
		Rect(X , prnt->DrawY(), 16, prnt->H());
		SetColor(prnt->BR, prnt->BG, prnt->BB, 255);
		for (int i = 1; i < 7; i++) {
			Rect(X + 8 - i, Y + 2+i, i * 2, 1);
			Rect(X + 8 - i, (Y + prnt->H()) - (2 + i), i * 2, 1);
		}
		if (MouseHit(1) && MouseX() > X && MouseX() < X + 16) {
			if (MouseY() > Y && MouseY() < Y + 16) prnt->SetScrollY(prnt, prnt->ScrollY - 1);
			else if (MouseY() > Y + prnt->H() - 16 && MouseY() < Y + prnt->H()) prnt->SetScrollY(prnt, prnt->ScrollY + 1);
		}
	}

	j19gadget* AttachScrollV(j19gadget* parent) {
		if (!parent) { SEr("Null parent"); return nullptr; }
		switch (parent->GetKind()) {
		case j19kind::ListBox:
			break;
		default:
			SEr("Parent not allowed");
			return nullptr;
		}
		if (parent->W() < 50) {
			SEr("Parent width too low");
			return nullptr;
		}
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::VertScrollBar, DrawScrollV);
		}
		parent->W(parent->W() - 16);
		ret->SetParent(parent);
		ret->W(32);
		ret->H(parent->H()); // set for good measure, as these are actually not needed.
		ret->SetKind(j19kind::VertScrollBar);
		//ret->CBAction(ActionScrollV); // Doesn't work? Fuck that!
		return ret;
	}
}