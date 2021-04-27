#include "stdafx.h"
#include "zhangsuen.h"

namespace zs
{

//-------------------------------------------------------------------------
int num_one_pixel_neighbours(const ch_vec& image, const Point& point, int cx)
{
    int ret = 0, r = point.first, c = point.second;
    for (int i = r - 1; i <= r + 1; i++)
        for (int j = c - 1; j <= c + 1; j++)
            if (i != r || j != c)
                if (image[i*cx+j] >= 1) ret++;
    return ret;
}
//-------------------------------------------------------------------------
int num_zero_pixel_neighbours(const ch_vec& image, const Point& point, int cx)
{
    int ret = 0, r = point.first, c = point.second;
    for (int i = r - 1; i <= r + 1; i++)
        for (int j = c - 1; j <= c + 1; j++)
            if (i != r || j != c)
                if (image[i*cx+j] == 0) ret++;
    return ret;
}
//-------------------------------------------------------------------------
int connectivity(const ch_vec& image, const Point& point, int cx)
{
    int r = point.first, c = point.second, ret = 0;
    if (image[r*cx+c+1] >= 1 && image[(r-1)*cx+c+1] == 0)
        ret++;
    if (image[(r-1)*cx+c+1] >= 1 && image[(r-1)*cx+c] == 0)
        ret++;    
    if (image[(r-1)*cx+c] >= 1 && image[(r-1)*cx+c-1] == 0)
        ret++;
    if (image[(r-1)*cx+c-1] >= 1 && image[r*cx+c-1] == 0)
        ret++;
    if (image[r*cx+c-1] >= 1 && image[(r+1)*cx+c-1] == 0)
        ret++;
    if (image[(r+1)*cx+c-1] >= 1 && image[(r+1)*cx+c] == 0)
        ret++;
    if (image[(r+1)*cx+c] >= 1 && image[(r+1)*cx+c+1] == 0)
        ret++;
    if (image[(r+1)*cx+c+1] >= 1 && image[r*cx+c+1] == 0)
        ret++;
    return ret;
}
//-------------------------------------------------------------------------
int yokoi_connectivity(const ch_vec& image, const Point& point, int cx)
{
    int r = point.first, c = point.second;
    // We use x != 0 to map all zero values to 0 and all non-zero values
    // to 1.
    std::vector<int> N(8);
	
     N[0] =   image[r*cx+ c + 1] != 0;
     N[1] =    image[(r - 1)*cx+ c + 1] != 0;
     N[2] =    image[(r - 1)*cx+ c] != 0;
     N[3] =    image[(r - 1)*cx+ c - 1] != 0;
     N[4] =    image[r*cx+ c - 1] != 0;
     N[5] =    image[(r + 1)*cx+ c - 1] != 0;
     N[6] =    image[(r + 1)*cx+ c] != 0;
     N[7] =    image[(r + 1)*cx+ c + 1] != 0;
    
    int ret = 0;
    for (std::vector<int>::size_type i = 1; i < N.size(); i += 2)
    {
        int i1 = (i + 1) % 8, i2 = (i + 2) % 8;
        ret += N[i] - N[i] * N[i1] * N[i2];
    }
    return ret;
}
//-------------------------------------------------------------------------
void delete_pixels(ch_vec& image, std::set<Point>& points, int cx)
{
    for (auto& point : points)
    {
        image[point.first*cx+ point.second] = 0;
    }
    points.clear();
}
//-------------------------------------------------------------------------
void remove_staircases(ch_vec& image, int cx, int cy)
{
    std::set<Point> points;
    for (int i = 0; i < 2; i++)
    {
        for (int i = 1; i < cx - 1; i++)
        {
            for (int j = 1; j < cy - 1; j++)
            {
                int c = image[i*cx+ j];
                if (!c) continue;
                int e = image[i*cx+ j + 1],
                    ne = image[(i - 1)*cx+ j + 1],
                    n = image[(i - 1)*cx+ j],
                    nw = image[(i - 1)*cx+ j - 1],
                    w = image[i*cx+ j - 1],
                    sw = image[(i + 1)*cx+ j - 1],
                    s = image[(i + 1)*cx+ j],
                    se = image[(i + 1)*cx+ j + 1];

                if (i == 0)
                {
                    // North biased staircase removal
                    if (!(c && !(n && 
                                 ((e && !ne && !sw && (!w || !s)) || 
                                  (w && !nw && !se && (!e || !s))))))
                    {
                        points.insert(std::make_pair(i, j)); 
                    }
                }
                else
                {
                    // South bias staircase removal
                    if (!(c && !(s && 
		                         ((e && !se && !nw && (!w || !n)) || 
		                          (w && !sw && !ne && (!e || !n))))))
                    {
                        points.insert(std::make_pair(i, j));
                    }

                }
            }
        }
        delete_pixels(image, points, cx);
    }
}
//-------------------------------------------------------------------------
void thin(ch_vec& img, const imgRECT &rect, const imgRECT &seg, bool need_boundary_smoothing,
          bool need_acute_angle_emphasis, bool destair)
{
    // Make everything either 0 or 1. Though we expect a bilevel image, 
    // the high level (white) can also be 255 rather than 1.
    //for (int i = 0; i < img.rows; i++)
	//{
    //    uchar_t *iter = img.ptr<uchar_t>(i);
    //    for (int j = 0; j < img.cols; j++, iter++)
    //        *iter = (uchar_t)(*iter != 0);
    //}

    // If our input image is M x N, create a M + 2 x N + 2 image that is
    // essentially the same image, bordered by 1(white) pixels.
	int x = seg.x, y = seg.y;
	int cx = x + seg.cx, cy = y + seg.cy;
	ch_vec image(img.size());
	image = img;
	//ch_vec image(img.begin(), img.end());

 //   int cx = seg.cx+2, cy = seg.cy+2;
	//ch_vec image((seg.cx+2)*(seg.cy+2));
	//
	//byte *row = const_cast<byte*>(image.data());
	//byte *data = const_cast<byte*>(img.data());
	//for (int y=1; y<seg.cy-1; ++y)
	//{
	//	memcpy(row+(y*(seg.cx+2))+2, data+(y*seg.cx), seg.cx);
	//	//image.insert(image.begin()+(y*(seg.cx+2))+2
	//	//	, img.begin()+(y*seg.cx)
	//	//	, img.begin()+((y+1)*seg.cx) );
	//}
	//

    // Stentiford Boundary smoothing to reduce line fuzz.
    if (need_boundary_smoothing)
        boundary_smooth(image, cx, cy);
    // Acute Angle Emphasis to curb necking.
    if (need_acute_angle_emphasis)
        acute_angle_emphasis(image, cx, cy);
    // The actual zhangsuen thinning procedure would like the black pixels to
    // be 1 and white pixels to be 0. So do that.
    for (int i = y; i < cy; i++)
    {
        for (int j = x; j < cx; j++)
        {
             if (image[i*rect.cx+j] == 255)
               image[i*rect.cx+j] = 1;
            else
               image[i*rect.cx+j] = 0;
        }
    }

    // Now call the actual thinning routine.
	zhangsuen_thin(image, rect, x, y, cx, cy);
	//zhangsuen_thin(image, x, y, cx, cy);
	// Holt's staircase removal
    if (destair)
        remove_staircases(image, cx, cy);
    // Remember in the image we have, 1 means black and 0 means white. We need
    // to restore that to where 255 means white(background) and 0 means
    // black(object).
     for (int i = y; i < cy; i++)
    {
        for (int j = x; j < cx; j++)
        {
            if (image[i*rect.cx+j] == 0)
               image[i*rect.cx+j] = 0;
            else
               image[i*rect.cx+j] = 255;
        }
    }

	 img = image;
}
//-------------------------------------------------------------------------
void zhangsuen_thin(ch_vec& image, int x, int y, int cx, int cy)
{
    while (true)
    {
        std::set<Point> points;
        for (int i = y+1; i < cy - 1; i++)
        {
            for (int j = x+1; j < cx - 1; j++)
            {
                if (image[i*cx+j] != 1) continue;
                Point p(i, j);
                int k = num_one_pixel_neighbours(image, p, cx);
                if ((k >= 2 && k <= 6) && connectivity(image, p, cx) == 1)
                {
					// ws
                    int p1 = image[(i-1)*cx+ j] * 
                             image[(i)*cx+ j-1] *
                             image[(i+1)*cx+ j],

                        p2 = image[(i)*cx+ j-1] *
                             image[(i)*cx+ j+1] *
                             image[(i+1)*cx+ j];

                    if (p1 == 0 && p2 == 0)
                    {
                        points.insert(p);
                    }
                }
            }
        }
        if (points.size() == 0)
            break;
        delete_pixels(image, points, cx);
        for (int i = y+1; i < cy-1; i++)
        {
            for(int j = x+1; j < cx-1; j++)
            {
                if(image[i*cx+ j] != 1) continue;
                Point p(i, j);
                int k = num_one_pixel_neighbours(image, p, cx);
                if ((k >= 2 && k <= 6) && connectivity(image, p, cx) == 1)
                {
					// en
                    int p1 = image[(i - 1)*cx+ j] *
                             image[i*cx+ j + 1] *
                             image[(i + 1)*cx+ j],

                        p2 = image[(i-1)*cx+ j] *
                             image[(i)*cx+ j+1]*
                             image[i*cx+ j - 1];

                    if (p1 == 0 && p2 == 0)
                    {
                        points.insert(p);
                    }
                }
            }
        }
        if (points.size() == 0)
            break;
		delete_pixels(image, points, cx);
    }
}//-------------------------------------------------------------------------
void zhangsuen_thin(ch_vec& image, const imgRECT rect, int x, int y, int cx, int cy)
{
    while (true)
    {
        std::set<Point> points;
        for (int i = y+1; i < cy - 1; i++)
        {
            for (int j = x+1; j < cx - 1; j++)
            {
                if (image[i*rect.cx+j] != 1) continue;
                Point p(i, j);
                int k = num_one_pixel_neighbours(image, p, rect.cx);
                if ((k >= 2 && k <= 6) && connectivity(image, p, rect.cx) == 1)
                {
					// ws
                    int p1 = image[(i-1)*rect.cx+ j] * 
                             image[(i)*rect.cx+ j-1] *
                             image[(i+1)*rect.cx+ j],

                        p2 = image[(i)*rect.cx+ j-1] *
                             image[(i)*rect.cx+ j+1] *
                             image[(i+1)*rect.cx+ j];

                    if (p1 == 0 && p2 == 0)
                    {
                        points.insert(p);
                    }
                }
            }
        }
        if (points.size() == 0)
            break;
        delete_pixels(image, points, rect.cx);
        for (int i = y+1; i < cy-1; i++)
        {
            for(int j = x+1; j < cx-1; j++)
            {
                if(image[i*rect.cx+ j] != 1) continue;
                Point p(i, j);
                int k = num_one_pixel_neighbours(image, p, rect.cx);
                if ((k >= 2 && k <= 6) && connectivity(image, p, rect.cx) == 1)
                {
					// en
                    int p1 = image[(i - 1)*rect.cx+ j] *
                             image[i*rect.cx+ j + 1] *
                             image[(i + 1)*rect.cx+ j],

                        p2 = image[(i-1)*rect.cx+ j] *
                             image[(i)*rect.cx+ j+1]*
                             image[i*rect.cx+ j - 1];

                    if (p1 == 0 && p2 == 0)
                    {
                        points.insert(p);
                    }
                }
            }
        }
        if (points.size() == 0)
            break;
		delete_pixels(image, points, rect.cx);
    }
}
//-------------------------------------------------------------------------
// The Stentiford boundary smoothing to reduce line fuzz after thinning.
// This is expected to be called with BLACK = 0 and white = nonzero.
void boundary_smooth(ch_vec& image, int cx, int cy)
{
    std::set<Point> points;
    for (int i = 1; i < cy-1; i++)
    {
        for (int j = 1; j < cx-1; j++)
        {
            Point point(i, j);
            if (image[i*cx+ j] == 0)
            {
                // Mark all black points that have two or fewer black
                // neighbours and a Yokoi connectivity of less than 2.
                if (num_zero_pixel_neighbours(image, point, cx) <= 2 &&
                    yokoi_connectivity(image, point, cx) < 2)
                {
                    points.insert(point);
                }
            }
        }
    }
    // Set all the marked points to white
    for (auto& point : points)
    {
        image[point.first*cx+ point.second] = 1;
    }
}
//-------------------------------------------------------------------------
void acute_angle_emphasis(ch_vec& image, int cx, int cy)
{
    for (int k = 5; k >= 1; k -= 2)
    {
        std::set<Point> points;
        for (int i = 2; i < cy - 2; i++)
        {
            for (int j = 2; j < cx - 2; j++)
            {
                if (image[i*cx+ j] == 0)
                {
                    Point point(i, j);
                    if (match_templates(image, point, k, cx, cy))
                    {
                        image[i*cx+ j] = 2;
                        points.insert(point);
                    }
                }
            }
        }
        if (points.empty())
            break;
        for (auto& point : points)
        {
            image[point.first*cx+ point.second] = 1;
        }
        points.clear();
    }
}
//-------------------------------------------------------------------------
bool match(const ch_vec& image, const std::vector<uchar_t>& points,
           const std::vector<uchar_t>& values)
{
    bool m = true;
    for (std::vector<Point>::size_type i = 0; i < points.size(); i++)
    {
        if (values[i] == 2)
            continue;
        switch (values[i])
        {
        case 0:
            if (points[i] != 0)
                m = false;
            break;
        case 1:
            if (points[i] != 1)
                m = false;
            break;
        }
        if (!m) break;
    }
    return m;
}
//-------------------------------------------------------------------------
bool match_templates(const ch_vec& image, const Point& point, int k, int cx, int cy)
{
    int r = point.first, c = point.second;
    uchar_t data[25] = {
    image[(r - 2)*cx+ c - 2], image[(r - 2)*cx+ c - 1],
    image[(r - 2)*cx+ c], image[(r - 2)*cx+ c + 1], 
    image[(r - 2)*cx+ c + 2], image[(r - 1)*cx+ c - 2], 
    image[(r - 1)*cx+ c - 1], image[(r - 1)*cx+ c],
    image[(r - 1)*cx+ c + 1], image[(r - 1)*cx+ c + 2],
    image[r*cx+ c - 2], image[r*cx+ c - 1],
    image[r*cx+ c], image[r*cx+ c + 1],
    image[r*cx+ c + 2], image[(r + 1)*cx+ c - 2],
    image[(r + 1)*cx+ c - 1], image[(r + 1)*cx+ c],
    image[(r + 1)*cx+ c + 1], image[(r + 1)*cx+ c + 2],
    image[(r + 2)*cx+ c - 2], image[(r + 2)*cx+ c - 1], 
    image[(r + 2)*cx+ c], image[(r + 2)*cx+ c + 1], 
    image[(r + 2)*cx+ c + 2]
    };

	std::vector<uchar_t> points;
	points.insert(points.begin(), data, data+25);

    // 0 = zero
    // 1 = one 
    // 2 = don't care
    // D1
    uchar_t va_data[25] = {
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        2, 0, 0, 0, 2
    };
	std::vector<uchar_t> values;
	values.insert(values.begin(), va_data, va_data+25);

    if (match(image, points, values))
    {
        return true;
    }
    // D2
    if (k >= 2)
    {
        values[1] = 1;
        if (match(image, points, values))
        {
            return true;
        }
    }
    // D3
    if (k >= 3)
    {
        values[1] = 0;
        values[3] = 1;
        if (match(image, points, values))
        {
            return true;
        }
    }
    // D4 
    if (k >= 4)
    {
        values[3] = 0;
        values[1] = values[6] = 1;
        if (match(image, points, values))
        {
            return true;
        }
    }
    // D5
    if (k >= 5)
    {
        values[1] = values[6] = 0;
        values[3] = values[8] = 1;
        if (match(image, points, values))
        {
            return true;
        }
    }
    // U
    int va_data2[25] = {
        2, 0, 0, 0, 2,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0
    };
	values.clear();
	values.insert(values.begin(), va_data2, va_data2+25);

    // U1
    if (match(image, points, values))
    {
        return true;
    }
    // U2
    if (k >= 2)
    {
        values[21] = 1;
        if (match(image, points, values))
        {
            return true;
        }
    }
    // U3
    if (k >= 3)
    {
        values[21] = 0;
        values[23] = 1;
        if (match(image, points, values))
        {
            return true;
        }
    }
    // U4
    if (k >= 4)
    {
        values[23] = 0;
        values[16] = values[21] = 1;
        if (match(image, points, values))
        {
            return true;
        }
    }
    // U5
    if (k >= 5)
    {
        values[16] = values[21] = 0;
        values[18] = values[23] = 1;
        if (match(image, points, values))
        {
            return true;
        }

    }
    return false;
}

};	// namespace zs