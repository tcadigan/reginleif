#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

static int constexpr SEGMENTS_PER_TEXTURE = 64;
static float constexpr ONE_SEGMENT = 1.0f / SEGMENTS_PER_TEXTURE;
static int constexpr LANES_PER_TEXTURE = 8;
static float constexpr LANE_SIZE = 1.0f / LANES_PER_TEXTURE;
static int constexpr TRIM_RESOLUTION = 256;
static int constexpr TRIM_ROWS = 4;
static float constexpr TRIM_SIZE = 1.0f / TRIM_ROWS;
static int constexpr TRIM_PIXELS = TRIM_RESOLUTION / TRIM_ROWS;
static int constexpr LOGO_RESOLUTION = 512;
static int constexpr LOGO_ROWS = 16;
static float constexpr LOGO_SIZE = 1.0 / LOGO_ROWS;
static int constexpr LOGO_PIXELS = LOGO_RESOLUTION / LOGO_ROWS;

enum class texture_t {
    light,
    soft_circle,
    sky,
    logos,
    trim,
    bloom,
    headlight,
    lattice,
    building1,
    building2,
    building3,
    building4,
    building5,
    building6,
    building7,
    building8,
    building9,
    count,
};

static int constexpr BUILDING_COUNT = static_cast<int>(texture_t::building9) - static_cast<int>(texture_t::building1) + 1;

class Texture {
public:
    texture_t my_id_;
    unsigned int glid_;
    int desired_size_;
    int size_;
    int half_;
    int segment_size_;
    bool ready_;
    bool masked_;
    bool mipmap_;
    bool clamp_;

    Texture(texture_t id, int size, bool mipmap, bool clamp, bool masked);
    void clear();
    void rebuild();
    void draw_windows();
    void draw_sky();
    void draw_headlight();
};

unsigned int texture_from_name(char *name);
unsigned int texture_id(texture_t id);
void texture_init();
void texture_term();
unsigned int texture_random_building(int index);
bool texture_ready();
void texture_reset();
void texture_update();

#endif /* TEXTURE_HPP_ */
