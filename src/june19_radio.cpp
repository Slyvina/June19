// License:
// 	June19/src/june19_radio.cpp
// 	June 19 Radio Button
// 	version: 24.12.07
// 
// 	Copyright (C) 2024 Jeroen P. Broks
// 
// 	This software is provided 'as-is', without any express or implied
// 	warranty.  In no event will the authors be held liable for any damages
// 	arising from the use of this software.
// 
// 	Permission is granted to anyone to use this software for any purpose,
// 	including commercial applications, and to alter it and redistribute it
// 	freely, subject to the following restrictions:
// 
// 	1. The origin of this software must not be misrepresented; you must not
// 	   claim that you wrote the original software. If you use this software
// 	   in a product, an acknowledgment in the product documentation would be
// 	   appreciated but is not required.
// 	2. Altered source versions must be plainly marked as such, and must not be
// 	   misrepresented as being the original software.
// 	3. This notice may not be removed or altered from any source distribution.
// End License
// Lic:
// src/june19_radio.cpp
// June 19 - Radio Buttons
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
#include "june19_radio.hpp"

using namespace std;
using namespace Slyvina::Units;
using namespace Slyvina::TQSG;
using namespace Slyvina::TQSE;

namespace Slyvina {
	namespace June19 {

		static string _error{ "" };
		static void DrawRadio(j19gadget* g) {
			auto Enab{ g->RecEnabled() };
			auto Deler{ 1 }; if (!Enab) Deler = 2;
			j19chat("Drawing label: " << g->Caption << "(" << g->DrawX() << "," << g->DrawY() << ")");
			_error = "";
			auto F{ g->Font() };
			if (!F) { _error = "No font for label"; j19chat("Font issue");  return; }
			if (g->BA) {
				SetColor(g->BR, g->BG, g->BB, g->BA);
				Rect(g->DrawX(), g->DrawY(), g->W(), g->H());
			}
			SetColor(g->FR / Deler, g->FG / Deler, g->FB / Deler, g->FA);
			int radius{ min(g->FontHeight() / 2 ,12) };
			Circle(g->DrawX() + radius, g->DrawY() + radius, radius);
			if (Enab && MouseHit(1)) {
				auto hz{ abs((g->DrawX() + radius) - MouseX()) };
				auto vt{ abs((g->DrawY() + radius) - MouseY()) };
				auto dist{ sqrt((hz * hz) + (vt * vt)) }; // Pythagoras all the way, folks!
				// cout<< "Radio Pythagoras: " << dist << " <= " << radius << "\t"<< g->Caption<<"\n";
				if (dist <= radius) {
					for (size_t i = 0; g->GetParent()->Kid(i); i++) {
						if (g->GetParent()->Kid(i)->GetKind()==j19kind::RadioButton) g->GetParent()->Kid(i)->checked = false; // g->GetParent()->Kid(i) == g;

					}
					g->checked = true; // Safety, but should be unneeded (should be and to be are 2 different things)
					// cout << "\x07 CBAction:" << (int)g->CBAction << " (" << g->Caption << ")\n"; // debug
					if (g->CBAction) g->CBAction(g, j19action::Check);
				}
			}
			if (g->checked) for (int i = radius; i; --i) Circle(g->DrawX() + radius, g->DrawY() + radius, i);
			F->Text(g->Caption, g->DrawX() + (radius * 3), g->DrawY());
		}

		j19gadget* CreateRadioButton(std::string caption, int x, int y, int w, int h, j19gadget* mommy, bool defaultvalue) {
			static auto init{ false };
			auto ret{ new j19gadget() };
			if (!init) {
				j19gadget::RegDraw(j19kind::RadioButton, DrawRadio);
			}
			_error = "";
			ret->SetKind(j19kind::RadioButton);
			ret->Caption = caption;
			ret->X(x);
			ret->Y(y);
			ret->W(w);
			ret->H(h);
			ret->SetParent(mommy);
			ret->IntFlag = 0;
			ret->checked = defaultvalue;
			return ret;
			//return nullptr;
		}
	}
}
