// License:
// 	June19/src/june19_core.cpp
// 	June 19
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

#undef DEBUG_PULLDOWNKEY

#include <TQSG.hpp>
#include <TQSE.hpp>

#include <SlyvString.hpp>
#include <SlyvHSVRGB.hpp>

#include "june19_core.hpp"

using namespace std;
using namespace Slyvina::Units;
using namespace Slyvina::TQSG;
using namespace Slyvina::TQSE;

// Macros are evil! I know!
// But I am evil, as well!

#define ggc(ac,tc,rc)\
	_error = "";\
	if (pure)\
		return ac;\
	switch (tx) { \
		case j19ctype::Absolute: return ac;\
		case j19ctype::Percent: \
			if (!parent) { _error = "No parent"; return 0; }\
			return (int)floor(parent->rc() * (ac / 100)); \
		default: \
			_error = "Unknown coordinate calculation type! (bug?) ("+to_string((int)tx)+")"; \
			return 0; \
	}

#define sgc(ac,tc)\
	if (kind == j19kind::Unknown || kind == j19kind::EntireScreen || kind == j19kind::WorkScreen) { _error = "This gadget type may not be modified"; return; } \
	ac = value; tc = t;


namespace Slyvina {
	namespace June19 {

		static string _error{ "" };
		static j19gadget _Screen;
		static j19gadget _WorkScreen;

		static vector <std::shared_ptr<j19pulldown>> _MenuBar{};

		std::string j19gadget::_StatusText{ "" };
		size_t j19gadget::__count{ 0 };

		static void DrawScreen(j19gadget* self) {} // This just had to exist, that's all!
		static void DrawWorkScreen(j19gadget* self) {
			// TODO: Pulldown
			if (j19gadget::ScreenHasPullDown()) {
				int h{ self->Font()->Height("ABC") };
				SetColor(self->BR, self->BG, self->BB, self->BA);
				Rect(0, 0, ScreenWidth(), h);
				SetColor(self->FR, self->FG, self->FB, 255);
				int mx = 0;
				//for (int i = 0; j19gadget::ScreenPullDown(i); i++) {
				//	auto itm = j19gadget::ScreenPullDown(i);
				for (auto itm : _MenuBar) {
					//std::cout << itm.Caption << endl;
					self->Font()->Text(itm->Caption.c_str(), mx + 10, 0);
					mx += 20 + self->Font()->Width(itm->Caption.c_str());
				}
				if ((KeyDown(SDLK_RCTRL) || KeyDown(SDLK_LCTRL)) && GetKey()) j19pulldown::ExeKey(GetKey());
			}
			if (j19gadget::ScreenHasStatus()) {
				//std::cout << j19gadget::StatusText() << "\n";
				auto sstat = *Split(j19gadget::StatusText(), '\t');
				auto lstat = sstat.size();
				auto tstat = (int)floor(ScreenWidth() / (lstat + 1));
				int h{ self->Font()->Height("ABC") };
				SetColor(self->BR, self->BG, self->BB, self->BA);
				Rect(0, ScreenHeight() - h, ScreenWidth(), h);
				SetColor(self->FR, self->FG, self->FB, 255);
				for (int i = 0; i < lstat; ++i) {
					//std::cout << i << ": " << sstat[i]<<std::endl;
					self->Font()->Text(sstat[i], tstat * i, ScreenHeight(), Align::Left, Align::Bottom);
				}
			}
			// TODO: Status
		}

		std::map<j19kind, j19draw> j19gadget::HowToDraw{};
		std::map<j19kind, j19draw> j19gadget::HowToDispose{};

		j19gadget* j19gadget::active{ nullptr };
		bool j19gadget::defaultfontloaded{ false };
		TImageFont j19gadget::_DefaultFont{ nullptr };
		bool j19gadget::haspulldown{ false };
		bool j19gadget::hasstatus{ false };
		bool j19gadget::Active() { return active == this; }
		void j19gadget::Activate() { active = this; j19callback(this, Activate); }

