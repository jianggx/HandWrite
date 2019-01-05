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
#include"stdafx.h"
#include "strokesmoother.h"
#include <algorithm>

StrokeSmoother::StrokeSmoother()
{
	reset();
}

void StrokeSmoother::setSmoothing(int strength)
{
	assert(strength>0);
	_points.resize(strength);
	reset();
}

void StrokeSmoother::addPoint(const PPoint &point)
{
	int size = _points.size();
	assert(size>0);

	if(_count == 0) {
		/* Pad the buffer with this point, so we blend away from it
		 * gradually as we gain more points. We still only count this
		 * as one point so we know how much real data we have to
		 * drain if it was a very short stroke. */
      std::fill(_points.begin(), _points.end(), point);
	  _sum.rx() = point.x() * size;
	  _sum.ry() = point.y() * size;
	  _sum.rp() = point.p() * size;
		//_points.fill(point);
	} else {
		if(--_pos < 0)
			_pos = _points.size()-1;
		
		_sum.rx() += point.x()- _points[_pos].x();
		_sum.ry() += point.y() - _points[_pos].y();
		_sum.rp() += point.p() - _points[_pos].p();

		_points[_pos] = point;
	}

	if(_count < _points.size())
		++_count;
}

PPoint StrokeSmoother::at(int i) const
{
	return _points.at((_pos+i) % _points.size());
}

void StrokeSmoother::setLastPointPressure(double pressure)
{
	PPoint& point = _points.at(_pos % _points.size());
	_sum.rp() += pressure - point.m_p;

	point.setPressure(pressure);
}

void StrokeSmoother::reset()
{
	_count=0;
	_pos = 0;
}

bool StrokeSmoother::hasSmoothPoint() const
{
	return _count > 0;
}

PPoint StrokeSmoother::smoothPoint() const
{
	assert(hasSmoothPoint());

	// A simple unweighted sliding-average smoother
	PPoint p = at(0);

	const double c = _points.size();
	p.rx() = _sum.x() / c;
	p.ry() = _sum.y() / c;
	p.rp() = _sum.p() / c;

	return p;
}

void StrokeSmoother::removePoint()
{
	assert(_points.size()!=0);
	/* Pad the buffer with the final point, overwriting the oldest first,
	 * for symmetry with starting. For very short strokes this should
	 * really set all points between --_count and _points.size()-1. */
	PPoint& point = _points[(_pos + --_count) % _points.size()];
	PPoint lp = latestPoint();
	_sum.rx() += lp.x() - point.x();
	_sum.ry() += lp.y() - point.y();
	_sum.rp() += lp.p() - point.p();

	_points[(_pos + --_count) % _points.size()] = lp;
}
