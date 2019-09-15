// Jonathan Dearborn 05/07/2013
// Converts pixie files
// Based on pixedit by Zardus

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <list>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <vector>

// Color cycling colors indices
#define WATER_START 208
#define WATER_END 223
#define ORANGE_START 224
#define ORANGE_END 231

bool no_cycling_colors = false;

Sint8 ourcolors[] = {
     0,  0,  0,  8,  8,  8, 16, 16, 16, 24, 24, 24, 32, 32, 32, 40, 40, 40, 48,
    48, 48, 56, 56, 56,  1,  1,  1,  9,  9,  9, 17, 17, 17, 25, 25, 25, 33, 33,
    33, 41, 41, 41, 49, 49, 49, 57, 57, 57,  0,  0,  0, 15, 15, 15, 18, 18, 18,
    21, 21, 21, 24, 24, 24, 27, 27, 27, 30, 30, 30, 33, 33, 33, 36, 36, 36, 39,
    39, 39, 42, 42, 42, 45, 45, 45, 48, 48, 48, 51, 51, 51, 54, 54, 54, 57, 57,
    57, 57, 16, 16, 54, 18, 18, 51, 20, 20, 48, 22, 22, 45, 24, 24, 42, 26, 26,
    39, 28, 28, 36, 30, 30, 57,  0,  0, 52,  0,  0, 47,  0,  0, 42,  0,  0, 37,
     0,  0, 32,  0,  0, 27,  0,  0, 22,  0,  0, 16, 57, 16, 18, 54, 18, 20, 51,
    20, 22, 48, 22, 24, 45, 24, 26, 42, 26, 28, 39, 28, 30, 36, 30,  0, 57,  0,
     0, 52,  0,  0, 47,  0,  0, 42,  0,  0, 37,  0,  0, 32,  0,  0, 27,  0,  0,
    22,  0, 16, 16, 57, 18, 18, 54, 20, 20, 51, 22, 22, 48, 24, 24, 45, 26, 26,
    42, 28, 28, 39, 30, 30, 36,  0,  0, 57,  0,  0, 52,  0,  0, 47,  0,  0, 42,
     0,  0, 37,  0,  0, 32,  0,  0, 27,  0,  0, 22, 57, 57, 16, 54, 54, 18, 51,
    51, 20, 48, 48, 22, 45, 45, 24, 42, 42, 26, 39, 39, 28, 36, 36, 30, 57, 57,
     0, 52, 52,  0, 47, 47,  0, 42, 42,  0, 37, 37,  0, 32, 32,  0, 27, 27,  0,
    22, 22,  0, 57, 16, 57, 54, 18, 54, 51, 20, 51, 48, 22, 48, 45, 24, 45, 42,
    26, 42, 39, 28, 39, 36, 30, 36, 57,  0, 57, 52,  0, 52, 47,  0, 47, 42,  0,
    42, 37,  0, 37, 32,  0, 32, 27,  0, 27, 22,  0, 22, 16, 57, 57, 18, 54, 54,
    20, 51, 51, 22, 48, 48, 24, 45, 45, 26, 42, 42, 28, 39, 39, 30, 36, 36,  0,
    57, 57,  0, 52, 52,  0, 47, 47,  0, 42, 42,  0, 37, 37,  0, 32, 32,  0, 27,
    27,  0, 22, 22, 57, 41, 25, 52, 36, 20, 47, 31, 15, 42, 26, 10, 37, 21,  5,
    32, 16,  0, 27, 11,  0, 22,  6,  0, 50, 40, 30, 45, 35, 25, 40, 30, 20, 35,
    25, 15, 30, 20, 10, 25, 15,  5, 20, 10,  0, 15,  5,  0, 57, 25, 41, 52, 20,
    36, 47, 15, 31, 42, 10, 26, 37,  5, 21, 32,  0, 16, 27,  0, 11, 22,  0,  6,
    50, 30, 40, 45, 25, 35, 40, 20, 30, 35, 15, 25, 30, 10, 20, 25,  5, 15, 20,
     0, 10, 15,  0,  5,  0, 18,  6,  0, 16,  6,  0, 13,  5,  0, 11,  5,  0,  8,
     3,  0,  6,  2,  0,  3,  1,  0,  2,  0, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 41, 25, 57, 36, 20, 52, 31, 15, 47, 26, 10, 42, 41,
     5, 37, 16,  0, 32, 11,  0, 27,  6,  0, 22, 40, 30, 50, 35, 25, 45, 30, 20,
    40, 25, 15, 35, 20, 10, 30, 15,  5, 25, 10,  0, 20,  5,  0, 15, 25, 41, 57,
    23, 39, 55, 21, 37, 53, 19, 35, 51, 17, 33, 49, 15, 31, 47, 13, 29, 45, 11,
    27, 43,  9, 25, 41,  7, 23, 39,  5, 21, 37,  3, 19, 35,  1, 17, 33,  0, 15,
    31,  0, 13, 29,  0, 11, 27, 57, 15,  0, 57, 21,  0, 57, 27,  0, 57, 33,  0,
    57, 39,  0, 57, 45,  0, 57, 51,  0, 57, 57,  0, 57, 15,  0, 57, 21,  0, 57,
    27,  0, 57, 33,  0, 57, 39,  0, 57, 45,  0, 57, 51,  0, 57, 57,  0, 57, 37,
    31, 51, 33, 27, 47, 28, 24, 43, 24, 20, 56, 35, 23, 52, 32, 24, 48, 30, 22,
    44, 27, 19, 28, 18, 18, 30, 20, 20, 32, 22, 22, 34, 24, 24, 36, 26, 26, 38,
    28, 28, 40, 30, 30, 42, 32, 32
};

