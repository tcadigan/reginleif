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
 * enroll.c - Racegen interface for Galactic Bloodshed race enrollment program.
 *            Copyright (c) 1991 Leonard Dickens (leonard@cs.umd.edu)
 *
 * Anybody who does alter this program, please take credit!
 */

#include <string.h>

#include "../server/racegen.h"

#define DEFAULT_ENROLLMENT_FILENAME "enroll.saves"
#define DEFAULT_ENROLLMENT_FAILURE_FILENAME "failures.saves"

extern int enroll_valid_race(void);

int critique_to_file(FILE *, int, int);
int Dialogue(char const *, ...);
void modify_print_loop(int);
void print_to_file(FILE *, int);
int load_from_file(FILE *);
int cost_of_race(void);

/*
 * Returns:
 *   0 If race was successfully enrolled
 *   1 if not
 */
int enroll_player_race(char *failure_filename)
{
    char c[128];
    FILE *f;
    FILE *g;
    int n;
    static int recursing = 0;
    static int successful_enroll_in_fix_mode = 0;

    n = critique_to_file(NULL, 1, 1);

    while (n) {
        if (n > 1) {
            printf("Race (%s) unacceptable, for the following reasons:\n",
                   race.name);
        } else {
            printf("Race (%s) unacceptable, for the following reason:\n",
                   race.name);
        }

        critique_to_file(stdout, 1, 1);

        if (recursing) {
            printf("\"Quit\" to break out of fix mode.\n");

            return 1;
        }

        if (race.status == STATUS_ENROLLED) {
            return 0;
        }

        n = Dialogue("Abort, enroll anyway, fix, mail rejection?",
                     "abort",
                     "enroll",
                     "fix",
                     "mail",
                     0);

        /*
         * Enroll anyway
         */
        if (n == 1) {
            break;
        }

        /*
         * Fix
         */
        if (n == 2) {
            printf("Recursive racegen. \"Enroll\" or \"Quit\" to exit.\n");
            recursing = 1;
            modify_print_loop(1);
            recursing = 0;
            please_quit = recursing;

            if (successful_enroll_in_fix_mode) {
                successful_enroll_in_fix_mode = 0;

                return 0;
            }

            continue;
        }

        if (failure_filename != NULL) {
            f = fopen(failure_filename, "w+");

            if (f == NULL) {
                printf("Warning: Unable to open failures file \"%s\".\n",
                       failure_filename);
                printf("Race not saved to failures file.\n");
            } else {
                print_to_file(f, 0);
                printf("Race appended to failures file \"%s\".\n",
                       failure_filename);
                fclose(f);
            }
        }

        /*
         * Abort
         */
        if (n == 0) {
            return 1;
        }

        g = fopen(TMP, "w");

        if (g == NULL) {
            printf("Unable to open file \"%s\".\n", TMP);

            return 1;
        }

        fprintf(g, "To: %s\n", race.address);
        fprintf(g, "Subject: %s Race Registration\n", GAME);
        fprintf(g, "\n");
        fprintf(g, "The race you submitted (%s) was not accepted, ", race.name);
        if (n > 1) {
            fprintf(g, "for the following reasons:\n");
        } else {
            fprintf(g, "for the following reason:\n");
        }

        critique_to_file(g, 1, 1);
        fprintf(g, "\n");
        fprintf(g,
                "Please re-submit a race if you want to play in %s.\n",
                GAME);
        fprintf(g,
                "(Check to make sure you are using racegen %s)\n",
                RG_VERSION);
        fprintf(g, "\n");
        fprintf(g,
                "For verification, here is my understanding of your race:\n");
        print_to_file(g, 1);
        fclose(g);

        printf("Sending critique to %s via %s...", race.address, MAILER);
        fflush(stdout);
        sprintf(c, "cat %s | %s %s", TMP, MAILER, race.address);
        system(c);
        printf("done.\n");

        return 1;
    }

    if (enroll_valid_race()) {
        return enroll_player_race(failure_filename);
    }

    if (recursing) {
        successful_enroll_in_fix_mode = 1;
        please_quit = 1;
    }

    g = fopen(TMP, "w");

    if (g == NULL) {
        printf("Unable to open file \"%s\".\n", TMP);

        return 0;
    }

    fprintf(g, "To: %s\n", race.address);
    fprintf(g, "Subject %s Race Accepted\n", GAME);
    fprintf(g, "\n");
    fprintf(g, "The race you submitted (%s) was accepted.\n", race.name);

#if 0
    if (race.modified_by_deity) {
        fprintf(g, "The race was altered in order to be acceptable.\n");
        fprintf(g, "Your race now looks like this:\n");
        fprintf(g, "\n");
        print_to_file(g, verbose, 0);
        fprintf(g, "\n");
    }
#endif

    fclose(g);

    printf("Sending acceptance to %s via %s...", race.address, MAILER);
    fflush(stdout);
    sprintf(c, "cat %s | %s %s", TMP, MAILER, race.address);
    system(c);
    printf("done.\n");

    return 0;
}

