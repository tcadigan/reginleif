/*
 * Copyright (C) 1995-2002 FSGames. Ported by Sean Ford and Yan Shosh
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "screen.hpp"

#include "input.hpp"
// #include "os_depend.hpp"
#include "util.hpp"

// Private variables for SAI2x
static Uint32 colorMask = 0xF7DEF7DE;
static Uint32 lowPixelMask = 0x08210821;
static Uint32 qcolorMask = 0xE79CE79C;
static Uint32 qlowpixelMask = 0x18631863;
static Sint32 xsai_depth = 0;
static Uint8 *src_line[4];
static Uint8 *dst_line[2];

#define GET_RESULT(A, B, C, D) ((((A) != (C)) || ((A) != (D))) - (((B) != (C)) || ((B) != (D))))
#define INTERPOLATE(A, B) (((((A) & colorMask) >> 1) + (((B) & colorMask) >> 1)) + (((A) & (B)) & lowPixelMask))
#define Q_INTERPOLATE(A, B, C, D)               \
    (((((((A) & qcolorMask) >> 2)               \
        + (((B) & qcolorMask) >> 2))            \
       + (((C) & qcolorMask) >> 2))             \
      + (((D) & qcolorMask) >> 2))              \
     + ((((((((A) & qlowpixelMask)              \
             + ((B) & qlowpixelMask))           \
            + ((C) & qlowpixelMask))            \
           + ((D) & qlowpixelMask)))            \
         >> 2) & qlowpixelMask))

// Works only for bpp 32!
Sint32 Init_2xSaI()
{
    /*
     * Color Mask:      0xFEFEFE
     * Low Pixel Mask:  0x10101
     * QColor Mask:     0xFCFCFC
     * QLow Pixel Mask: 0x30303
     */

    colorMask = 0xFEFEFE;
    lowPixelMask = 0x10101;
    qcolorMask = 0xFCFCFC;
    qlowpixelMask = 0x30303;
    xsai_depth = 32;

    return 0;
}

