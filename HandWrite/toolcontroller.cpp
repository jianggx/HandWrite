/*
   Drawpile - a collaborative drawing program.

   Copyright (C) 2015-2018 Calle Laakkonen

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
#include "toolcontroller.h"
#include "GdiPainter.h"
#include"pressure.h"

#include "PPoint.h"



ToolController::ToolController()
	:
	m_toolbox{},
	m_activeTool(nullptr),
	m_painter(nullptr),
	m_smoothing(10),
	m_drawBegined(false)
{
}

ToolController::ToolController(Painter* painter)
	:
	m_toolbox{},
	m_activeTool(nullptr),
	m_painter(painter),
	m_drawBegined(false)
{

	registerTool(new Freehand(*this)); // eraser is a specialized freehand tool

	m_activeTool = m_toolbox[Tool::FREEHAND];
	m_pressureMaping.mode = PressureMapping::Mode::VELOCITY;
	m_pressureMaping.curve.fromString("0,1;1,0");

	m_smoothing = 5;
	m_smoother.setSmoothing(m_smoothing);
	m_pressureMaping.param = 80;
	m_painter->setPenWidth(5);

}

void ToolController::registerTool(Tool *tool)
{
	assert(tool->type() >= 0 && tool->type() < Tool::_LASTTOOL);
	assert(m_toolbox[int(tool->type())] == nullptr);
	m_toolbox[tool->type()] = tool;
}

ToolController::~ToolController()
{
	for (Tool *t : m_toolbox)
		delete t;
}

Tool *ToolController::activeTool()const
{
	return m_activeTool;
}

void ToolController::setActiveTool(Tool::Type tool)
{
	m_activeTool = getTool(tool);
}

Tool *ToolController::getTool(Tool::Type type)
{
	assert(type >= 0 && type < Tool::_LASTTOOL);
	assert(m_toolbox[int(type)] == nullptr);
	return m_toolbox[type];

}


void ToolController::setSmoothing(int smoothing)
{
	if (m_smoothing != smoothing) {
		m_smoothing = smoothing;
		if (smoothing > 0)
			m_smoother.setSmoothing(smoothing);
	}
}

void ToolController::startDrawing(const Point &point, double pressure)
{
	assert(m_activeTool);

	m_lastPoint = point;
	m_drawBegined = false;

}

void ToolController::continueDrawing(const Point &point, double pressure)
{
	assert(m_activeTool);
	PPoint ppt(point, pressure);
	double velocity = point.distance(m_lastPoint);
	if (!m_drawBegined) {
		PPoint ppt(m_lastPoint, m_pressureMaping.mapPressure(velocity));
		if (m_smoothing > 0 && m_activeTool->allowSmoothing()) {
			m_smoother.reset();
			m_smoother.addPoint(ppt);
		}
		// TODO handle hasSmoothPoint() == false
		m_activeTool->begin(ppt);
		m_drawBegined = true;
	}

	ppt.setPressure(m_pressureMaping.mapPressure(velocity));
	m_lastPoint = point;

	if (m_smoothing > 0 && m_activeTool->allowSmoothing()) {
		m_smoother.addPoint(ppt);

		if (m_smoother.hasSmoothPoint()) {

			m_activeTool->motion(m_smoother.smoothPoint());
		}

	}
	else {
		m_activeTool->motion(ppt);
	}
}


void ToolController::endDrawing()
{
	assert(m_activeTool);
	if (!m_drawBegined) //只绘制单点
	{
		PPoint ppt(m_lastPoint,0.5);
		if (m_smoothing > 0 && m_activeTool->allowSmoothing()) {
			m_smoother.reset();
			m_smoother.addPoint(ppt);
		}
		// TODO handle hasSmoothPoint() == false
		m_activeTool->begin(ppt);
	}
	// Drain any remaining points from the smoothing buffer
	if (m_smoothing > 0 && m_activeTool->allowSmoothing()) {
		if (m_smoother.hasSmoothPoint())
			m_smoother.removePoint();

		// 尾部处理
		m_smoother.setLastPointPressure(0);
		while (m_smoother.hasSmoothPoint()) {
			m_activeTool->motion(m_smoother.smoothPoint());
			m_smoother.removePoint();
		}
	}
	m_activeTool->end();
}