		void j19gadget::DeActivate() { active = nullptr; }
		j19kind j19gadget::GetKind() {
			return kind;
		}
		j19gadget* j19gadget::GetParent() { return parent; }
		bool j19gadget::ScreenHasPullDown() { return haspulldown; }
		j19pulldown* j19gadget::ScreenPullDown(int i) {
			if (i < _MenuBar.size()) return _MenuBar[i].get();
			return nullptr;
		}
		bool j19gadget::ScreenHasStatus() {
			return hasstatus;
		}
		bool j19gadget::RegDraw(j19kind k, j19draw v) {
			_error = "";
			if (HowToDraw.count(k)) {
				_error = "Duplicate kind draw registration";
				return false;
			}
			HowToDraw[k] = v;
			return true;
		}

		bool j19gadget::RegDispose(j19kind k, j19draw v) {
			_error = "";
			if (HowToDispose.count(k)) {
				_error = "Duplicate kind draw registration";
				return false;
			}
			HowToDispose[k] = v;
			return true;
		}

		void j19gadget::SetKind(j19kind value) {
			_error = "";
			if (kind != j19kind::Unknown)
				_error = "Kind already set for this gadget";
			kind = value;
		}

		void j19gadget::SetParent(j19gadget* value) {
			if (parent)
				return;
			else {
				parent = value;
				value->kids.push_back(this);
			}
		}

		void j19gadget::X(int value) { X(value, tx); }

		void j19gadget::X(int value, j19ctype t) {
			/* proto
			if (kind == j19kind::Unknown || kind == j19kind::EntireScreen || kind == j19kind::WorkScreen) { _error = "This gadget type may not be modified"; return; }
			_x = value; tx = t;
			// */
			sgc(_x, tx);
		}

		int j19gadget::X(bool pure) {
			/* proto
			_error = "";
			if (pure)
				return _x;
			switch (tx) {
			case j19ctype::Absolute: return _x;
			case j19ctype::Percent:
				if (!parent) { _error = "No parent"; return 0; }
				return (int)floor(parent->W() * (_x / 100));
			default:
				_error = "Unknown coordinate calculation type! (bug?)";
				return 0;
			}
			*/
			switch (kind) {
			case j19kind::EntireScreen: return 0;
			case j19kind::WorkScreen:
				return 0;
			default:
				ggc(_x, tx, W)
			}
		}
		void j19gadget::W(int value) { W(value, tw); }
		void j19gadget::W(int value, j19ctype t) { sgc(_w, tw); }
		int j19gadget::W(bool pure) {
			switch (kind) {
			case j19kind::EntireScreen:
			case j19kind::WorkScreen:
				return ScreenWidth();
			default:
				ggc(_w, tw, W);
			}
		}
		void j19gadget::Y(int value) { Y(value, ty); }
		void j19gadget::Y(int value, j19ctype t) { sgc(_y, ty); }
		int j19gadget::Y(bool pure) {
			switch (kind) {
			case j19kind::EntireScreen:
				return 0;
			case j19kind::WorkScreen:
			{
				auto h{ 0 }; // Pulldown menus and status bar can play a role here!
				auto fh{ FontHeight() };
				if (haspulldown) h += fh;
				//cout << "fuck: " << h << endl;
				return h;
			}
			default:
				ggc(_y, ty, H);
			}
		}
		void j19gadget::H(int value) { H(value, th); }
		void j19gadget::H(int value, j19ctype t) { sgc(_h, th); }
		int j19gadget::H(bool pure) {
			switch (kind) {
			case j19kind::EntireScreen:
				return ScreenHeight();
			case j19kind::WorkScreen:
			{
				auto h{ ScreenHeight() }; // Pulldown menus and status bar can play a role here!
				auto fh{ FontHeight() };
				if (haspulldown) h -= fh;
				if (hasstatus) h -= fh;
				return h;
			}
			default:
				ggc(_h, th, H);
			}
		}

		int j19gadget::DrawX() {
			switch (kind) {
			case j19kind::EntireScreen:
			case j19kind::WorkScreen:
				return 0;
			default:
				if (X() < 0)
					return (parent->DrawX() + parent->W()) + X();
				else
					return X() + parent->DrawX();
			}

		}