void Super2xSaI_ex2(Uint8 *src, Sint32 srcx, Sint32 srcy, Sint32 srcw, Sint32 srch,
                    Sint32 src_pitch, Sint32 src_height, Uint8 *dst, Sint32 dstx,
                    Sint32 dsty, Sint32 dst_pitch)
{
    Uint8 *srcPtr = src + ((4 * srcx) + (srcy * src_pitch));
    Uint8 *dstPtr = dst + ((4 * dstx) + (dsty * dst_pitch));

    src_pitch = src_pitch / 4;
    dst_pitch = dst_pitch / 4;

    if ((srcx + srcw) >= src_pitch) {
        srcw = src_pitch - srcx;
    }

    Sint32 ybeforelast1 = (src_height - 1) - srcy;
    Sint32 ybeforelast2 = (src_height - 2) - srcy;
    Sint32 xbeforelast1 = (src_pitch - 1) - srcx;
    Sint32 xbeforelast2 = (src_pitch - 2) - srcx;

    for (Sint32 y = 0; y < srch; ++y) {
        Uint32 *bP = reinterpret_cast<Uint32 *>(srcPtr);
        Uint32 *dP = reinterpret_cast<Uint32 *>(dstPtr);

        for (Sint32 x = 0; x < srcw; ++x) {
            Uint32 color4;
            Uint32 color5;
            Uint32 color6;
            Uint32 color1;
            Uint32 color2;
            Uint32 color3;
            Uint32 colorA0;
            Uint32 colorA1;
            Uint32 colorA2;
            Uint32 colorA3;
            Uint32 colorB0;
            Uint32 colorB1;
            Uint32 colorB2;
            Uint32 colorB3;
            Uint32 colorS1;
            Uint32 colorS2;
            Uint32 product1a;
            Uint32 product1b;
            Uint32 product2a;
            Uint32 product2b;
            Sint32 add1;
            Sint32 add2;
            Sint32 sub1;
            Sint32 nextl1;
            Sint32 nextl2;
            Sint32 prevl1;

            if (x == 0) {
                sub1 = 0;
            } else {
                sub1 = 0;
            }

            if (x >= xbeforelast2) {
                add2 = 0;
            } else {
                add2 = 1;
            }

            if (x >= xbeforelast1) {
                add1 = 0;
            } else {
                add1 = 1;
            }

            if (y == 0) {
                prevl1 = 0;
            } else {
                prevl1 = src_pitch;
            }

            if (y >= ybeforelast2) {
                nextl2 = 0;
            } else {
                nextl2 = src_pitch;
            }

            if (y >= ybeforelast1) {
                nextl1 = 0;
            } else {
                nextl1 = src_pitch;
            }

            colorB0 = *(bP - prevl1 - sub1);
            colorB1 = *(bP - prevl1);
            colorB2 = *(bP - prevl1 + add1);
            colorB3 = *(bP - prevl1 + add1 + add2);

            color4 = *(bP - sub1);
            color5 = *(bP);
            color6 = *(bP + add1);
            colorS2 = *(bP + add1 + add2);

            color1 = *(bP + nextl1 - sub1);
            color2 = *(bP + nextl1);
            color3 = *(bP + nextl1 + add1);
            colorS1 = *(bP + nextl1 + add1 + add2);

            colorA0 = *(bP + nextl1 + nextl2 - sub1);
            colorA1 = *(bP + nextl1 + nextl2);
            colorA2 = *(bP + nextl1 + nextl2 + add1);
            colorA3 = *(bP + nextl1 + nextl2 + add1 + add2);

            /*
             * B0 B1 B2 B3     0  1  2  3
             *  4 5*  6 S2 ->  4 5*  6  7
             *  1  2  3 S1     8  9 10 11
             * A0 A1 A2 A3    12 13 14 15
             */

            if ((color2 == color6) && (color5 != color3)) {
                product1b = color2;
                product2b = product1b;
            } else if ((color5 == color3) && (color2 != color6)) {
                product1b = color5;
                product2b = product1b;
            } else if ((color5 == color3) && (color2 == color6)) {
                Sint32 r = 0;
                r += GET_RESULT(color6, color5, color1, colorA1);
                r += GET_RESULT(color6, color5, color4, colorB1);
                r += GET_RESULT(color6, color5, colorA2, colorS1);
                r += GET_RESULT(color6, color5, colorB2, colorS2);

                if (r > 0) {
                    product1b = color6;
                    product2b = product1b;
                } else if (r < 0) {
                    product1b = color5;
                    product2b = product1b;
                } else {
                    product1b = INTERPOLATE(color5, color6);
                    product2b = product1b;
                }
            } else {
                if ((color6 == color3) && (color3 == colorA1) && (color2 != colorA2) && (color3 != colorA0)) {
                    product2b = Q_INTERPOLATE(color3, color3, color3, color2);
                } else if ((color5 == color2) && (color2 == colorA2) && (colorA1 != color3) && (color2 != colorA3)) {
                    product2b = Q_INTERPOLATE(color2, color2, color2, color3);
                } else {
                    product2b = INTERPOLATE(color2, color3);
                }

                if ((color6 == color3) && (color6 == colorB1) && (color5 != colorB2) && (color6!= colorB0)) {
                    product1b = Q_INTERPOLATE(color6, color6, color6, color5);
                } else if ((color5 == color2) && (color5 == colorB2) && (colorB1 != color6) && (color5 != colorB3)) {
                    product1b = Q_INTERPOLATE(color6, color5, color5, color5);
                } else {
                    product1b = INTERPOLATE(color5, color6);
                }
            }

            if ((color5 == color3) && (color2 != color6) && (color4 == color5) && (color5 != colorA2)) {
                product2a = INTERPOLATE(color2, color5);
            } else if ((color5 == color1) && (color6 == color5) && (color4 != color2) && (color5 != colorA0)) {
                product2a = INTERPOLATE(color2, color5);
            } else {
                product2a = color2;
            }

            if ((color2 == color6) && (color5 != color3) && (color1 == color2) && (color2 != colorB2)) {
                product1a = INTERPOLATE(color2, color5);
            } else if ((color4 == color2) && (color3 == color2) && (color1 != color5) && (color2 != colorB0)) {
                product1a = INTERPOLATE(color2, color5);
            } else {
                product1a = color5;
            }

            *dP = product1a;
            *(dP + 1) = product1b;
            *(dP + dst_pitch) = product2a;
            *(dP + dst_pitch + 1) = product2b;

            bP += 1;
            dP += 2;
        }

        srcPtr += (src_pitch * 4);
        dstPtr += ((2 * dst_pitch) * 4);
    }
}

