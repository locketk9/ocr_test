/// @author	locketk9@gmail.com
/// @file draw_pic.h
/// @date 2019-05-23

#pragma once

#include <vector>
#include <cmath>
#include <algorithm>


namespace curve {

	/// @class Point
	class Point {
	public:
		Point(void) {
			x = y = 0;
		}
		Point(double nx, double ny) {
			x = nx;
			y = ny;
		}
		Point(const Point& src) {
			(*this) = src;
		}
		~Point(void) {
		}
		Point& operator=(const Point& src) {
			x = src.x;
			y = src.y;
			return *this;
		}
		Point operator*(const Point& alt) {
			Point p(x * alt.x, y * alt.y);
			return p;
		}
		Point operator/(const Point& alt) {
			Point p(x / alt.x, y / alt.y);
			return p;
		}
		Point operator-(const Point& alt) {
			Point p(x - alt.x, y - alt.y);
			return p;
		}
		Point operator+(const Point& alt) {
			Point p(x + alt.x, y + alt.y);
			return p;
		}
		Point operator*(double num) {
			Point p(x * num, y * num);
			return p;
		}
		Point operator/(double num) {
			Point p(x / num, y / num);
			return p;
		}
		Point operator-(double num) {
			Point p(x - num, y - num);
			return p;
		}
		Point operator+(double num) {
			Point p(x + num, y + num);
			return p;
		}
		Point operator*=(const Point& alt) {
			x *= alt.x;
			y *= alt.y;
			return *this;
		}
		Point operator/=(const Point& alt) {
			x /= alt.x;
			y /= alt.y;
			return *this;
		}
		Point operator-=(const Point& alt) {
			x -= alt.x;
			y -= alt.y;
			return *this;
		}
		Point operator+=(const Point& alt) {
			x += alt.x;
			y += alt.y;
			return *this;
		}
		Point operator*=(double num) {
			x *= num;
			y *= num;
			return *this;
		}
		Point operator/=(double num) {
			x /= num;
			y /= num;
			return *this;
		}
		Point operator-=(double num) {
			x -= num;
			y -= num;
			return *this;
		}
		Point operator+=(double num) {
			x += num;
			y += num;
			return *this;
		}

		//private:
		double x;
		double y;
	};

	using vertex = std::vector<Point>;

	/* Drawing constants. */
#define BORDER        10  /* border width in each viewport                      */
#define MAX_POINTS   100  /* maximum number of control points                   */
#define MARK_FACTOR 0.35  /* scale factor for 'x' that marks each control point */
#define DELTA_T     0.05  /* time step factor for drawing each curve            */


/// @class CDrawCurve
	class CDrawCurve {
	public:
		CDrawCurve() {}
		~CDrawCurve() {}

		int setPoints(const std::vector<Point> &pts) {
			pts_.clear();
			in_.clear();
			in_.reserve(pts.size());
			in_.assign(pts.begin(), pts.end());
			interact();
			return 0;
		}

		enum CurveType { mark, lagrange, bezier, spline };

		/// @brief setPoints() must be called previously.
		int drawCurve(CurveType ct) {
			switch (ct) {
			case mark: mark_points(); break;
			case lagrange: Lagrange_interpolate(); break;
			case bezier: Bezier(); break;
			case spline: Spline(); break;
			default:
				return -1;
			}

			return 0;
		}

		const std::vector<Point>& getPts() const {
			return pts_;
		}

	private:
		/* This function gets the input data for the program to process. */
		void interact(void)
		{
			number_of_points = 0;

			/* determining maximum and minimum values. */
			maxx = maxy = -1.0e38;
			minx = miny = 1.0e38;

			for (auto &e : in_)
			{
				px[number_of_points] = e.x;
				py[number_of_points] = e.y;

				if (px[number_of_points] < minx) minx = px[number_of_points];
				if (px[number_of_points] > maxx) maxx = px[number_of_points];
				if (py[number_of_points] < miny) miny = py[number_of_points];
				if (py[number_of_points] > maxy) maxy = py[number_of_points];
				++number_of_points;
				if (number_of_points >= MAX_POINTS)
				{
					break;
				}
			}

			/* Determine length of line segments for making 'x' marks. */
			if (maxx - minx > maxy - miny)
				markd = (maxx - minx) / number_of_points * MARK_FACTOR;
			else
				markd = (maxy - miny) / number_of_points * MARK_FACTOR;
		}

