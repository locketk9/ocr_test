/// @file   seg_project.h
/// @author locketk9@gmail.com
/// @date   2021-04-28

#pragma once

#include "ocr.h"

/// @brief segmenattion by projection
/// 일단 1라인만 가능하다.
std::vector<imgRECT> seg_projection(const ch_vec &bin, int cx, int cy, int_vec &hp, int_vec &vp) {
    hp.resize(cx, 0);
    vp.resize(cy, 0);
    
    // calc horizontal projection & vertical projection
    for (int y=0; y!=cy; ++y)   {
        for (int x=0; x!=cx; ++x)   {
            if (bin[cx*y + x] != 0) {
                ++hp[x];
                ++vp[y];
            } 
        }
    }

    // calc roi
    std::vector<imgRECT> segs;
    int idx = 0;
    // horizon
    int gap = 0;
    bool s = false, e = false;
    for (int h=0; h!=hp.size(); ++h)    {
        if (hp[h] != 0 && s == false) {
            s = true;
            gap = h;
        } else if (hp[h] == 0 && s == true) {
            e = true;
        }
        if (s && e) {
            imgRECT seg(gap, 0, h-gap, 0);
            segs.push_back(seg);

            // vertical
            gap = 0;
            s = e = false;
            for (int v = 0; v != vp.size(); ++v) {
                if (vp[v] != 0 && s == false) {
                    s = true;
                    gap = v;
                }
                else if (vp[v] == 0 && s == true) {
                    e = true;
                }
                if (s && e) {
                    segs[idx].y = gap; segs[idx].cy = v - gap;
                    e = s = false;
                    ++idx;
                }
            }

            e = s = false;
        }
    }

    return segs;
}