void Scale_SuperEagle(Uint8 *src, Sint32 srcx, Sint32 srcy, Sint32 srcw, Sint32 srch,
                      Sint32 src_pitch, Sint32 src_height, Uint8 *dst, Sint32 dstx,
                      Sint32 dsty, Sint32 dst_pitch)
{
    /*
     * We need to ensure that the update is aligned to 4 pixels - Colourless
     * The idea was to prevent artifacts from appearing, but it doesn't seem
     * to help
     *
     * Sint32 sx = ((srcx - 4) / 4) * 4;
     * Sint32 ex = (((srcx + srcw) + 7) / 4) * 4;
     * Sint32 sy = ((srcy - 4) / 4) * 4;
     * Sint32 ey = (((srcy + srch) + 7) / 4) * 4;
     *
     * if (sx < 0) {
     *     sx = 0;
     * }
     *
     * if (sy < 0) {
     *     sy = 0;
     * }
     *
     * if (ex > sline_pixels) {
     *     ex = sline_pixels;
     * }
     *
     * if (ey > sheight) {
     *     ey = sheight;
     * }
     *
     * srcx = sx;
     * srcy = sy;
     * srcw = ex - sx;
     * srch = ey - sy;
     */

    Uint8 *srcPtr = src + ((4 * srcx) + (srcy * src_pitch));
    Uint8 *dstPtr = dst + ((4 * dstx) + (dsty * dst_pitch));

    src_pitch = src_pitch / 4;
    dst_pitch = dst_pitch / 4;

    if ((srcx + srcw) >= src_pitch) {
        srcw = src_pitch - srcx;
    }

    Sint32 ybeforelast1 = (src_height - 1) - srcy;
    Sint32 ybeforelast2 = (src_height - 2) - srcy;
    Sint32 xbeforelast1 = (src_pitch - 1) - srcx;
    Sint32 xbeforelast2 = (src_pitch - 2) - srcx;

    for (Sint32 y = 0; y < srch; ++y) {
        Uint32 *bP = reinterpret_cast<Uint32 *>(srcPtr);
        Uint32 *dP = reinterpret_cast<Uint32 *>(dstPtr);

        for (Sint32 x = 0; x < srcw; ++x) {
            Uint32 color4;
            Uint32 color5;
            Uint32 color6;
            Uint32 color1;
            Uint32 color2;
            Uint32 color3;
            // Uint32 colorA0;
            // Uint32 colorA3;
            // Uint32 colorB0;
            // Uint32 colorB3;
            Uint32 colorA1;
            Uint32 colorA2;
            Uint32 colorB1;
            Uint32 colorB2;
            Uint32 colorS1;
            Uint32 colorS2;
            Uint32 product1a;
            Uint32 product1b;
            Uint32 product2a;
            Uint32 product2b;

            /*
             * B0 B1 B2 B3
             *  4  5  6 S2
             *  1  2  3 S1
             * A0 A1 A2 A3
             */
            Sint32 add1;
            Sint32 add2;
            Sint32 sub1;
            Sint32 nextl1;
            Sint32 nextl2;
            Sint32 prevl1;

            if (x == 0) {
                sub1 = 0;
            } else {
                sub1 = 1;
            }

            if (x >= xbeforelast2) {
                add2 = 0;
            } else {
                add2 = 1;
            }

            if (x >= xbeforelast1) {
                add1 = 0;
            } else {
                add1 = 1;
            }

            if (y == 0) {
                prevl1 = 0;
            } else {
                prevl1 = src_pitch;
            }

            if (y >= ybeforelast2) {
                nextl2 = 0;
            } else {
                nextl2 = src_pitch;
            }

            if (y >= ybeforelast1) {
                nextl1 = 0;
            } else {
                nextl1 = src_pitch;
            }

            // colorB0 = *(bP - prevl1 - sub1);
            colorB1 = *(bP - prevl1);
            colorB2 = *(bP - prevl1 + add1);
            // colorB3 = *(bP - prevl1 + add1 + add2);

            color4 = *(bP - sub1);
            color5 = *(bP);
            color6 = *(bP + add1);
            colorS2 = *(bP + add1 + add2);

            color1 = *(bP + nextl1 - sub1);
            color2 = *(bP + nextl1);
            color3 = *(bP + nextl1 + add1);
            colorS1 = *(bP + nextl1 + add1 + add2);

            // colorA0 = *(bP + nextl1 + nextl2 - sub1);
            colorA1 = *(bP + nextl1 + nextl2);
            colorA2 = *(bP + nextl1 + nextl2 + add1);
            // colorA3 = *(bP + nextl1 + nextl2 + add1 + add2);

            if ((color2 == color6) && (color5 != color3)) {
                product2a = color2;
                product1b = product2a;
                // manip.copy(product2a, color2);
                // prodcut1b = prodcut2a;

                if ((color1 == color2) || (color6 == colorB2)) {
                    product1a = INTERPOLATE(color2, color5);
                    product1a = INTERPOLATE(color2, product1a);
                    // product1a = QInterpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color2, color2, color2, color5, manip);
                } else {
                    product1a = INTERPOLATE(color5, color6);
                    // product1a = Interpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color6, color5, manip);
                }

                if ((color6 == colorS2) || (color2 == colorA1)) {
                    product2b = INTERPOLATE(color2, color3);
                    product2b = INTERPOLATE(color2, product2b);
                    // product2b = QInterpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color2, color2, color2, color3, manip);
                } else {
                    product2b = INTERPOLATE(color2, color3);
                    // product2b = Interpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color2, color3, manip);
                }
            } else if ((color5 == color3) && (color2 != color6)) {
                product1a = color5;
                product2b = product1a;
                // manip.copy(product1a, color5);
                // product2b = product1a;

                if ((colorB1 == color5) || (color3 == colorS1)) {
                    product1b = INTERPOLATE(color5, color6);
                    product1b = INTERPOLATE(color5, product1b);
                    // product1b = QInterpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color5, color5, color5, color6, manip);
                } else {
                    product1b = INTERPOLATE(color5, color6);
                    // product1b = Interpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color5, color6, manip);
                }

                if ((color3 == colorA2) || (color4 == color5)) {
                    product2a = INTERPOLATE(color5, color2);
                    product2a = INTERPOLATE(color5, product2a);
                    // product2a = QInterpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color2, color5, color5, color5, manip);
                } else {
                    product2a = INTERPOLATE(color2, color3);
                    // product2a = Interpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color3, color2, manip);
                }
            } else if ((color5 == color3) && (color2 == color6)) {
                Sint32 r = 0;

                r += GET_RESULT(color6, color5, color1, colorA1);
                r += GET_RESULT(color6, color5, color4, colorB1);
                r += GET_RESULT(color6, color5, colorA2, colorS1);
                r += GET_RESULT(color6, color5, colorB2, colorS2);
                // r += GetResult1<Source_pixel>(color5, color6, color4, colorB1);
                // r += GetResult2<Source_pixel>(color6, color5, colorA2, colorS1);
                // r += GetResult2<Source_pixel>(color6, color5, color1, colorA1);
                // r += GetResult1<Source_pixel>(color5, color6, colorB2, colorS2);

                if (r > 0) {
                    product2a = color2;
                    product1b = product2a;
                    // manip.copy(product2a, color2);
                    // product1b = product2a;
                    product2b = INTERPOLATE(color5, color6);
                    product1a = product2b;
                    // product2b = Interpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color5, color6, manip);
                    // product1a = product2b;
                } else if (r < 0) {
                    product1a = color5;
                    product2b = product1a;
                    // manip.copy(product1a, color5);
                    // product2b = product1a;
                    product2a = INTERPOLATE(color5, color6);
                    product1b = product2a;
                    // product2a = Interpolate_2xSaI<Source_pixel, Dest_Pixel, Mainp_pixels>(color5, color6, manip);
                    // product1b = product2a;
                } else {
                    product1a = color5;
                    product2b = product1a;
                    // manip.copy(product1a, color5);
                    // product2b = product1a;
                    product2a = color2;
                    product1b = product2a;
                    // manip.copy(product2a, color2);
                    // product1b = product2a;
                }
            } else {
                product1a = INTERPOLATE(color2, color6);
                product2b = product1a;
                product2b = Q_INTERPOLATE(color3, color3, color3, product2b);
                product1a = Q_INTERPOLATE(color5, color5, color5, product1a);
                // product2b = OInterpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color3, color2, color6, manip);
                // product1a = OInterpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color5, color6, color2, manip);

                product1b = INTERPOLATE(color5, color3);
                product2a = product1b;
                product2a = Q_INTERPOLATE(color2, color2, color2, product2a);
                product1b = Q_INTERPOLATE(color6, color6, color6, product1b);
                // product2a = OInterpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color2, color5, color3, manip);
                // product1b = OInterpolate_2xSaI<Source_pixel, Dest_pixel, Manip_pixels>(color6, color5, color3, manip);
            }

            *dP = product1a;
            *(dP + 1) = product1b;
            *(dP + dst_pitch) = product2a;
            *(dP + dst_pitch + 1) = product2b;

            bP += 1;
            dP += 2;
        }

        srcPtr += (src_pitch * 4);
        dstPtr += ((2 * dst_pitch) * 4);
    }
}

