// Lic:
// src/june19_picture.cpp
// June 19 - Picture
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
#include <iostream>
#include "june19_picture.hpp"

using namespace TrickyUnits;

namespace june19 {

	static std::string _error;

    void PicDraw(j19gadget* g){
		auto img{ g->Image() };
		//std::cout << "PICDRAW: " << (int)img<< '\n';
		if (!img) return;
		//std::cout << "Go on!";
		if (g->BA) {
			TQSG_ACol(g->BR, g->BG, g->BB, g->BA);
			TQSG_Rect(g->DrawX(), g->DrawY(), g->W(), g->H());
		}
		TQSG_ACol(g->FR, g->FG, g->FB, g->FA);
		//std::cout << (int)g->FR << "/" << (int)g->FG << "/" << (int)g->FB << "/" << (int)g->FA << std::endl;
		int cx{ g->DrawX() + (g->W() / 2) };
		int cy{ g->DrawY() + (g->H() / 2) };
		int icx{ cx - (img->Width() / 2) };
		int icy{ cy - (img->Height() / 2) };
		int ecx{ g->DrawX() + g->W() - img->Width() };
		int ecy{ g->DrawY() + g->H() - img->Height() };
		//std::cout << g->IntFlag << std::endl;
		switch (g->IntFlag) {			
		case Pic_TopLeft:
			std::cout << "Topleft: (" << g->DrawX() << "," << g->DrawY() << ") Frame: " << g->ImageFrame() << "\n Img: "<<img->Width()<<"x"<<img->Height()<<"\t"<<std::endl;
			img->Draw(g->DrawX(), g->DrawY(), g->ImageFrame());
			break;
		case Pic_TopCenter:
			img->Draw(icx, g->DrawY(), g->ImageFrame());
			break;
		case Pic_TopRight:
			img->Draw(ecx, g->DrawY(), g->ImageFrame());
			break;
		case Pic_CenterLeft:
			img->Draw(g->DrawX(), icy, g->ImageFrame());
			break;
		case Pic_Center:
			img->Draw(icx, icy, g->ImageFrame());
			break;
		case Pic_Center_Right:
			img->Draw(ecx, icy, g->ImageFrame());
			break;
		case Pic_BottomLeft:
			img->Draw(g->DrawX(), ecy, g->ImageFrame());
			break;
		case Pic_BottomCenter:
			img->Draw(icx, ecy, g->ImageFrame());
			break;
		case Pic_BottomRight:
			img->Draw(ecx, ecy, g->ImageFrame());
			break;
		case Pic_FullStretch:
			img->StretchDraw(g->DrawX(), g->DrawY(), g->W(), g->H(), g->ImageFrame());
			break;
		case Pic_Tiled:
			img->Tile(g->DrawX(), g->DrawY(), g->W(), g->H(), g->ImageFrame());
			break;
		case Pic_Fit: {
			double ratw{ (double)img->Width() / g->W() };
			double rath{ (double)img->Height() / g->H() };
			if (ratw == 1 && rath == 1) {
				img->Draw(g->DrawX(), g->DrawY(), g->ImageFrame());
			} else if (ratw > 1 && rath <= 1) {
				double rat{ (double)g->W() / img->Width() };
				img->StretchDraw(g->DrawX(), g->DrawY(), img->Width() * rat, img->Height() * rat);
			} else if (ratw <= 1 && rath > 1) {
				double rat{ (double)g->H() / img->Height() };
				img->StretchDraw(g->DrawX(), g->DrawY(), img->Width() * rat, img->Height() * rat);
			} else if (ratw < rath && rath <= 1) {
				double rat{ (double)img->Height() / g->H() };
				img->StretchDraw(g->DrawX(), g->DrawY(), img->Width() * rat, img->Height() * rat);
			} else if (rath < ratw && ratw <= 1) {
				double rat{ (double)img->Width() / g->W() };
				img->StretchDraw(g->DrawX(), g->DrawY(), img->Width() * rat, img->Height() * rat);
			} else {
				static bool warned{ false };// Only one warning! If more is wrong, well, if this error comes it's likely stuff is bugged anyway (or inconplete, what you like)
				if (!warned) std::cout << "\7ERROR! Don't know how to fit the image ratios W:" << ratw << "H:" << rath << "   img:" << img->Width() << "x" << img->Height() << "   gadget:" << g->W() << "x" << g->H() << std::endl;
				warned = true;
			}
			break;
		}
		default:
		{
			static std::map<int, bool> warned;
			if (!warned.count(g->IntFlag)) {
				std::cout << "\7ERROR! Draw method (" << g->IntFlag << ") unknown!\n";
				warned[g->IntFlag] = true;
			}
		}
		}
	}

    j19gadget* CreatePicture(int x, int y, int w, int h, j19gadget* Opoe, int flag) {
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::Picture, PicDraw);
		}
		_error = "";
		ret->SetKind(j19kind::Picture);
		ret->X(x);
		ret->Y(y);
		ret->W(w);
		ret->H(h);
		ret->SetParent(Opoe);
		ret->IntFlag = flag;
		return ret;
	}

	j19gadget* CreatePictureR(int x, int y, int w, int h, j19gadget* Opoe, int flag) {
		static auto init{ false };
		auto ret{ new j19gadget() };
		if (!init) {
			j19gadget::RegDraw(j19kind::Picture, PicDraw);
		}
		_error = "";
		ret->SetKind(j19kind::Label);
		ret->X(x, j19ctype::Percent);
		ret->Y(y, j19ctype::Percent);
		ret->W(w, j19ctype::Percent);
		ret->H(h, j19ctype::Percent);
		ret->SetParent(Opoe);
		ret->IntFlag = flag;
		return ret;
	}

}