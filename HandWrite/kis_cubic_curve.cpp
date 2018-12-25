/*
 *  Copyright (c) 2005 C. Boemann <cbo@boemann.dk>
 *  Copyright (c) 2009 Dmitry Kazakov <dimula73@gmail.com>
 *  Copyright (c) 2010 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include"stdafx.h"
#include "kis_cubic_curve.h"
#include<list>
#include<math.h>

#include <algorithm>

class KisTridiagonalSystem
{
    /*
     * e.g.
     *      |b0 c0  0   0   0| |x0| |f0|
     *      |a0 b1 c1   0   0| |x1| |f1|
     *      |0  a1 b2  c2   0|*|x2|=|f2|
     *      |0   0 a2  b3  c3| |x3| |f3|
     *      |0   0  0  a3  b4| |x4| |f4|
     */

public:

    /**
     * @return - vector that is storing x[]
     */
    static
    std::vector<float> calculate(std::vector<float> &a,
                         std::vector<float> &b,
                         std::vector<float> &c,
                         std::vector<float> &f) {
        std::vector<float> x;
        std::vector<float> alpha;
        std::vector<float> beta;

        int i;
        int size = b.size();

        assert(a.size() == size - 1 &&
                 c.size() == size - 1 &&
                 f.size() == size);

        x.resize(size);

        /**
         * Check for special case when
         * order of the matrix is equal to 1
         */
        if (size == 1) {
            x[0] = f[0] / b[0];
            return x;
        }

        /**
         * Common case
         */

        alpha.resize(size);
        beta.resize(size);


        alpha[1] = -c[0] / b[0];
        beta[1] =  f[0] / b[0];

        for (i = 1; i < size - 1; i++) {
            alpha[i+1] = -c[i] /
                         (a[i-1] * alpha[i] + b[i]);

            beta[i+1] = (f[i] - a[i-1] * beta[i])
                        /
                        (a[i-1] * alpha[i] + b[i]);
        }

        x.back() = (f.back() - a.back() * beta.back())
                   /
                   (b.back() + a.back() * alpha.back());

        for (i = size - 2; i >= 0; i--)
            x[i] = alpha[i+1] * x[i+1] + beta[i+1];

        return x;
    }
};

class KisCubicSpline
{
    /**
     *  s[i](x)=a[i] +
     *          b[i] * (x-x[i]) +
     *    1/2 * c[i] * (x-x[i])^2 +
     *    1/6 * d[i] * (x-x[i])^3
     *
     *  h[i]=x[i+1]-x[i]
     *
     */

protected:
    std::vector<float> m_a;
    std::vector<float> m_b;
    std::vector<float> m_c;
    std::vector<float> m_d;

    std::vector<float> m_h;
    float m_begin;
    float m_end;
    int m_intervals;

public:
    KisCubicSpline() {}
    KisCubicSpline(const std::vector<PPoint> &a) {
        createSpline(a);
    }

    /**
     * Create new spline and precalculate some values
     * for future
     *
     * @a - base points of the spline
     */
    void createSpline(const std::vector<PPoint> &a) {
        int intervals = m_intervals = a.size() - 1;
        int i;
        m_begin = a.begin()->x();
        m_end = a.back().x();

        m_a.clear();
        m_b.resize(intervals);
        m_c.clear();
        m_d.resize(intervals);
        m_h.resize(intervals);

        for (i = 0; i < intervals; i++) {
            m_h[i] = a[i+1].x() - a[i].x();
            m_a.push_back(a[i].y());
        }
        m_a.push_back(a.back().y());


        std::vector<float> tri_b;
        std::vector<float> tri_f;
        std::vector<float> tri_a; /* equals to @tri_c */

        for (i = 0; i < intervals - 1; i++) {
            tri_b.push_back(2.*(m_h[i] + m_h[i+1]));

            tri_f.push_back(6.*((m_a[i+2] - m_a[i+1]) / m_h[i+1] - (m_a[i+1] - m_a[i]) / m_h[i]));
        }
        for (i = 1; i < intervals - 1; i++)
            tri_a.push_back(m_h[i]);

        if (intervals > 1) {
            m_c = KisTridiagonalSystem::calculate(tri_a, tri_b, tri_a, tri_f);
        }
        m_c.insert(m_c.begin(),0);
        m_c.push_back(0);

        for (i = 0; i < intervals; i++)
            m_d[i] = (m_c[i+1] - m_c[i]) / m_h[i];

        for (i = 0; i < intervals; i++)
            m_b[i] = -0.5 * (m_c[i] * m_h[i])  - (1 / 6.0) * (m_d[i] * m_h[i] * m_h[i]) + (m_a[i+1] - m_a[i]) / m_h[i];
    }

    /**
     * Get value of precalculated spline in the point @x
     */
    float getValue(float x) const {
        float x0;
        int i = findRegion(x, x0);
        /* TODO: check for asm equivalent */
        return m_a[i] +
               m_b[i] *(x - x0) +
               0.5 * m_c[i] *(x - x0) *(x - x0) +
               (1 / 6.0)* m_d[i] *(x - x0) *(x - x0) *(x - x0);
    }

