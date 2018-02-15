// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#include "blockbuf.h"
#include "buffer.h"
#include "util.h"


BlockBuf::BlockBuf(const BLOCKBUF *buf, int height) {
    trace(2, "[%p] %p, %d", this, buf, height);
    // explode if null?
    buf_ = buf;
    height_ = height;
}


int BlockBuf::Width() const {
    trace(3, "[%p]", this);
    return buf_->width;
}

int BlockBuf::Height() const {
    trace(3, "[%p]", this);
    return height_ == -1 ? buf_->height : height_;
}

const struct blockbuf_char& BlockBuf::At(int line, int col) const {
    trace(3, "[%p] %d, %d", this, line, col);
    // TODO: bounds-check?
    return buf_->content[(line * Width()) + col];
}


Buffer *BlockBuf::ToBuffer() const {
    auto buffer = new Buffer();

    const blockbuf_char bbchBlank = (blockbuf_char){
        .character = Z_UCS_SPACE,
        .font = buf_->default_font,
        .style = buf_->default_style,
        .foreground_colour = buf_->default_foreground_colour,
        .background_colour = buf_->default_background_colour,
    };

    const blockbuf_char bbchNewline = (blockbuf_char){.character = Z_UCS_NEWLINE};
    const int lines = Height();
    for (int l = 0; l < lines; l++) {
        // work right-to-left to find the last non-empty position...
        int end;
        for (end = buf_->width - 1; end >= 0; end--) {
            auto bbch = At(l, end);
            if (bbch != bbchBlank) {
                break;
            }
        }

        end++;

        for (int c = 0; c < end; c++) {
            buffer->Append(At(l, c));
        }
        buffer->Append(bbchNewline);
    }

    return buffer;
}


bool operator==(const blockbuf_char& lhs, const blockbuf_char &rhs) {
    return lhs.character == rhs.character &&
        lhs.font == rhs.font &&
        lhs.style == rhs.style &&
        lhs.foreground_colour == rhs.foreground_colour &&
        lhs.background_colour == rhs.background_colour;
}

bool operator!=(const blockbuf_char& lhs, const blockbuf_char &rhs) {
    return !(lhs == rhs);
}