		int j19gadget::DrawY() {
			switch (kind) {
			case j19kind::EntireScreen:
				return 0;
			case j19kind::WorkScreen:
				_error = "Workscreen not fully supported yet!";
				if (haspulldown) return FontHeight();
				return 0; // Pulldown menus and status bar can play a role here!
			default:
				if (Y() < 0)
					return (parent->DrawY() + parent->H()) + Y();
				else
					return Y() + parent->DrawY();
			}
		}

		bool j19gadget::RecEnabled() {
			auto work{ this };
			do {
				if (!work->Enabled) return false;
				work = work->parent;
			} while (work);
			return true;
		}

		TImageFont j19gadget::Font() {
			if (!fontloaded) {
				if (defaultfontloaded) return _DefaultFont;
				return nullptr;
			}
			return _Font; 
		}

		void j19gadget::KillFont() {
			// cout << "Killing font for gadget " << this << " f:" << &_Font << "\t" << fontloaded << endl;
			//if (fontloaded) _Font.Kill();
			_Font = nullptr;
			fontloaded = false;
		}

		void j19gadget::SetFont(std::string FFile) {
			auto J = JCR6::JCR6_Dir(FFile);
			SetFont(J, "");
		}

		void j19gadget::SetFont(std::string MFile, std::string FFile) {
			auto J = JCR6::JCR6_Dir(MFile);
			SetFont(J, FFile);
		}

		void j19gadget::SetFont(JCR6::JT_Dir MFile, std::string FFile) {
			KillFont();
			_Font = LoadImageFont(MFile, FFile);
			fontloaded = true;
		}

		void j19gadget::SetFont(TImageFont fnt) {
			KillFont();
			_Font = fnt;
			fontloaded = true;
		}


		void j19gadget::SetDefaultFont(std::string FFile) {
			auto J = JCR6::JCR6_Dir(FFile);
			SetDefaultFont(J, "");
		}

		void j19gadget::SetDefaultFont(std::string MFile, std::string FFile) {
			auto J = JCR6::JCR6_Dir(FFile);
			SetDefaultFont(J, FFile);
		}

		void j19gadget::SetDefaultFont(JCR6::JT_Dir MFile, std::string FFile) {
			KillDefaultFont();
			_DefaultFont = LoadImageFont(MFile, FFile); //_DefaultFont.LoadFont(*MFile, FFile);
			defaultfontloaded = true;
		}

		void j19gadget::SetDefaultFont(TImageFont f) {
			KillDefaultFont();
			_DefaultFont = f;
			defaultfontloaded = true;
		}

		void j19gadget::KillDefaultFont() {
			//if (defaultfontloaded) _DefaultFont.Kill();
			_DefaultFont = nullptr;
			defaultfontloaded = false;
		}

		TImageFont j19gadget::GetDefaultFont() {
			return _DefaultFont;
		}


		int j19gadget::FontHeight() {
			if (!Font()) return 0;
			return Font()->Height("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG 1234567890 the quick brown fox jumps over the lazy dog");
		}

		void j19gadget::StatusText(std::string nt) {
			hasstatus = true;
			_StatusText = nt;
		}

		std::string j19gadget::StatusText() { return _StatusText; }

		void j19gadget::KillImage(bool force) {
			if (force || AutoDelImage) {
				if (_Image) {
					//delete _Image;
					_Image = nullptr;
					AutoDelImage = false;
					_ImageFrame = 0;
				}
			}
		}

		void j19gadget::Image(TImage img, bool autodel) {
			KillImage();
			_ImageFrame = 0;
			_Image = img;
			AutoDelImage = autodel;
		}

		void j19gadget::Image(std::string img) {
			/* Old! May be removed later!
			auto i{ new TQSG_Image() };
			i->Create(img);
			if (TQSG_GetError() != "") {
				cout << "\7ERROR:" << TQSG_GetError() << "\n";
				delete i;
				return;
			}
			*/
			auto i{ LoadImage(img) };
			Image(i, true);
		}