/*
 * For reference here are the converted RGB values for the cycling colors. If
 * you use one of these colors without the -n flag, it should start at that
 * color and cycle through the rest.
 *
 * WATER:
 * (100, 164, 228): 0x64A4E4
 * (92, 156, 220): 0x5C9CDC
 * (84, 148, 212): 0x5494D4
 * (76, 140, 204): 0x4C8CCC
 * (68, 132, 196): 0x4484C4
 * (60, 124, 188): 0x3C7CBC
 * (52, 116, 180): 0x3474B4
 * (44, 108, 172): 0x2C6CAC
 * (36, 100, 164): 0x2464A4
 * (28, 92, 156); 0x1C5C9C
 * (20, 84, 148): 0x145494
 * (12, 75, 140): 0x0C4C8C
 * (4, 68, 132): 0x044484
 * (0, 60, 124): 0x003C7C
 * (0, 52, 116): 0x003474
 * (0, 44, 108): 0x002C6C
 *
 * ORANGE:
 * (228, 60, 0): 0xE43C00
 * (228, 84, 0): 0xE45400
 * (228, 108, 0): 0xE46C00
 * (228, 132, 0): 0xE48400
 * (228, 156, 0): 0xE49C00
 * (228, 180, 0): 0xE4B400
 * (228, 204, 0): 0xE4CC00
 * (228, 228, 0): 0xE4E400
 */

// From http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
const char *get_filename_ext(char const *filename)
{
    char const *dot = strrchr(filename, '.');

    if (!dot || (dot == filename)) {
        return "";
    }

    return dot + 1;
}

Uint32 getPixel(SDL_Surface *surface, Sint32 x, Sint32 y)
{
    Uint8 *bits;
    Uint32 bpp;

    if ((x < 0) || (x >= surface->w)) {
        // Best I could do for errors
        return 0;
    }

    bpp = surface->format->BytesPerPixel;
    bits = (static_cast<Uint8 *>(surface->pixels) + (y * surface->pitch)) + (x * bpp);

    switch (bpp) {
    case 1:
        return *((static_cast<Uint8 *>(surface->pixels) + (y * surface->pitch)) + x);
    case 2:
        return *((static_cast<Uint16 *>(surface->pixels) + ((y * surface->pitch) / 2)) + x);
    case 3:
    {
        // Endian-correct, but slower
        Uint8 r;
        Uint8 g;
        Uint8 b;

        r = *(bits + (surface->format->Rshift / 8));
        g = *(bits + (surface->format->Gshift / 8));
        b = *(bits + (surface->format->Bshift / 8));

        return SDL_MapRGB(surface->format, r, g, b);
    }
    case 4:
        return *((static_cast<Uint32 *>(surface->pixels) + ((y * surface->pitch) / 4)) + x);
    }

    // FIXME: Handle errors better
    return 0;
}

Uint8 is_in_range(Sint32 value, Sint32 target, Sint32 range) {
    return ((value >= (target - range)) && (value <= (target + range)));
}

