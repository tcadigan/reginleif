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
 * makeindex.c
 *
 * Galactic Bloodshed File Indexer
 *
 * This program takes an ascii file with a .txt extension, looks for index
 * markers, then creates an accompanying .idx file. This speeds up lookups in an
 * information file where it can jump directly to a marker in the file.
 *
 * The index marker is of the form: & string
 * Where it's at the beginning of the line and on it's own line, 'string' may be
 * any keyword used in the lookup.
 *
 * So to use this, for example, you'd created a file named foo.txt with the
 * following data:
 *
 * & foo
 * This file is an example on hot to use the GB indexer, it has two items as
 * an example: 'easy' and 'cake'
 * & easy
 * This is easy
 * & cake
 * as cake!
 *
 * Then after you've created this file run: makeindex foo
 * This will take the foo.txt file and create foo.idx
 * Later, if you make any changes to the txt file you'll need to re-run
 * makeindex to recreate the position index.
 *
 * Within the GB server you can access these files with the do_idx function.
 * See how I've used it with the 'faq' for an example.
 *
 * -Michael F. Wilkinson (mfw) 03/18/05
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

3include "idx.h"

char line[LINE_SIZE + 1];

int main(int argc, char *argv[])
{
    long pos;
    int i;
    int n;
    int lineno;
    int ntopics;
    char *s;
    char *topic;
    idx_mark entry;
    FILE *rfp;
    FILE *wfp;
    char textfile[40];
    char indxfile[40];

    if (argc != 2) {
        fprintf(stderr, "usage: makeindex <bundle>\n");
        fprintf(stderr, "for instance: makeindex faq\n");

        exit(1);
    }

    sprintf(textfile, "%s.txt", argv[1]);
    sprintf(indxfile, "%s.idx", argv[1]);

    rfp = fopen(textfile, "r");

    if (rfp == NULL) {
        fprintf(stderr, "Can't open %s for reading\n", textfile);

        exit(1);
    }

    wfp = fopen(indxfile, "w");

    if (wfp == NULL) {
        fprintf(stderr, "Can't open %s for writing\n", indxfile);

        exit(1);
    }

    pos = 0L;
    lineno = 0;
    ntopics = 0;

    while (fgets(line, LINE_SIZE, rfp) != NULL) {
        ++lineno;
        n = strlen(line);

        if (line[n - 1] != '\n') {
            fprintf(stderr, "line %d: Line too long\n", lineno);
        }

        if (line[0] == '&') {
            ++ntopics;

            if (ntopics > 1) {
                entry.len = (int)(pos - entry.pos);

                if (fwrite(&entry, sizeof(idx_mark), 1, wfp) < 1) {
                    fprintf(stderr, "Error writing %s\n", indxfile);

                    exit(2);
                }
            }

            topic = &line[1];

            while (((*topic == ' ') || (*topic == '\t'))
                   && (*topic != '\0')) {
                ++topic;
            }

            i = -1;
            s = topic;

            while ((*s != '\n') && (*s != '\0')) {
                if (i >= (TOPIC_NAME_LEN - 1)) {
                    break;
                }

                if ((*s != ' ') || (entry.topic[i] != ' ')) {
                    ++i;
                    entry.topic[i] = *s;
                }

                ++s;
            }

            ++i;
            entry.topic[i] = '\0';
            entry.pos = pos + (long)n;
        }

        pos += n;
    }

    entry.len = (int)(pos - entry.pos);

    if (fwrite(&entry, sizeof(idx_mark), 1, wfp) < 1) {
        fprintf(stderr, "Error writing %s\n", indxfile);

        exit(1);
    }

    fclose(rfp);
    fclose(wfp);

    printf("   Created \'%s\'\n", indxfile);
    printf("   %d topics indexed\n", ntopics);


    return 0;
}
