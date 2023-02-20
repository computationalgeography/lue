#pragma once
#include "lue/framework/core/shape.hpp"


// The Hilbert curve algorithm is inspired by the Gilbert code:
//     https://github.com/jakubcerveny/gilbert
// That code is release under the folowing license:

// BSD 2-Clause License
//
// Copyright (c) 2018, Jakub Červený
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


namespace lue {
    namespace detail {

        inline static Index offset(Index const x)
        {
            if (x < 0)
            {
                return -1;
            }
            else if (x > 0)
            {
                return 1;
            }

            return 0;
        }


        template<Rank rank>
        class HilbertCurve
        {
        };


        template<>
        class HilbertCurve<2>
        {
                using Shape = lue::Shape<Count, 2>;

            public:

                template<typename Functor>
                static void hilbert(
                    Functor& functor,
                    Index& idx,
                    Index x,
                    Index y,
                    Index const ax,
                    Index const ay,
                    Index const bx,
                    Index const by)
                {
                    Index const w = std::abs(ax + ay);
                    Index const h = std::abs(bx + by);

                    // unit major direction
                    Index const dax = offset(ax);
                    Index const day = offset(ay);

                    // unit orthogonal direction
                    Index const dbx = offset(bx);
                    Index const dby = offset(by);

                    if (h == 1)
                    {
                        // trivial row fill
                        for (Index i = 0; i < w; ++i)
                        {
                            functor(idx++, y, x);
                            x += dax;
                            y += day;
                        }

                        return;
                    }

                    if (w == 1)
                    {
                        // trivial column fill
                        for (Index i = 0; i < h; ++i)
                        {
                            functor(idx++, y, x);
                            x += dbx;
                            y += dby;
                        }

                        return;
                    }

                    Index ax2 = ax / 2;
                    Index ay2 = ay / 2;
                    Index bx2 = bx / 2;
                    Index by2 = by / 2;

                    Index const w2 = std::abs(ax2 + ay2);
                    Index const h2 = std::abs(bx2 + by2);

                    if (2 * w > 3 * h)
                    {
                        if (w2 % 2 > 0 && w > 2)
                        {
                            // prefer even steps
                            ax2 += dax;
                            ay2 += day;
                        }

                        // long case: split in two parts only
                        hilbert(functor, idx, x, y, ax2, ay2, bx, by);
                        hilbert(functor, idx, x + ax2, y + ay2, ax - ax2, ay - ay2, bx, by);
                    }
                    else
                    {

                        if (h2 % 2 > 0 && h > 2)
                        {
                            // prefer even steps
                            bx2 += dbx;
                            by2 += dby;
                        }

                        // standard case: one step up, one long horizontal, one step down
                        hilbert(functor, idx, x, y, bx2, by2, ax2, ay2);
                        hilbert(functor, idx, x + bx2, y + by2, ax, ay, bx - bx2, by - by2);
                        hilbert(
                            functor,
                            idx,
                            x + (ax - dax) + (bx2 - dbx),
                            y + (ay - day) + (by2 - dby),
                            -bx2,
                            -by2,
                            -(ax - ax2),
                            -(ay - ay2));
                    }
                }

                template<typename Functor>
                static void visit(Shape const& shape, Functor& functor)
                {
                    auto const [shape_0, shape_1] = shape;

                    // Linear index to map to 2D indices
                    Index idx{0};

                    if (shape_1 >= shape_0)
                    {
                        hilbert(functor, idx, 0, 0, shape_1, 0, 0, shape_0);
                    }
                    else
                    {
                        hilbert(functor, idx, 0, 0, 0, shape_0, shape_1, 0);
                    }
                }
        };


        // 3D Python version:

        // def gilbert3d(x, y, z,
        //               ax, ay, az,
        //               bx, by, bz,
        //               cx, cy, cz):
        //     """
        //     Generalized Hilbert ('Gilbert') space-filling curve for arbitrary-sized
        //     3D rectangular grids.
        //     """
        //
        //     w = abs(ax + ay + az)
        //     h = abs(bx + by + bz)
        //     d = abs(cx + cy + cz)
        //
        //     (dax, day, daz) = (sgn(ax), sgn(ay), sgn(az)) # unit major direction ("right")
        //     (dbx, dby, dbz) = (sgn(bx), sgn(by), sgn(bz)) # unit ortho direction ("forward")
        //     (dcx, dcy, dcz) = (sgn(cx), sgn(cy), sgn(cz)) # unit ortho direction ("up")
        //
        //     # trivial row/column fills
        //     if h == 1 and d == 1:
        //         for i in range(0, w):
        //             print x, y, z
        //             (x, y, z) = (x + dax, y + day, z + daz)
        //         return
        //
        //     if w == 1 and d == 1:
        //         for i in range(0, h):
        //             print x, y, z
        //             (x, y, z) = (x + dbx, y + dby, z + dbz)
        //         return
        //
        //     if w == 1 and h == 1:
        //         for i in range(0, d):
        //             print x, y, z
        //             (x, y, z) = (x + dcx, y + dcy, z + dcz)
        //         return
        //
        //     (ax2, ay2, az2) = (ax/2, ay/2, az/2)
        //     (bx2, by2, bz2) = (bx/2, by/2, bz/2)
        //     (cx2, cy2, cz2) = (cx/2, cy/2, cz/2)
        //
        //     w2 = abs(ax2 + ay2 + az2)
        //     h2 = abs(bx2 + by2 + bz2)
        //     d2 = abs(cx2 + cy2 + cz2)
        //
        //     # prefer even steps
        //     if (w2 % 2) and (w > 2):
        //        (ax2, ay2, az2) = (ax2 + dax, ay2 + day, az2 + daz)
        //
        //     if (h2 % 2) and (h > 2):
        //        (bx2, by2, bz2) = (bx2 + dbx, by2 + dby, bz2 + dbz)
        //
        //     if (d2 % 2) and (d > 2):
        //        (cx2, cy2, cz2) = (cx2 + dcx, cy2 + dcy, cz2 + dcz)
        //
        //     # wide case, split in w only
        //     if (2*w > 3*h) and (2*w > 3*d):
        //        gilbert3d(x, y, z,
        //                  ax2, ay2, az2,
        //                  bx, by, bz,
        //                  cx, cy, cz)
        //
        //        gilbert3d(x+ax2, y+ay2, z+az2,
        //                  ax-ax2, ay-ay2, az-az2,
        //                  bx, by, bz,
        //                  cx, cy, cz)
        //
        //     # do not split in d
        //     elif 3*h > 4*d:
        //        gilbert3d(x, y, z,
        //                  bx2, by2, bz2,
        //                  cx, cy, cz,
        //                  ax2, ay2, az2)
        //
        //        gilbert3d(x+bx2, y+by2, z+bz2,
        //                  ax, ay, az,
        //                  bx-bx2, by-by2, bz-bz2,
        //                  cx, cy, cz)
        //
        //        gilbert3d(x+(ax-dax)+(bx2-dbx),
        //                  y+(ay-day)+(by2-dby),
        //                  z+(az-daz)+(bz2-dbz),
        //                  -bx2, -by2, -bz2,
        //                  cx, cy, cz,
        //                  -(ax-ax2), -(ay-ay2), -(az-az2))
        //
        //     # do not split in h
        //     elif 3*d > 4*h:
        //        gilbert3d(x, y, z,
        //                  cx2, cy2, cz2,
        //                  ax2, ay2, az2,
        //                  bx, by, bz)
        //
        //        gilbert3d(x+cx2, y+cy2, z+cz2,
        //                  ax, ay, az,
        //                  bx, by, bz,
        //                  cx-cx2, cy-cy2, cz-cz2)
        //
        //        gilbert3d(x+(ax-dax)+(cx2-dcx),
        //                  y+(ay-day)+(cy2-dcy),
        //                  z+(az-daz)+(cz2-dcz),
        //                  -cx2, -cy2, -cz2,
        //                  -(ax-ax2), -(ay-ay2), -(az-az2),
        //                  bx, by, bz)
        //
        //     # regular case, split in all w/h/d
        //     else:
        //        gilbert3d(x, y, z,
        //                  bx2, by2, bz2,
        //                  cx2, cy2, cz2,
        //                  ax2, ay2, az2)
        //
        //        gilbert3d(x+bx2, y+by2, z+bz2,
        //                  cx, cy, cz,
        //                  ax2, ay2, az2,
        //                  bx-bx2, by-by2, bz-bz2)
        //
        //        gilbert3d(x+(bx2-dbx)+(cx-dcx),
        //                  y+(by2-dby)+(cy-dcy),
        //                  z+(bz2-dbz)+(cz-dcz),
        //                  ax, ay, az,
        //                  -bx2, -by2, -bz2,
        //                  -(cx-cx2), -(cy-cy2), -(cz-cz2))
        //
        //        gilbert3d(x+(ax-dax)+bx2+(cx-dcx),
        //                  y+(ay-day)+by2+(cy-dcy),
        //                  z+(az-daz)+bz2+(cz-dcz),
        //                  -cx, -cy, -cz,
        //                  -(ax-ax2), -(ay-ay2), -(az-az2),
        //                  bx-bx2, by-by2, bz-bz2)
        //
        //        gilbert3d(x+(ax-dax)+(bx2-dbx),
        //                  y+(ay-day)+(by2-dby),
        //                  z+(az-daz)+(bz2-dbz),
        //                  -bx2, -by2, -bz2,
        //                  cx2, cy2, cz2,
        //                  -(ax-ax2), -(ay-ay2), -(az-az2))
        //
        //
        // def main():
        //     width = int(sys.argv[1])
        //     height = int(sys.argv[2])
        //     depth = int(sys.argv[3])
        //
        //     if width >= height and width >= depth:
        //        gilbert3d(0, 0, 0,
        //                  width, 0, 0,
        //                  0, height, 0,
        //                  0, 0, depth)
        //
        //     elif height >= width and height >= depth:
        //        gilbert3d(0, 0, 0,
        //                  0, height, 0,
        //                  width, 0, 0,
        //                  0, 0, depth)
        //
        //     else: # depth >= width and depth >= height
        //        gilbert3d(0, 0, 0,
        //                  0, 0, depth,
        //                  width, 0, 0,
        //                  0, height, 0)

    }  // namespace detail


    template<Rank rank, typename Functor>
    void hilbert_curve(Shape<Count, rank> const& shape, Functor functor)
    {
        if (nr_elements(shape) > 0)
        {
            // Walk the cells in the array shape and call functor for every
            // cell visited, passing the linear index and the cell indices
            detail::HilbertCurve<rank>::visit(shape, functor);
        }
    }

}  // namespace lue
