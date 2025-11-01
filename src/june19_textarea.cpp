// License:
// 
// June 19th
// Textarea (code)
// 
// 
// 
// 	(c) Jeroen P. Broks, 2025
// 
// 		This program is free software: you can redistribute it and/or modify
// 		it under the terms of the GNU General Public License as published by
// 		the Free Software Foundation, either version 3 of the License, or
// 		(at your option) any later version.
// 
// 		This program is distributed in the hope that it will be useful,
// 		but WITHOUT ANY WARRANTY; without even the implied warranty of
// 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// 		GNU General Public License for more details.
// 		You should have received a copy of the GNU General Public License
// 		along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// 	Please note that some references to data like pictures or audio, do not automatically
// 	fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 25.11.01 I
// End License

#include <TQSG.hpp>
#include "../head/june19_core.hpp"

#define MyMax(a,b) (a>b?a:b);
#define MyMin(a,b) (a<b?a:b);


using namespace Slyvina::TQSG;
using namespace Slyvina::TQSE;
using namespace Slyvina::Units;

namespace Slyvina {
	namespace June19 {

		static String _error{};

		void DrawTextArea(j19gadget*g) {
			static char altchar{0};
			SDL_KeyCode Key{GetKey()};
			char Chr{GetChar()};
			int deler{g->RecEnabled()?1:2};
			SetColor(g->BR/deler,g->BG/deler,g->BB/deler,g->BA);
			Rect(g->DrawX(),g->DrawY(),g->W(),g->H());
			if (g->Active()) {
				if (KeyDown(SDLK_LALT) || KeyDown(SDLK_RALT)) {
					SetColor(255-g->FR,255-g->FG,255-g->FB,g->FA);
				} else {
					SetColor(g->FR/deler,g->FG/deler,g->FB/deler,g->FA);
				}
				Rect(g->DrawX(),g->DrawY(),g->W(),g->H(),true);
				SetColor(g->FR/deler,g->FG/deler,g->FB/deler,g->FA);
			}
			int lny=0;
			int th=0;
			for(size_t ln=g->ScrollY; lny+th<g->H() && ln<g->NumItems();ln++) {
				//printf("%03d:%03d> lny:%04d\n",ln,g->NumItems(),lny);
				int lnc{g->ScrollX},lnx{0};
				auto lt{g->ItemText(ln)};
				for(size_t cn=g->ScrollX;cn<=lt.size() && lnx<g->W();cn++) {
					std::string ch{cn<lt.size()?Units::Mid(lt,cn+1,1):" "};
					if (cn==g->_tax && ln==g->_tay) {
						SetColor(g->FR/deler,g->FG/deler,g->FB/deler,255);
						Rect(lnx+g->DrawX(),lny+g->DrawY(),g->Font()->Width(ch),g->Font()->Height(ch==" "?"THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG":ch),!g->Active());
						if (g->Active()) SetColor(g->BR,g->BG,g->BB);
					} else SetColor(g->FR/deler,g->FG/deler,g->FB/deler,g->FA);
					//printf("(%03d,%03d)[%02d,%02d]: \"%s\" <- \"%s\"\n",lnx,lny,cn,ln,ch.c_str(),lt.c_str());
					g->Font()->Text(ch,lnx+g->DrawX(),lny+g->DrawY());
					lnx+=g->Font()->Width(ch);
				}
				lny+=g->Font()->Height(lt);
			}
			if (g->Active() && g->RecEnabled()) {
				g->Font()->Dark(Units::TrSPrintF("TextArea: %03d:%03d(%03d:%03d)",g->_tax,g->_tay,g->NumItems(),g->ItemText(g->_tay).size()),5,5);
				if (!g->NumItems()) g->AddItem("");
				switch(Key) {
					case SDLK_UP:
						//g->_tay=MyMax(0,--g->_tay);
						if (g->_tay>0) --g->_tay;
						g->_tax=MyMin(g->_tax,g->ItemText(g->_tay).size());
						j19callback(g, Up);
						break;
					case SDLK_DOWN:
						//g->_tay=MyMin(g->NumItems()-1,++g->_tay);
						if (g->_tay<g->NumItems()-1) ++g->_tay;
						g->_tax=MyMin(g->_tax,g->ItemText(g->_tay).size());
						j19callback(g, Down);
						break;
					case SDLK_LEFT:
						if (g->_tax) g->_tax--;
						else if (g->_tay) { g->_tay--; g->_tax=g->ItemText(g->_tay).size(); }
						j19callback(g, Left);
						break;
					case SDLK_RIGHT:
						if (g->_tax<g->ItemText(g->_tay).size()) g->_tax++;
						else if (g->_tay<g->NumItems()) {
							g->_tay++;
							g->_tax=0;
							j19callback(g, Right);
						}
						break;
					case SDLK_HOME:
						g->_tax=0;
						j19callback(g, Home);
						break;
					case SDLK_END:
						g->_tax=g->ItemText(g->_tay).size();
						j19callback(g, End);
						break;
					default: {
						if (Chr>=32 && Chr<=126) {
							auto t{g->ItemText(g->_tay)};
							if (g->_tax==0) {
								t=Units::TrSPrintF("%c",Chr)+t;
								g->_tax++;
							} else if (g->_tax==t.size()) {
								t+=Chr;
								g->_tax++;
							} else
								t=t.substr(0,g->_tax++)+Units::TrSPrintF("%c",Chr)+t.substr(g->_tax);
							g->ItemText(g->_tay,t);
						}
						} break;
				}
			}
			if (MouseHit(1) && MouseX() >= g->DrawX() && MouseY() >= g->DrawY() && MouseX() <= g->DrawX() + g->W() && MouseY() <= g->DrawY() + g->H()) { g->Activate(); altchar=0; }
		}

		static void SetContent(j19gadget*g,std::string Text) {
			Text=Units::StReplace(Text,"\r","");
			auto SpT{Units::Split(Text,'\n')};
			g->ClearItems();
			for(auto&L:*SpT) g->AddItem(L);
			//std::cout << "DEBUG: TEXTAREACONTENT:\n"<<Text<<"\nEND\nLines:"<<g->NumItems()<<"\n";
		}

		static std::string GetContent(j19gadget*g) {
			std::string ret{""};
			for(size_t i=0;i<g->NumItems();i++) {
				auto l{g->ItemText(i)};
				ret+=ret.size()?"\n":"";
				ret+=ret.size()||l.size()?l:"";
			}
			return ret;
		}

		j19gadget* CreateTextArea(int _x, int _y, int _width, int _height,j19gadget* _parent,int flags) {
			std::cout << "CreateTextArea(" <<_x<<","<<_y<<","<<_width<<","<<_height<<",<parent>,"<<flags<<");\n";
			static auto init{ false };
			auto ret{ new j19gadget() };
			if (!init) {
				j19gadget::RegDraw(j19kind::TextArea, DrawTextArea);
				j19gadget::RegSetContent(j19kind::TextArea,SetContent);
				j19gadget::RegGetContent(j19kind::TextArea,GetContent);
			}
			_error = "";
			ret->SetKind(j19kind::TextArea);
			ret->Caption = "";
			ret->Text = "";
			ret->ClearItems();
			ret->X(_x);
			ret->Y(_y);
			ret->W(_width);
			ret->H(_height);
			ret->SetParent(_parent);
			ret->BA = 110;
			ret->AutoResize = false;
			ret->ScrollY=0;
			ret->ScrollX=0;
			return ret;
		}
	}
}
