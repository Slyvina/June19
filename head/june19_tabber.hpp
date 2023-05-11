// Lic:
// head/june19_tabber.hpp
// June 19 - Tabber (header)
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

		j19gadget* CreateTabber(int x, int y, int w, int h, j19gadget* group);

		// Get the data of the tab of your choice;
		j19gadget* GetTab(j19gadget* g, size_t idx);

		// Get the data of the currently active tab
		j19gadget* GetTab(j19gadget* g);

		// Create a tab and get the panel attached to it. (Please note initially the tab will have the same sizes as its host tabber, however this can automatically be adapted in order to keep fitting when more tabs are added (or tabs are being removed)
		j19gadget* AddTab(j19gadget* parent, std::string Caption);

		// Get the latest error message concerning tabs
		std::string GetTabError();

	}
}