#pragma once

#include "PPoint.h"

class Painter
{
public:
   Painter() {};
   virtual ~Painter() {};
   virtual void PenMove(const std::vector<PPoint>& pts)=0;
   virtual void PenUp()=0;
};

