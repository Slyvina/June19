// Lic:
// head/june19_picture.hpp
// June 19 - Picture (header)
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
#include "june19_core.hpp"

namespace Slyvina {
	namespace June19 {

		const int Pic_TopLeft = 0;
		const int Pic_TopRight = 1;
		const int Pic_TopCenter = 2;
		const int Pic_CenterLeft = 3;
		const int Pic_Center = 4;
		const int Pic_Center_Right = 5;
		const int Pic_BottomLeft = 6;
		const int Pic_BottomRight = 7;
		const int Pic_BottomCenter = 8;
		const int Pic_Tiled = 9;
		const int Pic_FullStretch = 10;
		const int Pic_Fit = 11;

		j19gadget* CreatePicture(int x, int y, int w, int h, j19gadget* Opoe, int flag = Pic_TopLeft);


	}
}