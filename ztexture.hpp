#ifndef ZTEXTURE_HPP_
#define ZTEXTURE_HPP_

class ztexture {
public:
    ztexture();
    virtual ~ztexture();

    bool ready_;
    int size_;
    unsigned int texture_;
};

#endif
