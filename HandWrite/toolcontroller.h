/*
   Drawpile - a collaborative drawing program.

   Copyright (C) 2015-2016 Calle Laakkonen

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

#ifndef TOOLCONTROLLER_H
#define TOOLCONTROLLER_H

#include "strokesmoother.h"
#include "tool.h"
#include "freehand.h"


class PPoint;
class Point;
class Tool;
class Painter;
class PressureMapping;

/**
 * @brief The ToolController dispatches user input to the currently active tool
 */
class ToolController 
{
public:
   ToolController();
   ToolController(Painter* painter);
   ~ToolController();

	void setActiveTool(Tool::Type tool);
   Tool* activeTool() const;

	//void setActiveLayer(int id);
	//int activeLayer() const { return m_activeLayer; }


	//void setActiveBrush(const Brush &b);
	//const Brush &activeBrush() const { return m_activebrush; }

	//void setModel(canvas::CanvasModel *model);
	//canvas::CanvasModel *model() const { return m_model; }

	void setSmoothing(int smoothing);
	int smoothing() const { return m_smoothing; }

	void registerTool(Tool *);

	Tool *getTool(Tool::Type type);


public:
	//! Start a new stroke
	void startDrawing(const Point &point, double pressure=1);


	//! Continue a stroke
	void continueDrawing(const Point &point, double pressure=1);


	//! End a stroke
	void endDrawing();


	Painter& painter() const { return *m_painter; }


private:

	Tool *m_toolbox[Tool::_LASTTOOL];

	//canvas::CanvasModel *m_model;

	//Brush m_activebrush;
	Tool *m_activeTool;
	//int m_activeLayer;

	int m_smoothing;
	StrokeSmoother m_smoother;
	Painter* m_painter;
   PressureMapping m_pressureMaping;
   Point m_lastPoint;
};

#endif // TOOLCONTROLLER_H
