// License:
// 	head/june19_core.hpp
// 	June 19 - Core (header)
// 	version: 24.11.27
// 
// 	Copyright (C) 2020, 2021, 2023, 2024 Jeroen P. Broks
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
// head/june19_core.hpp
// June 19 - Core (header)
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
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>


#include <TQSG.hpp>
#include <TQSE.hpp>
#define j19byte unsigned char

#define j19nullgadget(gadget) {FreeGadget(gadget); gadget=nullptr;}

#undef j19loudmouth

#ifdef j19loudmouth
#define j19chat(A) std::cout << "LOUDMOUTH:> " << A << std::endl
#else
#define j19chat(A)
#endif


#define j19callback(g,a) if (g->CBAction) g->CBAction(g,j19action::a)
namespace Slyvina {
	namespace June19 {



		enum class j19kind {
			// System Core
			Unknown,
			EntireScreen,
			WorkScreen,
			Group,
			// Otherstuff
			Label,
			Panel,
			//TextField, // Dupe!
			RadioButton,
			CheckBox,
			Picture,
			ListBox,
			Tabber,
			Textfield,
			Button,
			VertScrollBar,
			HoriScrollBar			
		};

		enum class j19ctype { Absolute, Percent };
		enum class j19action { Unknown, Click, Select, DoubleClick, Activate, Draw, Check, UnCheck, Type, Enter, BackSpace, PDMenuAction, Dispose };


		class j19gadget;
		class j19gadgetitem;
		class j19pulldown;


		typedef void (*j19draw)(j19gadget* g);

		// Used for call back actions
		typedef void (*j19callbackfunc)(j19gadget* source, j19action action);

		typedef void(*j19scroll)(j19gadget* s, int scroll);

		class j19gadgetitem {
			j19gadget* Parent;
			TQSG::TImage _Icon{ nullptr };
			bool autodelicon{ false };
			j19gadget* kid{ nullptr }; // Only used by tabbers
		public:
			j19gadgetitem(j19gadget* Mama, std::string Capt = "");
			std::string Caption;
			TQSG::TImage Icon();


			void IconKill();
			void Icon(TQSG::TImage Ico, bool delwhenreleased = true);
			void Icon(std::string Ico);
			void Icon(JCR6::JT_Dir J, std::string Ico);
			void Icon(std::string JCRFile, std::string Ico);

			j19gadget* Kid(); // Will be coded when the tabbers come
			void CreateKid();
			void KillKid();
		};

		class j19gadget {
		private:
			static size_t __count;
			size_t __ID{ __count++ };
			j19gadget* parent{ nullptr };
			std::vector<j19gadget*> kids;
			j19kind kind{ j19kind::Unknown };
			int _x{ 0 }, _y{ 0 }, _w{ 100 }, _h{ 100 };
			j19ctype tx{ j19ctype::Absolute }, ty{ j19ctype::Absolute }, tw{ j19ctype::Absolute }, th{ j19ctype::Absolute };
			static std::map<j19kind, j19draw> HowToDraw;
			static std::map<j19kind, j19draw> HowToDispose;
			bool fontloaded{ false };
			TQSG::TImageFont _Font;
			static bool haspulldown;
			static bool hasstatus;
			static j19gadget* active;
			static std::string _StatusText;

			static bool defaultfontloaded;
			static TQSG::TImageFont _DefaultFont;

			std::vector<j19gadgetitem*> Items;

			TQSG::TImage _Image{ nullptr };
			bool AutoDelImage{ false };
			int _ImageFrame{ 0 };

			long long _SelectedItem{ -1 };

		public:
			inline size_t ID() { return __ID; }

			/// <summary>
			/// This field is normally ingored, but can used to store extra data.
			/// </summary>
			std::string HData{ "" };

			/// <summary>
			/// Some gadgets require special kind of data of any kind. This void pointer is meant for adressing that. Don't touch it unless you know what you are doing!
			/// </summary>
			void* bData{ nullptr };

			/// <summary>
			/// How many kids does this gadget have?
			/// </summary>
			/// <returns>The total number of kids</returns>
			size_t NumKids();

			/// <summary>
			/// Retrieves kid
			/// </summary>
			/// <param name="idx">Pointer to kid</param>
			/// <returns></returns>
			j19gadget* Kid(size_t idx);
			j19scroll SetScrollY;

			bool Active();
			void Activate();
			static void DeActivate();
			j19kind GetKind();
			j19gadget* GetParent();
			bool AutoResize{ false }; // Only works on TextFields and buttons
			bool Down{ false }; // Used for button mouse checks. That always works a bit odd on buttons, eh?
			static bool ScreenHasPullDown();
			static j19pulldown* ScreenPullDown(int i);
			static bool ScreenHasStatus();

			// Will contain true if an item was selected the last time a gadget was drawn (never assign any data to this variable yourself).
			bool EventSelectItem{ false };

			int IntFlag{ 0 };
			int ScrollX{ 0 }, ScrollY{ 0 };
			static bool RegDraw(j19kind k, j19draw v);
			static bool RegDispose(j19kind k, j19draw v);

			void SetKind(j19kind value); // Please note this only works once, and most creation fuctions take care of this automatically.
			void SetParent(j19gadget* value); // Please note this only works once, and most creation fuctions take care of this automatically.

			void X(int value);
			void X(int value, j19ctype t);
			int X(bool pure = false);

			void W(int value);
			void W(int value, j19ctype t);
			int W(bool pure = false);

			void Y(int value);
			void Y(int value, j19ctype t);
			int Y(bool pure = false);

			void H(int value);
			void H(int value, j19ctype t);
			int H(bool pure = false);

			int DrawX();
			int DrawY();

			bool Visible{ true };
			bool Enabled{ true };

