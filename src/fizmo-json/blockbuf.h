// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_BLOCKBUF_H
#define FIZMO_JSON_BLOCKBUF_H

#include <iostream>

extern "C" {
    #include <interpreter/blockbuf.h>
    #include <tools/z_ucs.h>
}

// We can't include buffer.h without creating a circular dependency, so we
// have to make do with a forward reference to the Buffer class.
// #include "buffer.h"
class Buffer;

// Wrap the fizmo BLOCKBUF type purely to encapsulate access.  Really, this is
// so that we can write `buf.At(...)` rather than `get_blockbuf_at(buf,...)`.
// That may not seem like much, but methods that *belong* to a type are a
// powerful paradigm.  (Yay for object-oriented programming!)
class BlockBuf {
  public:
    BlockBuf(const BLOCKBUF *buf, int height = -1);

    int Width() const;
    int Height() const;

    const struct blockbuf_char& At(int line, int col) const;

    Buffer *ToBuffer() const;

  private:
    const BLOCKBUF  *buf_;
    int             height_;
};


bool operator==(const blockbuf_char& lhs, const blockbuf_char &rhs);
bool operator!=(const blockbuf_char& lhs, const blockbuf_char &rhs);


#endif // FIZMO_JSON_BLOCKBUF_H