		void j19gadget::Image(JCR6::JT_Dir mainfile, std::string img) {
			/*
			auto i{ new TQSG_Image() };
			i->Create(mainfile, img);
			if (TQSG_GetError() != "") cout << "\7ERROR: " << TQSG_GetError() << "endl";
			*/
			auto i{ LoadImage(mainfile,img) };
			Image(i, true);
		}

		void j19gadget::Image(std::string mainfile, std::string img) {
			//auto i{ new TQSG_Image() };
			//i->Create(mainfile, img);
			auto i{ LoadImage(mainfile,img) };
			Image(i, true);
		}

		TImage j19gadget::Image() {
			return _Image;
		}

		void j19gadget::ImageFrame(int Frame) {
			if (_Image) {
				_ImageFrame = abs(Frame) % _Image->Frames();
			}
		}

		int j19gadget::ImageFrame() {
			return _ImageFrame;
		}

		void j19gadget::IncImageFrame(int modify) {
			ImageFrame(ImageFrame() + modify);
		}


		void j19gadget::SetForeground(j19byte R, j19byte G, j19byte B, j19byte Alpha) {
			FR = R;
			FG = G;
			FB = B;
			FA = Alpha;
		}

		void j19gadget::SetBackground(j19byte R, j19byte G, j19byte B, j19byte Alpha) {
			BR = R;
			BG = G;
			BB = B;
			BA = Alpha;
		}

		void j19gadget::SetForegroundHSV(double hue, double sat, double val) {
			auto _rgb = hsv2rgb({ hue,sat,val });
			FR = (byte)floor(_rgb.r * 255);
			FG = (byte)floor(_rgb.g * 255);
			FB = (byte)floor(_rgb.b * 255);
		}
		void j19gadget::SetBackgroundHSV(double hue, double sat, double val) {
			auto _rgb = hsv2rgb({ hue,sat,val });
			BR = (byte)floor(_rgb.r * 255);
			BG = (byte)floor(_rgb.g * 255);
			BB = (byte)floor(_rgb.b * 255);
		}

		j19gadget* j19gadget::ActiveGadget() {
			return active;
		}

		void j19gadget::Draw(bool force) {			
			_error = "";
			j19chat("Drawing: " << (int)kind << " at (" << X() << "." << DrawX() << " , " << Y() << "." << DrawY() << ") siz: " << W() << "x" << H() << "\tVis:" << Visible << "\tKids:" << kids.size());
			if (force || Visible) {
				if (!HowToDraw.count(kind)) {
					_error = "Kind of type " + to_string((int)kind) + " has no draw routine";
					j19chat(_error);
					return;
				}
				HowToDraw[kind](this);
				if (CBDraw) CBDraw(this, j19action::Draw);
				for (auto kid : kids) {
					if (!kid) 
						std::cout << "Error! Kid does not exist!\n";
					else
						kid->Draw(force);
				}
			}
		}

		void j19gadget::KillKids() {
			auto copykids{ kids };
			for (auto kid : copykids) {
				if (kid != &_WorkScreen) FreeGadget(kid);
			}
			kids.clear();
		}

		void j19gadget::DetachParent() {
			parent->RemoveKid(this);
		}

		void j19gadget::RemoveKid(j19gadget* kid) {
			_error = "";
			int f{ -1 };
			do {
				if (f >= kids.size()) { _error = "Kid to remove not found!"; return; }
			} while (kids[f++] != kid);
			kids.erase(kids.begin() + f);
		}

		void j19gadget::AddItem(std::string ItemText) {
			auto item{ new j19gadgetitem(this,ItemText) };
			Items.push_back(item);
		}
		void j19gadget::AddUniqueItem(std::string ItemText) {
			for (auto fitem : Items) {
				if (fitem->Caption == ItemText) return;
			}
			AddItem(ItemText);
		}

		void j19gadget::ClearItems() {
			for (auto i : Items) {
				i->IconKill();
				i->KillKid();
			}
			Items.clear();
			_SelectedItem = -1;
		}

		size_t j19gadget::NumItems() { return Items.size(); }