		/* This routine makes a mark for each data point in the arrays. */
		void mark_points()
		{
			int i;

			std::vector<Point> vertex(number_of_points);
			for (i = 0; i < number_of_points; i++)
			{
				pts_.push_back(Point(px[i] - markd, py[i] - markd));
				pts_.push_back(Point(px[i] + markd, py[i] + markd));
				pts_.push_back(Point(px[i] - markd, py[i] + markd));
				pts_.push_back(Point(px[i] + markd, py[i] - markd));
			}

		}

		/* Evaluates the blending functions for Lagrange interpolation. */
		double B(int n, double t)
		{
			switch (n)
			{
			case 1: return -t * (t - 1.0)*(t - 2.0) / 6.0;
				break;
			case 2: return (t + 1.0)*(t - 1.0)*(t - 2.0) / 2.0;
				break;
			case 3: return -(t + 1.0)*t*(t - 2.0) / 2.0;
				break;
			case 4: return (t + 1.0)*t*(t - 1.0) / 6.0;
				break;
			}
			return 0.0;  // default case, should never happen
		}

		/* This procedure does Lagrange interpolation of the data. */
		void Lagrange_interpolate()
		{
			int i;
			double t, x, y, b1, b2, b3, b4;

			/* Handle first set of 4 points between t=-1 and t=0 separately. */
			for (t = -1.0; t < DELTA_T / 2.0; t += DELTA_T)
			{
				b1 = B(1, t);
				b2 = B(2, t);
				b3 = B(3, t);
				b4 = B(4, t);
				x = px[0] * b1 + px[1] * b2 + px[2] * b3 + px[3] * b4;
				y = py[0] * b1 + py[1] * b2 + py[2] * b3 + py[3] * b4;
				pts_.push_back(Point(x, y));
			}

			/* Handle middle segments. */
			for (i = 1; i <= number_of_points - 3; i++)
			{
				for (t = DELTA_T; t < 1.0 + DELTA_T / 2.0; t += DELTA_T)
				{
					b1 = B(1, t);
					b2 = B(2, t);
					b3 = B(3, t);
					b4 = B(4, t);
					x = px[i - 1] * b1 + px[i] * b2 + px[i + 1] * b3 + px[i + 2] * b4;
					y = py[i - 1] * b1 + py[i] * b2 + py[i + 1] * b3 + py[i + 2] * b4;
					pts_.push_back(Point(x, y));
				}
			}

			/* Handle the last set of 4 points between t=1.0 and t=2.0 separately. */
			for (t = 1.0 + DELTA_T; t < 2.0 + DELTA_T / 2.0; t += DELTA_T)
			{
				b1 = B(1, t);
				b2 = B(2, t);
				b3 = B(3, t);
				b4 = B(4, t);
				x = px[number_of_points - 4] * b1 + px[number_of_points - 3] * b2 +
					px[number_of_points - 2] * b3 + px[number_of_points - 1] * b4;
				y = py[number_of_points - 4] * b1 + py[number_of_points - 3] * b2 +
					py[number_of_points - 2] * b3 + py[number_of_points - 1] * b4;
				pts_.push_back(Point(x, y));
			}
		}

		/* This function approximates the data with Bezier curves. */
		void Bezier()
		{
			int i, n;
			double ax, bx, cx, dx, ay, by, dy, cy, t, x, y;
			double delta_t = DELTA_T / 4.0;

			/* Make sure number of points is one more than a multiple of 3. */
			switch (number_of_points % 3)
			{
			case 0: px[number_of_points] = px[number_of_points - 1];
				py[number_of_points] = py[number_of_points - 1];
				n = number_of_points + 1;
				break;
			case 1: n = number_of_points;
				break;
			case 2: n = number_of_points + 2;
				px[n - 1] = px[n - 2] = px[number_of_points - 1];
				py[n - 1] = py[n - 2] = py[number_of_points - 1];
				break;
			}

			/* Construct Bezier curves for each grouping of four points. */
			for (i = 0; i < n - 1; i += 3)
			{
				ax = -px[i] + 3.0*(px[i + 1] - px[i + 2]) + px[i + 3];
				ay = -py[i] + 3.0*(py[i + 1] - py[i + 2]) + py[i + 3];
				bx = 3.0*(px[i] - 2.0*px[i + 1] + px[i + 2]);
				by = 3.0*(py[i] - 2.0*py[i + 1] + py[i + 2]);
				cx = -3.0*(px[i] - px[i + 1]);
				cy = -3.0*(py[i] - py[i + 1]);
				x = dx = px[i];
				y = dy = py[i];
				pts_.push_back(Point(x, y));
				for (t = delta_t; t < 1.0 + delta_t / 2.0; t += delta_t)
				{
					x = ((ax*t + bx)*t + cx)*t + dx;
					y = ((ay*t + by)*t + cy)*t + dy;
					pts_.push_back(Point(x, y));
				}
			}
		}