void Super2xSaI_ex(Uint8 *src, Uint32 src_pitch, Uint8 *unused, Uint8 *dest, Uint32 dest_pitch,
                   Uint32 width, Uint32 height)
{
    // For avoid warning
    unused = nullptr;
    // Sint32 j;
    Uint32 x;
    Uint32 y;
    Uint32 color[16];

    // Point to the first 3 lines.
    src_line[0] = src;
    src_line[1] = src;
    src_line[2] = src + src_pitch;
    src_line[3] = src + (src_pitch * 2);

    dst_line[0] = dest;
    dst_line[1] = dest + dest_pitch;

    x = 0;
    y = 0;
    Uint32 *lbp;
    lbp = reinterpret_cast<Uint32 *>(src_line[0]);
    color[0] = *lbp;
    color[1] = color[0];
    color[2] = color[0];
    color[3] = color[0];
    color[4] = color[0];
    color[5] = color[0];
    color[6] = *(lbp + 1);
    color[7] = *(lbp + 2);

    lbp = reinterpret_cast<Uint32 *>(src_line[2]);
    color[8] = *lbp;
    color[9] = color[8];
    color[10] = *(lbp + 1);
    color[11] = *(lbp + 2);

    lbp = reinterpret_cast<Uint32 *>(src_line[3]);
    color[12] = *lbp;
    color[13] = color[12];
    color[14] = *(lbp + 1);
    color[15] = *(lbp + 2);

    for (y = 0; y < height; ++y) {
        /*
         * Todo:
         * x = width - 2;
         * x = width - 1;
         */
        for (x = 0; x < width; ++x) {
            Uint32 product1a;
            Uint32 product1b;
            Uint32 product2a;
            Uint32 product2b;

            /*
             * B0 B1 B2 B3     0  1  2  3
             *  4 5*  6 S2 ->  4 5*  6  7
             *  1  2  3 S1     8  9 10 11
             * A0 A1 A2 A3    12 13 14 15
             */

            if ((color[9] == color[6]) && (color[5] != color[10])) {
                product2b = color[9];
                product1b = product2b;
            } else if ((color[5] == color[10]) && (color[9] != color[6])) {
                product2b = color[5];
                product1b = product2b;
            } else if ((color[5] == color[10]) && (color[9] == color[6])) {
                Sint32 r = 0;

                r += GET_RESULT(color[6], color[5], color[8], color[13]);
                r += GET_RESULT(color[6], color[5], color[4], color[1]);
                r += GET_RESULT(color[6], color[5], color[14], color[11]);
                r += GET_RESULT(color[6], color[5], color[2], color[7]);

                if (r > 0) {
                    product1b = color[6];
                } else if (r < 0) {
                    product1b = color[5];
                } else {
                    product1b = INTERPOLATE(color[5], color[6]);
                }

                product2b = product1b;
            } else {
                if ((color[6] == color[10]) && (color[10] == color[13]) && (color[9] != color[14]) && (color[10] != color[12])) {
                    product2b = Q_INTERPOLATE(color[10], color[10], color[10], color[9]);
                } else if ((color[5] == color[9]) && (color[9] == color[14]) && (color[13] != color[10]) && (color[9] != color[15])) {
                    product2b = Q_INTERPOLATE(color[9], color[9], color[9], color[10]);
                } else {
                    product2b = INTERPOLATE(color[9], color[10]);
                }

                if ((color[6] == color[10]) && (color[6] == color[1]) && (color[5] != color[2]) && (color[6] != color[0])) {
                    product1b = Q_INTERPOLATE(color[6], color[6], color[6], color[5]);
                } else if ((color[5] == color[9]) && (color[5] == color[2]) && (color[1] != color[6]) && (color[5] != color[3])) {

                    product1b = Q_INTERPOLATE(color[6], color[5], color[5], color[5]);
                } else {
                    product1b = INTERPOLATE(color[5], color[6]);
                }
            }

            if ((color[5] == color[10]) && (color[9] != color[6]) && (color[4] == color[5]) && (color[5] != color[14])) {
                product2a = INTERPOLATE(color[9], color[5]);
            } else if ((color[5] == color[8]) && (color[6] == color[5]) && (color[4] != color[9]) && (color[5] != color[12])) {
                product2a = INTERPOLATE(color[9], color[5]);
            } else {
                product2a = color[9];
            }

            if ((color[9] == color[6]) && (color[5] != color[10]) && (color[8] == color[9]) && (color[9] != color[2])) {
                product1a = INTERPOLATE(color[9], color[5]);
            } else if ((color[4] == color[9]) && (color[10] == color[9]) && (color[8] != color[5]) && (color[9] != color[0])) {
                product1a = INTERPOLATE(color[9], color[5]);
            } else {
                product1a = color[5];
            }

            *(reinterpret_cast<Uint32 *>(&dst_line[0][x * 8])) = product1a;
            *(reinterpret_cast<Uint32 *>(&dst_line[0][(x * 8) + 4])) = product1b;
            *(reinterpret_cast<Uint32 *>(&dst_line[1][x * 8])) = product2a;
            *(reinterpret_cast<Uint32 *>(&dst_line[1][(x * 8) + 4])) = product2b;

            // Move color matrix forward
            color[0] = color[1];
            color[4] = color[5];
            color[8] = color[9];
            color[12] = color[13];
            color[1] = color[2];
            color[5] = color[6];
            color[9] = color[10];
            color[13] = color[14];
            color[2] = color[3];
            color[6] = color[7];
            color[10] = color[11];
            color[14] = color[15];

            if (x < (width - 3)) {
                x += 3;
                color[3] = *((reinterpret_cast<Uint32 *>(src_line[0])) + x);
                color[7] = *((reinterpret_cast<Uint32 *>(src_line[1])) + x);
                color[11] = *((reinterpret_cast<Uint32 *>(src_line[2])) + x);
                color[15] = *((reinterpret_cast<Uint32 *>(src_line[3])) + x);
                x -= 3;
            }
        }

        // We're done with one line, so we shift the source lines up
        src_line[0] = src_line[1];
        src_line[1] = src_line[2];
        src_line[2] = src_line[3];

        // Read next line
        if ((y + 3) >= height) {
            src_line[3] = src_line[2];
        } else {
            src_line[3] = src_line[2] + src_pitch;
        }

        // Then shift the color matrix up
        Uint32 *lbp;
        lbp = reinterpret_cast<Uint32 *>(src_line[0]);
        color[0] = *lbp;
        color[1] = color[0];
        color[2] = *(lbp + 1);
        color[3] = *(lbp + 2);

        lbp = reinterpret_cast<Uint32 *>(src_line[1]);
        color[4] = *lbp;
        color[5] = color[4];
        color[6] = *(lbp + 1);
        color[7] = *(lbp + 2);

        lbp = reinterpret_cast<Uint32 *>(src_line[2]);
        color[8] = *lbp;
        color[9] = color[9];
        color[10] = *(lbp + 1);
        color[11] = *(lbp + 2);

        lbp = reinterpret_cast<Uint32 *>(src_line[3]);
        color[12] = *lbp;
        color[13] = color[12];
        color[14] = *(lbp + 1);
        color[15] = *(lbp + 2);

        if (y < (height - 1)) {
            dst_line[0] = dest + (dest_pitch * ((y * 2) + 2));
            dst_line[1] = dest + (dest_pitch * ((y * 2) + 3));
        }
    }
}