		long long j19gadget::SelectedItem() {
			if (_SelectedItem >= Items.size()) _SelectedItem = -1;
			return _SelectedItem;
		}

		void j19gadget::SelectItem(long long idx) {
			_SelectedItem = idx;
			if (_SelectedItem >= Items.size()) _SelectedItem = -1;
		}

		void j19gadget::SelectItem(String itemText) {
			_SelectedItem = -1;
			for (int i = 0;i<Items.size();++i) {
				auto gi{ Items[i] };
				if (gi->Caption == itemText) { _SelectedItem = i; return; }
			}
		}

		void j19gadget::ItemText(long long idx, std::string NewText) {
			if (idx < 0) { _error = "Illegal index"; return; }
			if (idx >= NumItems()) { _error = "Item index out of range"; return; }
			Items[idx]->Caption = NewText;
		}

		void j19gadget::ItemText(std::string NewText) { ItemText(SelectedItem(), NewText); }

		std::string j19gadget::ItemText(long long idx) {
			if (idx < 0) { _error = "Illegal index"; return ""; }
			if (idx >= NumItems()) { _error = "Item index out of range"; return ""; }
			return Items[idx]->Caption;
		}

		std::string j19gadget::ItemText() { return ItemText(SelectedItem()); }

		j19gadgetitem* j19gadget::__ITEM(long long idx) {
			if (idx < 0) { _error = "Illegal index"; return nullptr; }
			if (idx >= NumItems()) { _error = "Item index out of range"; return nullptr; }
			return Items[idx];
		}

		j19pulldown* j19gadget::AddMenu(std::string Caption) {
			haspulldown = true;
			auto MSI = std::make_shared<j19pulldown>(Caption);
			_MenuBar.push_back(MSI);
			auto NI{ _MenuBar[_MenuBar.size() - 1].get() };
#ifdef DEBUG_PULLDOWNKEY
			cout << "Created Menu Item: " << NI->Caption << " #" << (unsigned long long)NI << endl;
#endif
			return NI;
		}

		j19gadget* Screen() {
			static bool initiated{ false };
			if (!initiated) {
				initiated = true;
				WorkScreen();
				_Screen.SetKind(j19kind::EntireScreen);
			}
			return &_Screen;
		}

		j19gadget* WorkScreen() {
			static bool initiated{ false };
			if (!initiated) {
				initiated = true;
				_WorkScreen.SetParent(Screen());
				_WorkScreen.SetKind(j19kind::WorkScreen);
			}
			return &_WorkScreen;
		}

		void FreeGadget(j19gadget* gadget) {
			_error = "";
			if (gadget == &_Screen || gadget == &_WorkScreen) {
				_error = "Screen and WorkScreen cannot be disposed!";
				return;
			}
			gadget->KillKids();
			gadget->DetachParent();
			//gadget->KillFont();
			gadget->KillImage();
			gadget->ClearItems();
			if (gadget->Active()) j19gadget::DeActivate();
			delete gadget;
		}

		std::string GetCoreError() { return _error; }

		j19gadget* CreateGroup(int x, int y, int w, int h, j19gadget* Parent, j19ctype coordtype) {
			static bool init{ j19gadget::RegDraw(j19kind::Group,DrawScreen) };
			auto ret{ new j19gadget() };
			ret->SetParent(Parent);
			ret->SetKind(j19kind::Group);
			ret->X(x, coordtype);
			ret->Y(y, coordtype);
			ret->W(w, coordtype);
			ret->H(h, coordtype);
			return ret;
		}

		void FreeJune19() {
			Screen()->KillKids();
			WorkScreen()->KillKids();
		}

		static bool DS_Success{ j19gadget::RegDraw(j19kind::EntireScreen,DrawScreen) };
		static bool DWS_Succes{ j19gadget::RegDraw(j19kind::WorkScreen,DrawWorkScreen) };

		void j19gadgetitem::IconKill() {
			if (!_Icon) return;
			//if (autodelicon) delete _Icon;
			_Icon = nullptr;
		}

		j19gadgetitem::j19gadgetitem(j19gadget* Mama, std::string Capt) {
			Caption = Capt;
			Parent = Mama;
		}

