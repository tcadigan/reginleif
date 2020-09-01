#include "arrays.h"

#ifdef ARRAY

/* 16 Novaes of 11 (+1 for null)x7 */
char *Novae[16][7] = {
    {"           ",
     "           ",
     "           ",
     "    %*     ",
     "           ",
     "           ",
     "           "},
    {"           ",
     "           ",
     "     %     ",
     "    %*%    ",
     "     %     ",
     "           ",
     "           "},
    {"           ",
     "           ",
     "     %%    ",
     "   %%*%%   ",
     "    %%     ",
     "           ",
     "           "},
    {"           ",
     "           ",
     "     %%%   ",
     "    %*%%%  ",
     "     %%%   ",
     "           ",
     "           "},
    {"           ",
     "           ",
     "    %%%    ",
     "   %%*%%%  ",
     "    %%%%   ",
     "           ",
     "           "},
    {"           ",
     "     %     ",
     "   %%%%    ",
     "  %%%*%%%  ",
     "   %%%%%   ",
     "           ",
     "           "},
    {"           ",
     "     %     ",
     "   %%%%%   ",
     "  %% *%%%  ",
     "   %%%%%   ",
     "     %     ",
     "           "},
    {"           ",
     "     %     ",
     "  %%%%%%%  ",
     " %%% * %%% ",
     "   %%%%%   ",
     "     %%    ",
     "           "},
    {"           ",
     "    %%%    ",
     "  %%%%%%%  ",
     " %%% * %%% ",
     "  %%% %%%  ",
     "    %%%    ",
     "           "},
    {"           ",
     "    %%%%   ",
     "  %%% %%%  ",
     " %%% * %%% ",
     "  %%% %%%% ",
     "   %%%%%%  ",
     "     %%    "},
    {"           ",
     "    %%%%   ",
     "  %%% %%%  ",
     " %%% * %%% ",
     "  %%%  %%% ",
     "   %%%%%%  ",
     "     %%    "},
    {"     %%    ",
     "   %%%%%%  ",
     "  %%   %%% ",
     " %%% *  %%%",
     " %%%%  %%% ",
     "  %%%%%%%% ",
     "     %%%   "},
    {"    %%%%   ",
     "  %%%%%%%% ",
     " %%%  % %%%",
     " %%  *   %%",
     " %%%    %%%",
     "  %%%%%% % ",
     "   % %%%   "},
    {"   %%% %%  ",
     " %%% %% %% ",
     "%%      %%%",
     "%%%  * % %%",
     " % %     %%",
     " %%% % % % ",
     "  %%%%%%%  "},
    {"%  % % %%  ",
     "      %  % ",
     "%%      % %",
     "%    *   % ",
     " %        %",
     " %     % % ",
     "  % %%%    "},
    {"    %  %   ",
     "          %",
     "%          ",
     "     *    %",
     "%          ",
     " %        %",
     "   %   % % "}
};

/* 8 mirrors of size 9 (+1 for null)x5 */
char *Mirror[8][5] = {
    {"         ",
     "         ",
     "\\=======/",
     "         ",
     "         "},
    {"   |     ",
     "   \\     ",
     "   \\\\    ",
     "    \\\\   ",
     "     \\\\__"},
    {"     /   ",
     "    ||   ",
     "    ||   ",
     "    ||   ",
     "     \\   "},
    {"       __",
     "     //  ",
     "    //   ",
     "    /    ",
     "   |     "},
    {"         ",
     "         "
     "/=======\\", /* cc thinks this is a quoted " without the \ */
     "         ",
     "         "},
    {"__       ",
     "  \\\\     ", /* Weird stuff again */
     "   \\\\    ",
     "    \\\\   ",
     "     |   "},
    {"   \\     ",
     "   ||    ",
     "   ||    ",
     "   ||    ",
     "   /     "},
    {"     |   ",
     "    //   ",
     "   //    ",
     "__//     ",
     "         "}
};

#endif // ARRAY