void Super2xSaI(SDL_Surface *src, SDL_Surface *dest, Sint32 s_x, Sint32 s_y, Sint32 d_x,
                Sint32 d_y, Sint32 w, Sint32 h)
{
    Sint32 sbpp;
    Sint32 dbpp;

    if (!src || !dest) {
        Log("Error: In Super2xSaI, src or dest are NULL\n");

        return;
    }

    sbpp = src->format->BitsPerPixel;
    dbpp = dest->format->BitsPerPixel;

    // Must be same color depth
    if ((sbpp != xsai_depth) || (sbpp != dbpp)) {
        Log("Error: In Super2xSaI, sbpp or dbpp are not equal to xsai_depth\n");

        return;
    }

    sbpp = src->format->BytesPerPixel;
    dbpp = dest->format->BytesPerPixel;

    if ((w < 4) || (h < 4)) {
        // Image is too small to 2xSaI'ed
        Log("Error: Surface to copy is to small, TODO here\n");

        return;
    }

    Super2xSaI_ex((static_cast<Uint8 *>(src->pixels) + (src->pitch * s_y)) + (s_x * sbpp),
                  src->pitch,
                  NULL,
                  (static_cast<Uint8 *>(dest->pixels) + (dest->pitch * d_y)) + (d_x * dbpp),
                  dest->pitch,
                  w,
                  h);

    return;
}