		TImage j19gadgetitem::Icon() { return _Icon; }

		size_t j19gadget::NumKids() {
			return kids.size();
		}

		j19gadget* j19gadget::Kid(size_t idx) {
			if (idx >= NumKids()) return nullptr;
			return kids[idx];
		}

		void j19gadgetitem::Icon(TImage Ico, bool delwhenreleased) {
			IconKill();
			_Icon = Ico;
			autodelicon = delwhenreleased;
		}

		void j19gadgetitem::Icon(std::string Ico) {
			//auto I{ new TQSG_Image() };
			//I->Create(Ico);
			auto I{ LoadImage(Ico) };
			Icon(I, true);
		}

		void j19gadgetitem::Icon(JCR6::JT_Dir J, std::string Ico) {
			//auto I{ new TQSG_Image() };
			//I->Create(*J, Ico);
			auto I{ LoadImage(J,Ico) };
			Icon(I, true);
		}

		void j19gadgetitem::Icon(std::string JCRFile, std::string Ico) {
			//auto I{ new TQSG_Image() };
			//I->Create(JCRFile, Ico);
			auto I{ LoadImage(JCRFile,Ico) };
			Icon(I, true);
		}

		j19gadget* j19gadgetitem::Kid() {
			if (Parent->GetKind() != j19kind::Tabber) return nullptr;
			return kid;
		}

		j19gadget::~j19gadget(){
			if (CBDispose) CBDispose(this, j19action::Dispose);
			if (HowToDispose.count(kind)) HowToDispose[kind](this);
		}

		void j19gadgetitem::CreateKid() {
			KillKid();
			kid = CreateGroup(2, 2, 10, 10, Parent);
			// Please note, the width and height will always be automatically re-adjugest by the parent, so 10x10 is only a base value so i can start somewhere
		}

		void j19gadgetitem::KillKid() {
			if (kid) {
				//FreeGadget(kid);
				//delete kid; kid = nullptr; 
				kid = nullptr;
			}
		}
		j19pulldown* j19pulldown::Active{ nullptr };
		j19pulldown* j19pulldown::AddMenu(std::string Caption) {
			if (this->_type != 1) {
				return nullptr; // You can't add items to non-submenus.
			}
			auto ret{ std::make_shared<j19pulldown>(Caption) };
			ret->_parent = this;
			_kids.push_back(ret);
#ifdef DEBUG_PULLDOWNKEY
			cout << "Added pd item: " << ret->Caption << " in parent: " << this->Caption << endl;
#endif
			return ret.get();
		}
		j19pulldown* j19pulldown::AddItem(std::string Caption, j19callbackfunc CallBack, SDL_KeyCode QuickKey) {
			auto ret{ this->AddMenu(Caption) };
			if (!ret) return nullptr; // Crash prevention
			ret->_CallBack = CallBack;
			ret->_type = 0;
			ret->_quickkey = QuickKey;
			return ret;
		}
		j19pulldown* j19pulldown::AddStrike() {
			auto ret{ AddMenu("-----") };
			if (!ret) return nullptr; // Crash prevention
			ret->_type = 2;
			return ret;
		}

		int j19pulldown::Type() {
			return _type;
		}

		void j19pulldown::TrueExeKey(SDL_KeyCode C) {
#ifdef DEBUG_PULLDOWNKEY
			cout << "Check Menu: " << Caption << "; Key: " << (int)C << ":" << (int)_quickkey << endl;
#endif
			switch (Type()) {
			case 0:
				// Normal
				if (_quickkey == C)
					Call();
				break;
			case 1:
				for (auto kid : _kids) kid->TrueExeKey(C);
				break;
			}
		}

		void j19pulldown::Call() { if (_CallBack) _CallBack(WorkScreen(), j19action::PDMenuAction); }

		void j19pulldown::ExeKey(SDL_KeyCode C) {
			for (auto men : _MenuBar) men->TrueExeKey(C);
		}

		j19pulldown::j19pulldown(std::string aCaption) {
			_type = 1;
			Caption = aCaption;
		}
	}
}
