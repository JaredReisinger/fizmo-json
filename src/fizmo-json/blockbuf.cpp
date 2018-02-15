// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "blockbuf.h"
#include "util.h"


BlockBuf::BlockBuf(const BLOCKBUF *buf)
: buf_(buf) {
    trace(2, "%p", buf);
    // explode if null?
}

const struct blockbuf_char& BlockBuf::At(int line, int col) const {
    return buf_->content[(line * buf_->width) + col];
}
