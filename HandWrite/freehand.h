/*
   Drawpile - a collaborative drawing program.

   Copyright (C) 2006-2017 Calle Laakkonen

   Drawpile is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Drawpile is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Drawpile.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TOOLS_BRUSHES_H
#define TOOLS_BRUSHES_H

#include "tool.h"
#include"strokesmoother.h"
#include"pressure.h"

class ToolController;

//! Freehand brush tool
class Freehand : public Tool
{
public:
	Freehand(ToolController &toolControl);

	void begin(const PPoint& point) override;
	void motion(const PPoint& point) override;
	void end() override;

	bool allowSmoothing() const override { return true; }

private:
   void pen_move(const PPoint& point) const;

};


#endif

