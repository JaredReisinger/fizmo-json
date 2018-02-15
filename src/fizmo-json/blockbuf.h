// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_BLOCKBUF_H
#define FIZMO_JSON_BLOCKBUF_H

#include <iostream>

extern "C" {
    #include <interpreter/blockbuf.h>
    #include <tools/z_ucs.h>
}

// Wrap the fizmo BLOCKBUF type purely to encapsulate access.  Really, this is
// so that we can write `buf.At(...)` rather than `get_blockbuf_at(buf,...)`.
// That may not seem like much, but methods that *belong* to a type are a
// powerful paradigm.  (Yay for object-oriented programming!)
class BlockBuf {
  public:
    BlockBuf(const BLOCKBUF *buf);

    const struct blockbuf_char& At(int line, int col) const;

  private:
    const BLOCKBUF *buf_;
};


#endif // FIZMO_JSON_BLOCKBUF_H
