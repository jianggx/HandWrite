#pragma once
class Point
{
public:
   double m_x;
   double m_y;

   Point() :m_x(0), m_y(0) {};
   Point(double x, double y) :m_x(x), m_y(y){};


   double x() const { return m_x; }
   double y() const { return m_y; }

   double& rx() { return m_x; }
   double& ry() { return m_y; }

   //! Compare two points at subpixel resolution
   static bool roughlySame(const Point& p1, const Point &p2) {
      double dx = p1.x() - p2.x();
      double dy = p1.y() - p2.y();
      double d = dx*dx + dy*dy;
      return d <= ((1 / 4.0)*(1 / 4.0));
   }
   bool roughlySame(const Point& point) const { return roughlySame(*this, point); }

   //! Are the two points less than one pixel different?
   static bool intSame(const Point &p1, const Point &p2) {
      double dx = p1.x() - p2.x();
      double dy = p1.y() - p2.y();
      double d = dx*dx + dy*dy;
      return d < 1.0;
   }
   bool intSame(const Point &point) const { return intSame(*this, point); }

   static double distance(const Point &p1, const Point &p2)
   {
      return hypot(p1.x() - p2.x(), p1.y() - p2.y());
   }
   double distance(const Point &point) const { return distance(*this, point); }

};

static inline Point operator-(const Point& p1, const Point& p2)
{
   return Point(p1.x() - p2.x(), p1.y() - p2.y());
}

static inline Point operator+(const Point& p1, const Point& p2)
{
   return Point(p1.x() + p2.x(), p1.y() + p2.y());
}

static inline bool operator==(const Point& p1, const Point& p2)
{
   return Point::roughlySame(p1, p2);
}


////////////////////////////////////////////////////////////
class PPoint
{
public:
   double m_x;
   double m_y;
   double m_p; // Ñ¹Á¦

   PPoint() :m_x(0), m_y(0), m_p(0) {};
   PPoint(double x, double y, double p = 1.0) :m_x(x), m_y(y), m_p(p) {};
   PPoint(const Point& pt, double p = 1.0) :m_x(pt.x()), m_y(pt.y()), m_p(p) {};
   ~PPoint() {};

   double x() const { return m_x; }
   double y() const { return m_y; }
   double p() const { return m_p; }

   double& rx() { return m_x; }
   double& ry() { return m_y; }
   double& rp() { return m_p; }

   //! Set this point's pressure value
   void setPressure(double p) { assert(p >= 0 && p <= 1); m_p = p; }

   //! Compare two points at subpixel resolution
   static bool roughlySame(const PPoint& p1, const PPoint &p2) {
      double dx = p1.x() - p2.x();
      double dy = p1.y() - p2.y();
      double d = dx*dx + dy*dy;
      return d <= ((1 / 4.0)*(1 / 4.0));
   }
   bool roughlySame(const PPoint& point) const { return roughlySame(*this, point); }

   //! Are the two points less than one pixel different?
   static bool intSame(const PPoint &p1, const PPoint &p2) {
      double dx = p1.x() - p2.x();
      double dy = p1.y() - p2.y();
      double d = dx*dx + dy*dy;
      return d < 1.0;
   }
   bool intSame(const PPoint &point) const { return intSame(*this, point); }

   static double distance(const PPoint &p1, const PPoint &p2)
   {
      return hypot(p1.x() - p2.x(), p1.y() - p2.y());
   }
   double distance(const PPoint &point) const { return distance(*this, point); }

};

static inline PPoint operator-(const PPoint& p1, const PPoint& p2)
{
   return PPoint(p1.x() - p2.x(), p1.y() - p2.y(), p1.p());
}

static inline PPoint operator+(const PPoint& p1, const PPoint& p2)
{
   return PPoint(p1.x() + p2.x(), p1.y() + p2.y(), p1.p());
}

static inline bool operator==(const PPoint& p1, const PPoint& p2)
{
   return PPoint::roughlySame(p1, p2);
}