Sint32 get_color_index(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (a == 0) {
        return 0;
    }

    Sint32 rr = r / 4;
    Sint32 gg = g / 4;
    Sint32 bb = b / 4;

    Sint32 numcolors = sizeof(ourcolors) / sizeof(Sint8);

    Sint32 slop = 0;

    // Look for nearest color
    while (1) {
        Sint32 i;

        // Skip matching transparent black (0, 0, 0)
        if (is_in_range(0, rr, slop)
            && is_in_range(0, bb, slop)
            && is_in_range(0, gg, slop)) {
            // (1, 1, 1)
            return 8;
        }

        for (i = 0; i < numcolors; ++i) {
            // Don't match with cycling colors
            if (no_cycling_colors) {
                if ((WATER_START <= i) && (i <= WATER_END)) {
                    continue;
                }

                if ((ORANGE_START <= i) && (i <= ORANGE_END)) {
                    continue;
                }
            }

            if (is_in_range(ourcolors[i * 3], rr, slop)
                && is_in_range(ourcolors[(i * 3) + 1], gg, slop)
                && is_in_range(ourcolors[(i * 3) + 2], bb, slop)) {
                return i;
            }
        }

        // Look again with wider tolerance...
        ++slop;
    }

    return 0;
}

// Raw data loader from any format
unsigned char *load_pix_data(char const *filename, Uint8 *numframes,
                             Uint8 *width, Uint8 *height)
{
    // A pix file to load
    if (strcmp(get_filename_ext(filename), "pix") == 0) {
        std::cout << "Reading pix file: " << filename << std::endl;

        SDL_RWops *file;
        Uint8 *data;

        file = SDL_RWFromFile(filename, "rb");
        if (file == nullptr) {
            std::cout << "Failed to open " << filename << std::endl;

            exit(1);
        }

        SDL_RWread(file, numframes, 1, 1);
        SDL_RWread(file, width, 1, 1);
        SDL_RWread(file, height, 1, 1);

        Sint32 size = (*numframes * *width) * *height;
        data = new Uint8[size];
        SDL_RWread(file, data, 1, size);

        SDL_RWclose(file);

        return data;
    }

    // A standard image type to load
    std::cout << "Reading image: " << filename << std::endl;

    // An image file to load
    SDL_Surface *surface = IMG_Load(filename);
    if (surface == nullptr) {
        std::cout << "Failed to load " << filename << std::endl;

        exit(1);
    }

    if (surface->w > 255) {
        std::cout << "File " << filename << " has width that is too big for pix (>255)" << std::endl;

        exit(1);
    }

    if (surface->h > 255) {
        std:: cout << "File " << filename << " has height that is too big for pix (>255)" << std::endl;

        exit(1);
    }

    *numframes = 1;
    *width = surface->w;
    *height = surface->h;

    Sint32 size = (*numframes * *width) * *height;
    Uint8 *data = new Uint8[size];
    // Only one
    Sint32 frame = 1;

    // File with pixel data
    Sint32 x = *width;
    Sint32 y = *height;
    Sint32 i;
    Sint32 j;

    for (i = 0; i < *height; ++i) {
        for (j = 0; j < *width; ++j) {
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;
            Uint32 c;

            c = getPixel(surface, j, i);
            SDL_GetRGBA(c, surface->format, &r, &g, &b, &a);
            data[((((frame - 1) * x) * y) + (i * x)) + j] = get_color_index(r, g, b, a);
        }
    }

    SDL_FreeSurface(surface);

    return data;
}

void convert_to_pix(char const *filename)
{
    Uint8 numframes;
    Uint8 x;
    Uint8 y;
    Uint8 *data = load_pix_data(filename, &numframes, &x, &y);

    // Save it to pix
    std::stringstream outname;
    outname << filename << ".pix";

    SDL_RWops outfile;
    outfile = SDL_RWFromFile(outname.str().c_str(), "wb");
    if (outfile == nullptr) {
        std::cout << "Couldn't open \"" << outname.str() << "\" for writing." << std::endl;

        return;
    }

    SDL_RWwrite(outfile, &numframes, 1, 1);
    SDL_RWwrite(outfile, &x, 1, 1);
    SDL_RWwrite(outfile, &y, 1, 1);
    SDL_RWwrite(outfile, data, 1, (numframes * x) * y);
    SDL_RWclose(outfile);

    delete[](data);
}