		/* This function evaluates the uniform cubic B-spline. */
		double b(double t)
		{
			double tp2, tp1, tm2, tm1;

			if (t <= -2.0)
				return 0.0;
			else if (t <= -1.0)
			{
				tp2 = t + 2.0;
				return tp2 * tp2 * tp2 / 6.0;
			}
			else if (t <= 0.0)
			{
				tp2 = t + 2.0;
				tp1 = t + 1.0;
				tp2 = tp2 * tp2 * tp2 / 6.0;
				tp1 = 2.0 * tp1 * tp1 * tp1 / 3.0;
				return tp2 - tp1;
			}
			else if (t <= 1.0)
			{
				tm1 = 1.0 - t;
				tm2 = 2.0 - t;
				tm1 = 2.0 * tm1 * tm1 * tm1 / 3.0;
				tm2 = tm2 * tm2 * tm2 / 6.0;
				return tm2 - tm1;
			}
			else if (t <= 2.0)
			{
				tm2 = 2.0 - t;
				return tm2 * tm2 * tm2 / 6.0;
			}
			else
				return 0.0;
		}

		/* This function approximates the data with spline curves. */
		void Spline()
		{
			double xs[MAX_POINTS + 4], ys[MAX_POINTS + 4];
			double x, y, t, bt1, bt2, bt3, bt4;
			int i;

			/* Load local arrays with data and make the two endpoints multiple so that
			* they are interpolated. */
			xs[0] = xs[1] = px[0];
			ys[0] = ys[1] = py[0];
			for (i = 0; i < number_of_points; i++)
			{
				xs[i + 2] = px[i];
				ys[i + 2] = py[i];
			}
			xs[number_of_points + 2] = xs[number_of_points + 3] = px[number_of_points - 1];
			ys[number_of_points + 2] = ys[number_of_points + 3] = py[number_of_points - 1];

			/* Compute the values to plot. */
			pts_.push_back(Point(px[0], py[0]));
			for (i = 0; i <= number_of_points; i++)
				for (t = DELTA_T; t < 1.0 + DELTA_T / 2.0; t += DELTA_T)
				{
					bt1 = b(t - 2.0);
					bt2 = b(t - 1.0);
					bt3 = b(t);
					bt4 = b(t + 1.0);
					x = xs[i] * bt4 + xs[i + 1] * bt3 + xs[i + 2] * bt2 + xs[i + 3] * bt1;
					y = ys[i] * bt4 + ys[i + 1] * bt3 + ys[i + 2] * bt2 + ys[i + 3] * bt1;
					pts_.push_back(Point(x, y));
				}
		}

		double distance(double x1, double y1, double x2, double y2) {
			return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));;
		}

#if 0	
		Point bezier(std::vector<Point>& pts, double t) {
			Point p;
			std::size_t len = pts.size();
			for (std::size_t i = 0; i < len; i++) {
				p += pts[i] * choose(len - 1, i) * pow(1 - t, len - 1 - i) * pow(t, i);
			}
			return p;
		}

		double factorial(double num) {
			if (num <= 1) {
				return 1;
			}
			double fac = 1;
			for (double i = 1; i <= num; i++) {
				fac *= i;
			}
			return fac;
		}

		double choose(double a, double b) {
			return factorial(a) / (factorial(b) * factorial(a - b));
		}
#endif
	private:
		std::vector<Point> pts_;

		double px[MAX_POINTS], py[MAX_POINTS], minx = 0.0, maxx = 0.0, miny = 0.0, maxy = 0.0, markd = 0.0;
		int number_of_points = 0;

		std::vector<Point> in_;
	};

}