Screen::Screen(RenderEngine engine, Sint32 width, Sint32 height, Sint32 fullscreen)
{
    Engine = engine;

    if ((Engine == SAI) || (Engine == EAGLE)) {
        Init_2xSaI();
    }

    Sint32 w;
    Sint32 h;

    w = width;
    h = height;

    if (fullscreen) {
        window = SDL_CreateWindow("Gladiator",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  0, 0,
                                  SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        window = SDL_CreateWindow("Gladiator",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  w, h,
                                  SDL_WINDOW_INPUT_GRABBED);

    }


    if (window == nullptr) {
        exit(1);
    }

    SDL_GetWindowSize(window, &w, &h);
    window_w = w;
    window_h = h;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, width, height);

    render_tex = SDL_CreateTexture(renderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   width, height);
}

Screen::~Screen()
{
    SDL_DestroyTexture(render_tex);
    SDL_DestroyTexture(render2_tex);
    SDL_FreeSurface(render);
    SDL_FreeSurface(render2);

    SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
}

void Screen::SaveBMP(SDL_Surface *screen, std::string const &filename)
{
    SDL_SaveBMP(screen, filename.c_str());
}

void Screen::clear()
{
    SDL_FillRect(render, NULL, 0x000000);
}

void Screen::clear(Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
    SDL_Rect r = { x, y, w, h };
    SDL_FillRect(render, &r, 0x000000);
}

