#pragma once
class Point
{
public:
   float m_x;
   float m_y;

   Point() :m_x(0), m_y(0) {};
   Point(float x, float y) :m_x(x), m_y(y){};


   float x() const { return m_x; }
   float y() const { return m_y; }

   float& rx() { return m_x; }
   float& ry() { return m_y; }

   //! Compare two points at subpixel resolution
   static bool roughlySame(const Point& p1, const Point &p2) {
      float dx = p1.x() - p2.x();
      float dy = p1.y() - p2.y();
      float d = dx*dx + dy*dy;
      return d <= ((1 / 4.0)*(1 / 4.0));
   }
   bool roughlySame(const Point& point) const { return roughlySame(*this, point); }

   //! Are the two points less than one pixel different?
   static bool intSame(const Point &p1, const Point &p2) {
      float dx = p1.x() - p2.x();
      float dy = p1.y() - p2.y();
      float d = dx*dx + dy*dy;
      return d < 1.0;
   }
   bool intSame(const Point &point) const { return intSame(*this, point); }

   static float distance(const Point &p1, const Point &p2)
   {
      return hypot(p1.x() - p2.x(), p1.y() - p2.y());
   }
   float distance(const Point &point) const { return distance(*this, point); }

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
   float m_x;
   float m_y;
   float m_p; // Ñ¹Á¦

   PPoint() :m_x(0), m_y(0), m_p(0) {};
   PPoint(float x, float y, float p = 1.0) :m_x(x), m_y(y), m_p(p) {};
   PPoint(const Point& pt, float p = 1.0) :m_x(pt.x()), m_y(pt.y()), m_p(p) {};
   ~PPoint() {};

   float x() const { return m_x; }
   float y() const { return m_y; }
   float pressure() const { return m_p; }

   float& rx() { return m_x; }
   float& ry() { return m_y; }

   //! Set this point's pressure value
   void setPressure(float p) { assert(p >= 0 && p <= 1); m_p = p; }

   //! Compare two points at subpixel resolution
   static bool roughlySame(const PPoint& p1, const PPoint &p2) {
      float dx = p1.x() - p2.x();
      float dy = p1.y() - p2.y();
      float d = dx*dx + dy*dy;
      return d <= ((1 / 4.0)*(1 / 4.0));
   }
   bool roughlySame(const PPoint& point) const { return roughlySame(*this, point); }

   //! Are the two points less than one pixel different?
   static bool intSame(const PPoint &p1, const PPoint &p2) {
      float dx = p1.x() - p2.x();
      float dy = p1.y() - p2.y();
      float d = dx*dx + dy*dy;
      return d < 1.0;
   }
   bool intSame(const PPoint &point) const { return intSame(*this, point); }

   static float distance(const PPoint &p1, const PPoint &p2)
   {
      return hypot(p1.x() - p2.x(), p1.y() - p2.y());
   }
   float distance(const PPoint &point) const { return distance(*this, point); }

};

static inline PPoint operator-(const PPoint& p1, const PPoint& p2)
{
   return PPoint(p1.x() - p2.x(), p1.y() - p2.y(), p1.pressure());
}

static inline PPoint operator+(const PPoint& p1, const PPoint& p2)
{
   return PPoint(p1.x() + p2.x(), p1.y() + p2.y(), p1.pressure());
}

static inline bool operator==(const PPoint& p1, const PPoint& p2)
{
   return PPoint::roughlySame(p1, p2);
}