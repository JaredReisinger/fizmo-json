// This file is part of fizmo-json.  Please see LICENSE.md for the license.

#ifndef FIZMO_JSON_COLUMNS_H
#define FIZMO_JSON_COLUMNS_H

#include <list>

extern "C" {
    #include <jansson.h>
}

#include "blockbuf.h"
#include "paragraph.h"


class ColumnInfo;
class LineInfo;

class Columns {
  public:
    Columns();
    Columns(const BlockBuf &buf);

    json_t *ToJson() const;

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const Columns& columns);

  private:
    void Infer(const BlockBuf &buf);
    void AddColumnText(const BlockBuf &buf, int line, int start, int end);
    ColumnInfo *EnsureColumn(int column);

    std::list<ColumnInfo> infos_;
};


class ColumnInfo {
  public:
    ColumnInfo(int column);
    ColumnInfo(const ColumnInfo &info);
    ~ColumnInfo();

    int Column() const;

    void AddLine(const BlockBuf &buf, int line, int start, int end);

    json_t *ToJson() const;

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const ColumnInfo& info);

  private:
    int column_;
    std::list<LineInfo> infos_;
};


class LineInfo {
  public:
    LineInfo(const BlockBuf &buf, int line, int start, int end);
    LineInfo(const LineInfo &info);
    ~LineInfo();

    json_t *ToJson() const;

    // Debugging helper?  Do we like this, or is the operator overload
    // obnoxious?
    friend std::ostream & operator<<(std::ostream &os, const LineInfo& info);

  private:
    int line_;
    Paragraph text_;
};



#endif // FIZMO_JSON_COLUMNS_H