    float begin() const {
        return m_begin;
    }

    float end() const {
        return m_end;
    }

protected:

    /**
     * findRegion - Searches for the region containing @x
     * @x0 - out parameter, containing beginning of the region
     * @return - index of the region
     */
    int findRegion(float x, float &x0) const {
        int i;
        x0 = m_begin;
        for (i = 0; i < m_intervals; i++) {
            if (x >= x0 && x < x0 + m_h[i])
                return i;
            x0 += m_h[i];
        }
        if (x >= x0) {
            x0 -= m_h[m_intervals-1];
            return m_intervals - 1;
        }

        /* **never reached** */
        return -1;
    }
};

static bool pointLessThan(const PPoint &a, const PPoint &b)
{
    return a.x() < b.x();
}

struct KisCubicCurve::Data  {
    Data() {
        init();
    }
    Data(const Data& data){
        init();
        points = data.points;
    }
    void init() {
        validSpline = false;
        validU16Transfer = false;
        validFTransfer = false;
    }
    ~Data() {
    }

    mutable KisCubicSpline spline;
    std::vector<PPoint> points;
    mutable bool validSpline;
    mutable std::vector<uint16_t> u16Transfer;
    mutable bool validU16Transfer;
    mutable std::vector<float> fTransfer;
    mutable bool validFTransfer;
    void updateSpline();
    void keepSorted();
    float value(float x);
    void invalidate();
};

void KisCubicCurve::Data::updateSpline()
{
    if (validSpline) return;
    validSpline = true;
    spline.createSpline(points);
}

void KisCubicCurve::Data::invalidate()
{
    validSpline = false;
    validFTransfer = false;
    validU16Transfer = false;
}

void KisCubicCurve::Data::keepSorted()
{
	std::sort(points.begin(), points.end(), pointLessThan);
}

float qBound(float x0, float x, float x1)
{
   if (x < x0)
      return x0;
   if (x > x1)
      return x1;
   return x;
}
float KisCubicCurve::Data::value(float x)
{
    updateSpline();
    /* Automatically extend non-existing parts of the curve
     * (e.g. before the first point) and cut off big y-values
     */
    x = qBound(spline.begin(), x, spline.end());
    float y = spline.getValue(x);
    return qBound(float(0.0), y, float(1.0));
}

KisCubicCurve::KisCubicCurve() 
{
    d.reset(new Data);
    PPoint p;
    p.rx() = 0.0; p.ry() = 0.0;
    d->points.push_back(p);
    p.rx() = 1.0; p.ry() = 1.0;
    d->points.push_back(p);
}

KisCubicCurve::KisCubicCurve(const std::vector<PPoint>& points)
{
   d.reset(new Data);
   d->points = points;
   d->keepSorted();
}


KisCubicCurve::~KisCubicCurve()
{
}

KisCubicCurve& KisCubicCurve::operator=(const KisCubicCurve & curve)
{
    if (&curve != this) {
        *d = *curve.d;
    }
    return *this;
}

bool KisCubicCurve::operator==(const KisCubicCurve& curve) const
{
    if (d.get() == curve.d.get()) return true;
    return d->points == curve.d->points;
}

float KisCubicCurve::value(float x) const
{
    float value = d->value(x);
    return value;
}

std::vector<PPoint> KisCubicCurve::points() const
{
    return d->points;
}

void KisCubicCurve::setPoints(const std::vector<PPoint>& points)
{
    d->points = points;
    d->invalidate();
}


int StringSplit(std::vector<std::string>& dst, const std::string& src, const std::string& separator)
{
   if (src.empty() || separator.empty())
      return 0;

   int nCount = 0;
   std::string temp;
   size_t pos = 0, offset = 0;

   // 分割第1~n-1个
   while ((pos = src.find_first_of(separator, offset)) != std::string::npos)
   {
      temp = src.substr(offset, pos - offset);
      if (temp.length() > 0) {
         dst.push_back(temp);
         nCount++;
      }
      offset = pos + 1;
   }

   // 分割第n个
   temp = src.substr(offset, src.length() - offset);
   if (temp.length() > 0) {
      dst.push_back(temp);
      nCount++;
   }

   return nCount;
}


void KisCubicCurve::fromString(const std::string& string)
{
   std::vector<std::string> data;
   StringSplit(data, string, std::string(";"));

   std::vector<PPoint> points;

   for (const std::string & pair : data) {
      if (pair.find_first_of(std::string(","), 0) != std::string::npos) {
         PPoint p;
         std::vector<std::string> data2;
         StringSplit(data2, pair, std::string(","));
         if (data2.size() != 2)
            continue;

         p.rx() = qBound(0.0, atof(data2[0].c_str()), 1.0);
         p.ry() = qBound(0.0, atof(data2[1].c_str()), 1.0);
         points.push_back(p);
      }
   }

   if (points.size() < 2)
   {
      points.clear();
      points.push_back(PPoint(0.0, 0.0, 0));
      points.push_back(PPoint(1.0, 1.0, 0));
   }

   setPoints(points);
}