			// Scans back if it's fully enabled, checking all parents too
			bool RecEnabled();

			TQSG::TImageFont Font();
			void KillFont();
			void SetFont(std::string FFile);
			void SetFont(std::string MFile, std::string FFile);
			void SetFont(JCR6::JT_Dir MFile, std::string FFile);
			void SetFont(TQSG::TImageFont fnt);

			static void SetDefaultFont(std::string FFile);
			static void SetDefaultFont(std::string MFile, std::string FFile);
			static void SetDefaultFont(JCR6::JT_Dir MFile, std::string FFile);
			static void SetDefaultFont(TQSG::TImageFont F);
			static void KillDefaultFont();
			static TQSG::TImageFont GetDefaultFont();

			int FontHeight();

			static void StatusText(std::string nt);
			static std::string StatusText();

			std::string Caption{ "" };
			std::string Text{ "" };

			j19byte FR{ 255 }, FG{ 255 }, FB{ 255 }, FA{ 255 };
			j19byte BR{ 0 }, BG{ 0 }, BB{ 0 }, BA{ 0 };

			bool checked{ false };



			// Kills an image, unless it was assinged with a direct pointer assignment (this check is omitted by giving 'true' to the force parameter, but I only recommend to do so if you know what you are doing).
			void KillImage(bool force = false);

			// Assigns image to a gadget (if the gadget doesn't support images this action will be ignored). If 'autodel' is set to true, the image will be removed from the memory when calling KillImage(), FreeGadget() or when assiging another image to it.
			void Image(TQSG::TImage img, bool autodel = false);

			// Assigns image to a gadget from a file.
			void Image(std::string img);

			// Assigns image to a gadget from a JCR resource
			void Image(JCR6::JT_Dir mainfile, std::string img);
			// Assigns image to a gadget from a JCR resource
			void Image(std::string mainfile, std::string img);

			// Obtains the image pointer inside this gadget (retuns nullptr if no such image exists)
			TQSG::TImage Image();

			// Sets Image Frame value (mind the number of available frames as going for more can have funny results)
			void ImageFrame(int Frame);
			// Gets image Frame value
			int ImageFrame();
			// Increase image frame
			void IncImageFrame(int modify);

			// sets the Foreground color of the gadget
			void SetForeground(j19byte R, j19byte G, j19byte B, j19byte Alpha = 255);

			// Sets the background color of the gadget
			void SetBackground(j19byte R, j19byte G, j19byte B, j19byte Alpha = 255);

			void SetForegroundHSV(double hue, double sat, double val);
			void SetBackgroundHSV(double hue, double sat, double val);

			// Draw a gadget and all its children (if visible)
			void Draw(bool force = false);

			// These methods should NEVER be called directly! FreeGadget() needs them
			void KillKids(); // Called by "FreeGadget". Don't call this directly unless you know what you are doing
			void DetachParent(); // Remove Gadget from parent's kids. NEVER call this yourself unless you know what you doing! FreeGadget needs this method, that's all!
			void RemoveKid(j19gadget* kid); // Remove kid from parent. NEVER call this yuourself, unless youknow what you are doing! FreeGadget needs this methid, and that's all!

			// Adds an item to a gadget, please note that if you do this with a gadget that is not item based nothing visual will happen, and you will only waste RAM.
			void AddItem(std::string ItemText);

			// Removes all items from a gadget
			void ClearItems();

			// Returns the number of items in a gadget
			size_t NumItems();

			// Returns the index number of the selected, or -1 when none is selected
			long long SelectedItem();

			void SelectItem(long long idx);

			// Returns item text of an item
			void ItemText(long long idx, std::string NewText);
			void ItemText(std::string NewText);
			std::string ItemText(long long idx);
			std::string ItemText();


			j19gadgetitem* __ITEM(long long idx); // NOT TO BE CALLED DIRECTLY! Some gadgets need this

			// If defined this will be called when an action is generated by the gadget. Not all gadgets respond to this though.
			j19callbackfunc CBAction{ nullptr };

			// If defined this will be called every time a Draw action is requested from this gadget
			j19callbackfunc CBDraw{ nullptr };

			// If define this will be called when this gadget is being disposed
			j19callbackfunc CBDispose{ nullptr };

			j19pulldown* AddMenu(std::string Caption);

			static j19gadget* ActiveGadget();

			~j19gadget();
		};

		class j19pulldown {
		private:
			unsigned char _type{ 0 }; // 0 = normal, 1 = submenu, 2 = strike
			std::vector<std::shared_ptr<j19pulldown>> _kids{};
			j19callbackfunc _CallBack{ nullptr };
			j19pulldown* _parent{ nullptr };
			SDL_KeyCode _quickkey{ SDLK_UNKNOWN };
			void Call();
			void TrueExeKey(SDL_KeyCode C);
		public:
			static j19pulldown* Active;

			std::string Caption{ "" };

			j19pulldown* AddMenu(std::string Caption);
			j19pulldown* AddItem(std::string Caption, j19callbackfunc CallBack, SDL_KeyCode QuickKey = SDLK_UNKNOWN);
			j19pulldown* AddStrike();
			int Type();

			static void ExeKey(SDL_KeyCode C);

			// NEVER create a pulldown directly!
			j19pulldown(std::string Caption);
		};

		j19gadget* Screen();
		j19gadget* WorkScreen();
		void FreeGadget(j19gadget* gadget); // Disposes a gadget and all its children. 

		std::string GetCoreError();

		j19gadget* CreateGroup(int x, int y, int w, int h, j19gadget* Parent, j19ctype coordtype = j19ctype::Absolute);

		void FreeJune19(); // Frees all gadgets tried to Screen or WorkScreen. Recommended to do before closing down!

	}
}
