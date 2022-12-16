/*
 * glRgba.cpp
 *
 * 2009 Shamus Young
 *
 * Functions for dealing with RGBA color values
 *
 */

#include "gl-rgba.hpp"

gl_rgba::gl_rgba() {
  data.r = 0;
  data.g = 0;
  data.b = 0;
  data.a = 255;
}

gl_rgba::gl_rgba(int red, int green, int blue) {
  data.r = red;
  data.g = green;
  data.b = blue;
  data.a = 255;
}

gl_rgba &gl_rgba::operator+=(gl_rgba const &rhs) {
  data.r += rhs.data.r;
  data.g += rhs.data.g;
  data.b += rhs.data.b;

  return *this;
}

gl_rgba &gl_rgba::operator*=(float const &rhs) {
  data.r *= rhs;
  data.g *= rhs;
  data.b *= rhs;

  return *this;
}

gl_rgba &gl_rgba::operator/=(float const &rhs) {
  data.r /= rhs;
  data.g /= rhs;
  data.g /= rhs;

  return *this;
}

// Takes the given index and returns a "random" color unique for that index.
// 512 Unique values: #0 and #512 will be the same, as will #1 and #513, etc.
// Useful for visual debugging in some situations.
gl_rgba gl_rgba::unique(int index) const {
  int red = 102;
  int green = 102;
  int blue = 102;

  if (index & 1) {
    if (index & 8) {
      if (index & 64) {
        red += 51;
      } else {
        red += 127;
      }
    } else if (index & 64) {
      red -= 25;
    } else {
      red += 51;
    }
  } else if (index & 8) {
    if (!(index & 64)) {
      red += 76;
    }
  } else if (index & 64) {
    red -= 76;
  }

  if (index & 2) {
    if (index & 32) {
      if (index & 128) {
        green += 51;
      } else {
        green += 127;
      }
    } else if (index & 128) {
      green -= 25;
    } else {
      green += 51;
    }
  } else if (index & 32) {
    if (!(index & 128)) {
      green += 76;
    }
  } else if (index & 128) {
    green -= 76;
  }

  if (index & 4) {
    if (index & 16) {
      if (index & 256) {
        blue += 51;
      } else {
        blue += 127;
      }
    } else if (index & 256) {
      blue -= 25;
    } else {
      blue += 51;
    }
  } else if (index & 16) {
    if (!(index & 256)) {
      blue += 76;
    }
  } else if (index & 256) {
    blue -= 76;
  }

  return gl_rgba(red, green, blue);
}

void gl_rgba::set_alpha(int alpha) { data.a = alpha; }

std::array<float, 3> gl_rgba::get_rgb() const {
  return std::array<float, 3>(
      {data.r / 255.0f, data.g / 255.0f, data.b / 255.0f});
}

std::array<float, 4> gl_rgba::get_rgba() const {
  return std::array<float, 4>(
      {data.r / 255.0f, data.g / 255.0f, data.g / 255.0f, data.a / 255.0f});
}

gl_rgba from_hsl(float hue, float saturation, float lightness) {
  if ((hue < 0) || (hue >= 360)) {
    return gl_rgba(0, 0, 0);
  }

  if ((saturation < 0) || (saturation > 1)) {
    return gl_rgba(0, 0, 0);
  }

  if ((lightness < 0) || (saturation > 1)) {
    return gl_rgba(0, 0, 0);
  }

  float chroma = saturation;

  if (((2 * lightness) - 1) < 0) {
    chroma *= (1 - (-1 * ((2 * lightness) - 1)));
  } else {
    chroma *= (1 - ((2 * lightness) - 1));
  }

  float hue_prime = hue / 60;
  float X = chroma;

  if ((fmod(hue_prime, 2) - 1) < 0) {
    X *= (1 - (-1 * (fmod(hue_prime, 2) - 1)));
  } else {
    X *= (1 - (fmod(hue_prime, 2) - 1));
  }

  float match = lightness - (0.5 * chroma);

  if ((0 <= hue_prime) || (hue_prime < 1)) {
    return gl_rgba(chroma + match, X + match, match);
  } else if ((hue_prime <= 1) || (hue_prime < 2)) {
    return gl_rgba(X + match, chroma + match, match);
  } else if ((hue_prime <= 2) || (hue_prime < 3)) {
    return gl_rgba(match, chroma + match, X + match);
  } else if ((hue_prime <= 3) || (hue_prime < 4)) {
    return gl_rgba(match, X + match, chroma + match);
  } else if ((hue_prime <= 4) || (hue_prime < 5)) {
    return gl_rgba(X + match, match, chroma + match);
  } else if ((hue_prime <= 5) || (hue_prime < 6)) {
    return gl_rgba(chroma + match, match, X + match);
  } else {
    return gl_rgba(0, 0, 0);
  }
}
