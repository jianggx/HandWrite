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

#include "toolcontroller.h"

#include "colorpicker.h"
#include "laser.h"
#include "selection.h"
#include "shapetools.h"
#include "beziertool.h"
#include "floodfill.h"

#include "PPoint.h"
#include "core/layerstack.h"
#include "core/annotationmodel.h"
#include "canvas/canvasmodel.h"
#include "canvas/statetracker.h"


ToolController::ToolController()
	: 
	m_toolbox{},
	m_activeTool(nullptr),
	m_smoothing(10)
{

	registerTool(new Freehand(*this)); // eraser is a specialized freehand tool

	m_activeTool = m_toolbox[Tool::FREEHAND];
	m_pai
}

void ToolController::registerTool(Tool *tool)
{
	Q_ASSERT(tool->type() >= 0 && tool->type() < Tool::_LASTTOOL);
	Q_ASSERT(m_toolbox[int(tool->type())] == nullptr);
	m_toolbox[tool->type()] = tool;
}

ToolController::~ToolController()
{
	for(Tool *t : m_toolbox)
		delete t;
}

Tool *ToolController::getTool(Tool::Type type)
{
	Q_ASSERT(type >= 0 && type < Tool::_LASTTOOL);
	Tool *t = m_toolbox[type];
	Q_ASSERT(t);
	return t;
}

void ToolController::setActiveTool(Tool::Type tool)
{
	if(activeTool() != tool) {
		m_activeTool->cancelMultipart();
		m_activeTool = getTool(tool);
		emit activeToolChanged(tool);
		emit toolCursorChanged(activeToolCursor());
	}
}

void ToolController::setActiveAnnotation(int id)
{
	if(m_activeAnnotation != id) {
		m_activeAnnotation = id;
		emit activeAnnotationChanged(id);
	}
}

Tool::Type ToolController::activeTool() const
{
	Q_ASSERT(m_activeTool);
	return m_activeTool->type();
}

QCursor ToolController::activeToolCursor() const
{
	Q_ASSERT(m_activeTool);
	return m_activeTool->cursor();
}

void ToolController::setActiveLayer(int id)
{
	if(m_activeLayer != id) {
		m_activeLayer = id;
		if(m_model)
			m_model->layerStack()->setViewLayer(id);

		emit activeLayerChanged(id);
	}
}

void ToolController::setActiveBrush(const paintcore::Brush &b)
{
	m_activebrush = b;
	emit activeBrushChanged(b);
}

void ToolController::setModel(canvas::CanvasModel *model)
{
	if(m_model != model) {
		m_model = model;

		connect(m_model->stateTracker(), &canvas::StateTracker::myAnnotationCreated, this, &ToolController::setActiveAnnotation);
		connect(m_model->layerStack()->annotations(), &paintcore::AnnotationModel::rowsAboutToBeRemoved, this, &ToolController::onAnnotationRowDelete);

		emit modelChanged(model);
	}
}

void ToolController::onAnnotationRowDelete(const QModelIndex&, int first, int last)
{
	for(int i=first;i<=last;++i) {
		const QModelIndex &a = m_model->layerStack()->annotations()->index(i);
		if(a.data(paintcore::AnnotationModel::IdRole).toInt() == activeAnnotation())
			setActiveAnnotation(0);
	}
}

void ToolController::setSmoothing(int smoothing)
{
	if(m_smoothing != smoothing) {
		m_smoothing = smoothing;
		if(smoothing>0)
			m_smoother.setSmoothing(smoothing);
		emit smoothingChanged(smoothing);
	}
}

void ToolController::startDrawing(const QPointF &point, qreal pressure, bool right, float zoom)
{
	Q_ASSERT(m_activeTool);

	if(!m_model) {
		qWarning("ToolController::startDrawing: no model set!");
		return;
	}

	if(m_smoothing>0 && m_activeTool->allowSmoothing()) {
		m_smoother.reset();
		m_smoother.addPoint(paintcore::Point(point, pressure));
	}
	// TODO handle hasSmoothPoint() == false
	m_activeTool->begin(paintcore::Point(point, pressure), right, zoom);

	if(!m_activeTool->isMultipart())
		m_model->stateTracker()->setLocalDrawingInProgress(true);
}

void ToolController::continueDrawing(const QPointF &point, qreal pressure, bool shift, bool alt)
{
	Q_ASSERT(m_activeTool);

	if(!m_model) {
		qWarning("ToolController::continueDrawing: no model set!");
		return;
	}

	if(m_smoothing>0 && m_activeTool->allowSmoothing()) {
		m_smoother.addPoint(paintcore::Point(point, pressure));

		if(m_smoother.hasSmoothPoint()) {
			m_activeTool->motion(m_smoother.smoothPoint(), shift, alt);
		}

	} else {
		m_activeTool->motion(paintcore::Point(point, pressure), shift, alt);
	}

	m_prevShift = shift;
	m_prevAlt = alt;
}

void ToolController::hoverDrawing(const QPointF &point)
{
	Q_ASSERT(m_activeTool);
	if(!m_model)
		return;

	m_activeTool->hover(point);
}

void ToolController::endDrawing()
{
	Q_ASSERT(m_activeTool);

	if(!m_model) {
		qWarning("ToolController::endDrawing: no model set!");
		return;
	}

	// Drain any remaining points from the smoothing buffer
	if(m_smoothing>0 && m_activeTool->allowSmoothing()) {
		if(m_smoother.hasSmoothPoint())
			m_smoother.removePoint();
		while(m_smoother.hasSmoothPoint()) {
			m_activeTool->motion(m_smoother.smoothPoint(),
				m_prevShift, m_prevAlt);
			m_smoother.removePoint();
		}
	}

	m_activeTool->end();
	m_model->stateTracker()->setLocalDrawingInProgress(false);
}

bool ToolController::undoMultipartDrawing()
{
	Q_ASSERT(m_activeTool);

	if(!m_model) {
		qWarning("ToolController::undoMultipartDrawing: no model set!");
		return false;
	}

	if(!m_activeTool->isMultipart())
		return false;

	m_activeTool->undoMultipart();
	return true;
}

bool ToolController::isMultipartDrawing() const
{
	Q_ASSERT(m_activeTool);

	return m_activeTool->isMultipart();
}

void ToolController::finishMultipartDrawing()
{
	Q_ASSERT(m_activeTool);

	if(!m_model) {
		qWarning("ToolController::finishMultipartDrawing: no model set!");
		return;
	}

	if(m_model->stateTracker()->isLayerLocked(m_activeLayer)) {
		// It is possible for the active layer to become locked
		// before the user has finished multipart drawing.
		qWarning("Cannot finish multipart drawing: active layer is locked!");
		return;
	}

	m_smoother.reset();
	m_activeTool->finishMultipart();
}

void ToolController::cancelMultipartDrawing()
{
	Q_ASSERT(m_activeTool);

	if(!m_model) {
		qWarning("ToolController::cancelMultipartDrawing: no model set!");
		return;
	}

	m_smoother.reset();
	m_activeTool->cancelMultipart();
}

