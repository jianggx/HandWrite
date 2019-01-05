/*
   Drawpile - a collaborative drawing program.

   Copyright (C) 2014 Calle Laakkonen

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
#ifndef STROKESMOOTHER_H
#define STROKESMOOTHER_H

#include "PPoint.h"
#include<vector>

class ToolController;

class StrokeSmoother
{
public:
	StrokeSmoother();

	/**
	 * @brief Set smoothing strength
	 *
	 * The higher the value, the smoother the stroke will be.
	 * Strength should be greater than zero.
	 * @param strength
	 */
	void setSmoothing(int strength);

	/**
	 * @brief Reset smoother for a new stroke
	 */
	void reset();

	/**
	 * @brief Add a point to the smoother
	 * @param point
	 */
	void addPoint(const PPoint &point);

	/**
	 * @brief Is a smoothed point available?
	 *
	 * @return true if smoothPoint will return a valid value
	 */
	bool hasSmoothPoint() const;

	/**
	 * @brief Get the smoothed point
	 *
	 * @return smoothed point
	 * @pre hasSmoothPoint() == true
	 */
	PPoint smoothPoint() const;

	/**
	 * @brief Remove one point from the buffer, for ending a line
	 *
	 * @pre hasSmoothPoint() == true
	 */
	void removePoint();

	/**
	 * @brief Get the last point added to the smoother
	 * @return
	 */
   PPoint latestPoint() const { return at(0); }

   void setLastPointPressure(double pressure);

private:
	PPoint at(int i)const;

   friend class ToolController;
	std::vector<PPoint> _points;
	int _pos;
	int _count; ///< Number of actually sampled points in the buffer

	PPoint _sum;
};

#endif // STROKESMOOTHER_H
