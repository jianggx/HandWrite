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
#include"stdafx.h"
#include "freehand.h"

#include "toolcontroller.h"

Freehand::Freehand(ToolController &toolcontroller)
: Tool(toolcontroller, FREEHAND)
{

}

void Freehand::pen_move(const PPoint& point) const
{
   std::vector<PPoint> v;
   v.push_back(point);
   toolController().painter().PenMove(v);
}

void Freehand::begin(const PPoint& point)
{
   pen_move(point);
}

void Freehand::motion(const PPoint& point)
{
	pen_move(point);
}

void Freehand::end()
{

	toolController().painter().PenUp();
}