void Screen::swap(Sint32 x, Sint32 y, Sint32 w, Sint32 h)
{
    if ((Engine == SAI) || (Engine == EAGLE)) {
        if (render2 == nullptr) {
            render2 = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 400, 32, 0, 0, 0, 0);
            render2_tex = SDL_CreateTexture(renderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            640, 400);
        }

        SDL_LockSurface(render2);

        if (Engine == SAI) {
            Super2xSaI_ex2(static_cast<Uint8 *>(render->pixels), x, y, w, h, render->pitch, render->h,
                           static_cast<Uint8 *>(render2->pixels), 2 * x, 2 * y, render2->pitch);
        } else {
            Scale_SuperEagle(static_cast<Uint8 *>(render->pixels), x, y, w, h, render->pitch,
                             render->h, static_cast<Uint8 *>(render2->pixels), 2 * x, 2 * y,
                             render2->pitch);
        }

        SDL_UnlockSurface(render2);
        render = render2;
        render_tex = render2_tex;
    }

    SDL_UpdateTexture(render_tex, NULL, render->pixels, render->pitch);
    SDL_Rect dest = {
        viewport_offset_x,
        viewport_offset_y,
        viewport_w,
        viewport_h
    };

    SDL_RenderCopy(renderer, render_tex, NULL, &dest);
    SDL_RenderPresent(renderer);
}

void Screen::clear_window()
{
    SDL_Surface *source_surface = render;
    SDL_Texture *dest_texture = render_tex;

    SDL_FillRect(source_surface, NULL, 0x000000);
    SDL_UpdateTexture(dest_texture, NULL, source_surface->pixels, source_surface->pitch);
    SDL_Rect dest = { 0, 0, window_w, window_h };

    SDL_RenderCopy(renderer, dest_texture, NULL, &dest);
}
