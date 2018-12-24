/*
   Drawpile - a collaborative drawing program.

   Copyright (C) 2006-2015 Calle Laakkonen

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
#ifndef TOOLS_TOOL_H
#define TOOLS_TOOL_H

#include "PPoint.h"


/**
 * @brief Tools
 *
 * Tools translate commands from the local user into messages that
 * can be sent over the network.
 * Read-only tools can access the canvas directly.
 */


class Painter;

/**
 * @brief Base class for all tools
 * Tool classes interpret mouse/pen commands into editing actions.
 */
class Tool
{
public:
	enum Type {FREEHAND,  _LASTTOOL};

	Tool(Painter &painter, Type type)
		: painter(painter), m_type(type)
		{}
	virtual ~Tool() {}

	Type type() const { return m_type; }

	/**
	 * @brief Start a new stroke
	 * @param point starting point
	 * @param right is the right mouse/pen button pressed instead of the left one
	 * @param zoom the current view zoom factor
	 */
	virtual void begin(const PPoint& point) = 0;

	/**
	 * @brief Continue a stroke
	 * @param point new point
	 * @param constrain is the "constrain motion" button pressed
	 * @param cener is the "center on start point" button pressed
	 */
	virtual void motion(const PPoint& point) = 0;


	//! End stroke
	virtual void end() = 0;


	//! Does this tool allow stroke smoothing to be used?
	virtual bool allowSmoothing() const { return false; }

   Painter &getPainter() { return painter; }

protected:
   Painter &painter;

private:
	const Type m_type;
};


#endif