int enroll(int argc, char *argv[])
{
    int ret;
    FILE *g;

    if (argc < 2) {
        argv[1] = DEFAULT_ENROLLMENT_FAILURE_FILENAME;
    }

    g = fopen(argv[1], "w+");

    if (g == NULL) {
        printf("Unable to open failures file \"%s\".\n", argv[1]);
    }

    fclose(g);

    memcpy(&last, &race, sizeof(struct x));

    /*
     * race.address will be unequal to GODADDR in the instance that this is a
     * race submission mailed from somebody other than the moderator.
     */
    if (strcmp(race.address, GODADDR)) {
        ret = enroll_player_race(argv[1]);
    } else {
        ret = critique_to_file(NULL, 1, 0);

        if (ret) {
            if (ret > 1) {
                printf("Race (%s) unacceptable, for the following reasons:\n",
                       race.name);
            } else {
                printf("Race (%s) unacceptable, for the following reason:\n",
                       race.name);
            }

            critique_to_file(stdout, 1, 0);
        } else {
            ret = enroll_valid_race();

            if (ret) {
                critique_to_file(stdout, 1, 0);
            }
        }
    }

    if (ret) {
        printf("Enroll failed.\n");
    }

    return ret;
}

/*
 * Interatively loads races from a file, and enrolls them.
 */
void process(int argc, char *argv[])
{
    FILE *f;
    FILE *g;
    int n;
    int nenrolled;

    if (argc < 2) {
        argv[1] = DEFAULT_ENROLLMENT_FILENAME;
    }

    f = fopen(argv[1], "r");

    if (f == NULL) {
        printf("Unable to open races file \"%s\".\n", argv[1]);

        return;
    }

    if (argc < 3) {
        argv[2] = DEFAULT_ENROLLMENT_FAILURE_FILENAME;
    }

    g = fopen(argv[2], "w");

    if (g == NULL) {
        printf("Unable to open failures file \"%s\".\n", argv[2]);
    }

    fclose(g);

    n = 0;
    nenrolled = 0;

    while (!feof(f)) {
        if (load_from_file(f) == 1) {
            continue;
        }

        ++n;
        printf("%s, from %s\n", race.name, race.address);

        /*
         * We need the side effects:
         */
        last_npoints = npoints;
        npoints = STARTING_POINTS - cost_of_race();

        if (!enroll_player_race(argv[2])) {
            ++nenrolled;
        }
    }

    fclose(f);

    printf("Enrolled %d ", nenrolled);

    if (nenrolled != 1) {
        printf("races; ");
    } else {
        printf("race; ");
    }

    printf("%d ", n - nenrolled);

    if ((n - nenrolled) != 1) {
        printf("failures ");
    } else {
        printf("failure ");
    }

    printf("saved in file %s.\n", argv[2]);
}
