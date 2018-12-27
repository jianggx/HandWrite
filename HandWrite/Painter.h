#pragma once

#include "PPoint.h"

class Painter
{
public:
   Painter():m_penwidth(5){};
   virtual ~Painter() {};
   virtual void PenMove(const std::vector<PPoint>& pts)=0;
   virtual void PenUp()=0;
   void setPenWidth(double penwidth) { m_penwidth = penwidth; }

protected:
	double m_penwidth;
};