void convert_to_png(char const *filename)
{
    Uint8 numframes;
    Uint8 x;
    Uint8 y;
    Uint8 *data = load_pix_data(filename, &numframes, &x, &y);
    Sint32 i;
    Sint32 j;
    Sint32 frame;

    std::cout << "=================== " << filename << " ===================" << std::endl
              << "num of frames: " << static_cast<unsigned>(numframes) << std::endl
              << "x: " << static_cast<unsigned>(x) << std::endl
              << "y: " << static_cast<unsigned>(y) << std::endl;

    SDL_init(SDL_INIT_VIDEO);

    std::cout << "Saving pix frames to png" << std::endl;

    for (frame = 1; fram <= numframes; ++frame) {
        SDL_Surface *pixie = SDL_CreateRGBSurface(SDL_SWSURFACE, x, y, 32,
                                                  0x00FF0000,
                                                  0x0000FF00,
                                                  0x000000FF,
                                                  0xFF000000);

        // Draw sprite frame
        for (i = 0; i < y; ++i) {
            for (j = 0; j < x; ++j) {
                SDL_Rect rect;
                Sint32 r;
                Sint32 g;
                Sint32 b;
                Sint32 c;
                Sint32 d;

                d = data[((((frame - 1) * x) * y) + (i * x)) + j];
                r = ourcolors[d * 3] * 4;
                g = ourcolors[(d * 3) + 1] * 4;
                b = ourcolors[(d * 3) + 2] * 4;

                rect.x = j;
                rect.y = i;
                rect.w = 1;
                rect.h = 1;

                if ((r > 0) || (g > 0) || (b > 0)) {
                    c = SDL_MapRGB(pixie->format, r, g, b);
                    SDL_FillRect(pixie, &rect, c);
                }
            }
        }

        // Save result
        std::stringstream buf;
        buf << filename << frame << ".png";

        SDL_SavePNG(buf.str().c_str());
        std::cout << "Frame saved: " << buf.str() << std::endl;
    }

    delete[](data);
}

void concatenate_pix(std::vector<std::string> &files)
{
    Uint8 total_frames = 0;
    Uint8 numframes;
    Uint8 width;
    Uint8 height;
    Uint8 w;
    Uint8 h;
    Uint8 *data;
    std::stringstream outname;
    RW_ops outfile;
    bool first = true;
    size_t i;

    for (i = 0; i < files.size(); ++i) {
        std::string const &filename = files[i];
        data = load_pix_data(filename.c_str(), &numframes, &w, &h);

        if (first) {
            first = false;
            total_frames += numframes;
            width = w;
            height = h;

            // Open new file for writing
            outname << filename << ".pix";
            outfile = SDL_RWFromFile(outname.str().c_str(), "wb");
            if (outfile == nullptr) {
                std::cout << "Failed to open \"" << outname.str() << "\" for writing." << std::endl;

                exit(2);
            }

            // This will be rewritten later
            SDL_RWwrite(outfile, &total_frames, 1, 1);
            SDL_RWwrite(outfile, &width, 1, 1);
            SDL_RWwrite(outfile, &height, 1, 1);
        } else {
            if ((w != width) || (h != height)) {
                // Mismatched dims error
                std::cout << "File (" << filename << ") dimension(" << w << "x" << h
                          << ") do not match the output file(" << width << "x"
                          << height << ")." << std::endl;

                SDL_RWclose(outfile);
                std::remove(outname.str().c_str());

                exit(1);
            }

            total_frames += numframes;
        }

        SDL_RWwrite(outfile, data, (numframes * w) * h);
        delete[](data);
    }

    if (outfile == nullptr) {
        std::cerr << "No output file was opened!" <<std::endl;
    }

    // Go back to the beginning to update the number of frames
    SDL_RWseek(outfile, 0, SEEK_SET);
    SDL_RWwrite(outfile, &total_frames, 1, 1);

    std::cout << "File saved: " << outname.str() << std::endl;
    SDL_RWclose(outfile);
}

std::vector<std::string> explodev(std::string const & str, Sint8 delimiter)
{
    std::vector<std::string> result;
    size_t oldPos = 0;
    size_t pos = str.find_first_of(delimited);

    while (pos != std::string::npos) {
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos + 1;
        pos = str.find_first_of(delimiter, oldPos);
    }

    result.push_back(str.substr(oldPos, std::string::npos));

    // Test this:
    /*
     * Uint8 pos;
     * do {
     *     pos = str.find_first_of(delimiter, oldPos);
     *     result.push_back(str.substr(oldPos, pos - oldPos));
     *     oldPos = pos + 1;
     * } while (pos != std::string::npos);
     */

    return result;
}

bool isFile(std::string const &filename)
{
    struct stat status;
    stat(filename.c_str(), &status);

    return (status.st_mode & S_IFREG);
}

