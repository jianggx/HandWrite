/*
   Drawpile - a collaborative drawing program.

   Copyright (C) 2015 Calle Laakkonen

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

#ifndef CANVAS_PRESSURE_H
#define CANVAS_PRESSURE_H

#include "kis_cubic_curve.h"
#include <math.h>

struct PressureMapping {
	enum Mode {
		STYLUS, // real tablet pressure info, no parameter
		DISTANCE, // param=max distance
		VELOCITY // param=max velocity
	};

	Mode mode;
	KisCubicCurve curve;
	float param;

   PressureMapping() : mode(STYLUS), param(1.0) { };

   float mapPressure(float value)
   {
      switch (mode) {
      case PressureMapping::STYLUS:
         return curve.value(value);

      case PressureMapping::DISTANCE: {
         float d = min(value, param) / param;
         return curve.value(d);
      }

      case PressureMapping::VELOCITY:
         float v = min(value, param) / param;
         return curve.value(v);
      }
      return 0;
   }
};

#endif

