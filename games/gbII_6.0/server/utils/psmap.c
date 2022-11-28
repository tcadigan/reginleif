/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * psmap.c - Produces a Postscript map from the output of the GB command 'stars'
 *
 * Copyright: Andreas Girgensohn (andreasg@cs.colorado.edu)
 */

#include <stdio.h>

struct star {
    int x;
    int y;
    char name[80];
};

struct star stars[200];
int nstars;
int highlight_home = 1;
/*
 * Star that will be highlighted
 */
int highlighted_star = -1;
/*
 * Number of distance rings around the highlighted star
 */
int nrings = 20;
int ring_spacing = 10000;

int scan_stars(FILE *);
int produce_postscript(void);
int draw_star(struct star *, int, int, double);

int main(int argc, char *argv[])
{
    scan_stars(stdin);

    if (nstars == 0) {
        fprintf(stderr, "No stars\n");
    } else {
        produce_postscript();
    }

    return 0;
}

int scan_stars(FILE *fin)
{
    char line[200];
    int distance;
    int in_stars = 0;

    nstars = 0;

    while (fgets(line, 200, fin) != NULL) {
        if (sscanf(line,
                   "(%*d) %79s ( %d, %d) %d",
                   stars[nstars].name,
                   &stars[nstars].x,
                   &stars[nstars].y,
                   &distance) == 4) {
            if (highlight_home && (distance == 0)) {
                highlighted_star = nstars;
            }

            ++nstars;

            if (!in_stars) {
                in_stars = 1;
            }
        } else if (in_stars) {
            break;
        }
    }

    return 1;
}

int produce_postscript(void)
{
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int i;
    double scale;
    double nscale;

    max_x = stars[0].x;
    min_x = max_x;
    max_y = stars[0].y;
    min_y = max_y;

    for (i = 1; i < nstars; ++i) {
        if (stars[i].x < min_x) {
            min_x = stars[i].x;
        }

        if (stars[i].x > max_x) {
            max_x = stars[i].x;
        }

        if (stars[i].y < min_y) {
            min_y = stars[i].y;
        }

        if (stars[i].y > max_y) {
            max_y = stars[i].y;
        }
    }

    /*
     * Max map size: 8.5in x 11in sheet, 0.5in borders, 0.5in on the right for
     * star names
     */
    /*
     * 72 points = 1in
     */
    scale = (7.0 * 72) / (max_x - min_x);
    nscale = (10.0 * 72) / (max_y - min_y);

    if (nscale < scale) {
        scale = nscale;
    }

    printf("%%!PS-Adobe-2.0\n\n");
    /*
     * 0,0 is in the topleft corner
     */
    printf("0.5 72 mul 10.5 72 mul translate\n");
    printf("/drawcircle\n");
    printf("{\n");
    printf("  newpath 0 360 arc stroke\n");
    printf("}\n");
    printf("def\n\n");
    printf("/drawstar\n");
    printf("{\n");
    printf("  /starname exch def\n");
    printf("  /ypos exch def\n");
    printf("  /xpos exch def\n");
    printf("  xpos ypos 2 drawcircle\n");
    printf("  4 xpos add ypos moveto\n");
    printf("  starname show\n");
    printf("}\n");
    printf("def\n\n");
    printf("0 setlinewidth\n");
    printf("newpath -10 10 moveto 7.5 72 mul 10 add 10 lineto\n");
    printf("7.5 72 mul 10 add %d lineto -10 %d lineto closepath clip\n",
           (int)((min_y - max_y) * scale) - 10,
           (int)((min_y - max_y) * scale) - 10);

    if ((highlighted_star >= 0) && (highlighted_star < nstars)) {
        printf("\n/Times-Bold findfont 9 scalefont setfont\n\n");
        draw_star(stars + highlighted_star, min_x, min_y, scale);

        for (i = 1; i <= nrings; ++i) {
            printf("%d %d %d drawcircle\n",
                   (int)((stars[highlighted_star].x - min_x) * scale),
                   (int)((min_y - stars[highlighted_star].y) * scale),
                   (int)(i * ring_spacing * scale));
        }
    }

    printf("\n/Times-Roman findfont 8 scalefont setfont\n\n");

    for (i = 0; i < nstars; ++i) {
        if (i != highlighted_star) {
            draw_star(stars + i, min_x, min_y, scale);
        }
    }

    printf("\nshowpage\n");

    return 1;
}

int draw_star(struct star *star, int min_x, int min_y, double scale)
{
    printf("%d %d (%s) drawstar\n",
           (int)((star->x - min_x) * scale),
           (int)((min_y - star_y) * scale),
           star->name);

    return 1;
}