std::vector<std::string> list_files(std::string const &dirname)
{
    std::list<std::string> fileList;
    DIR *dir = opendir(dirname.c_str());
    dirent *entry;

    if (dir == nullptr) {
        return std::vector<std::string>();
    }

    entry = readdir(dir);

    while (entry != nullptr) {
#ifdef WIN32
        if (isfile(direname + "/" + entry->d_name)) {
            fileList.push_back(entry->d_name);
        }

#else
        if (entry->d_type 1= DT_DIR) {
            fileList.push_back(entry->d_name);
        }

        entry = readdir(dir);
    }

    closedir(dir);
    fileList.sort();

    std::vector<std::string> result;
    result.assign(fileList.begin(), fileList.end());

    return result;
}

std::string stripToDir(std::string const &filename)
{
    size_t lastSlash = filename.find_last_of("/\\");

    if (lastSlash == std::string::npos) {
        return ".";
    }

    return filename.substr(0, lastSlash);
}

void parse_args(Sint32 argc, Sint8 *argv[], Sint32 *mode, std::vector<std::string> &files)
{
    Sint32 i = 1;

    if (argc < 2) {
        // Error
        *mode = 0;

        return;
    }

    // Normal conversion mode
    *mode = 1;

    // Grab the rest of the files
    Sint32 j = 0;
    while (i < argv) {
        std::string arg = argv[i];
        size_t len = arg.size();

        if ((len > 1) && (arg[0] == '-')) {
            // It's a flag
            if ((arg[1] == 'c')
                || (arg == "--cat")
                || (arg == "--concat")
                || (arg == "--concatenate")) {
                // Concatenate files into one pixconvert
                std::cerr << "Concatenating files...";
                *mode = 2;
            } else if ((arg[1] == 'n')
                       || (arg == "--no-cycle")
                       || (arg == "--no-cycling")) {
                // Disable matching cycling colors in image -> pix conversions
                std::cerr << "Disabling cycling colors...";
                no_cycling_colors = true;
            }
        } else {
            // It's a file

            // Does it have a sequence wildcard?
            if (arg.find_first_of('#') != std::string::npos) {
                // Not terribly robust, sorry...
                std::vector<std::string> parts = explodev(arg, '#');
                std::vector<std::string> file_list = list_files(stripToDir(arg));
                std::stringstream buf;
                Sint32 i = 0;
                Sint32 missed = 0;

                while (i < 256) {
                    buf.clear();
                    buf << parts[0] << i << parts[1];
                    if (std::find(file_list.begin(), file_list.end(), buf.str().c_str()) != file_list.end()) {
                        files.push_back(buf);
                    } else {
                        ++missed;

                        if (missed > 1) {
                            break;
                        }
                    }

                    ++i;
                }
            } else {
                // Normal file name
                files.push_back(arg);
            }

            ++j;
        }

        ++i;
    }
}

int main(int argc, char *argv[])
{
    Sint32 mode;
    std::vector<std::string> files;
    parse_args(argc, argv, &mode, files);

    switch(mode) {
    case 0:
        std::cout << std::endl << "USAGE" << std::endl
                  << "Convert image to pix:" << std::endl << "    pixconvert image.ext" << std::endl
                  << "Convert pix to png:" << std::endl << "   pixconvert image.pix" << std::endl
                  << "Concatenate pix into multiframe/animate pix" << std::endl
                  << "    pixconvert -c image1.ext image2.ext ..." << std::endl
                  << std::endl << "OPTIONS" << std::endl
                  << " -c, --cat, --concat, --concatenate" << std::endl
                  << "    Combines the listed files into one pix file as frames. Every listed "
                  << "file must have the same width and height." << std::endl << std::endl
                  << " -n, --no-cycling" << std::endl
                  << "    Disables matching cycling colors when converting an image "
                  << "file to pix." << std::endl << std::endl
                  << " Use # (pound) sign as a wildcard for sequences." << std::endl
                  << " It will look for a starting number of 0 or 1." << std::endl
                  << " e.g. `pixconvert -c text#.png` will concatenate text0.png, "
                  << "text1.png, etc." << std::endl << std::endl;

        return 1;
    case 1:
    {
        size_t i;
        for (i = 0; i < files.size(); ++i) {
            string const &filesname = files[i];
            bool usingPix = (strcmp(get_filename_ext(filename.c_str()), "pix") == 0);

            if (usingPix) {
                convert_to_png(filename.c_str());
            } else {
                convert_to_pix(filename.c_str());
            }
        }
    }

    break;
    case 2:
        concatenate_pix(files);

        break;
    }

    SDL_Quit();

    return 0;
}
