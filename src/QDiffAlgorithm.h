#ifndef QDIFFALGORITHM_H
#define QDIFFALGORITHM_H

#include <QString>


namespace QDiffX{

enum class DiffOperation{
    Equal,
    Insert,
    Delete,
    Replace
};

struct DiffChange{
    DiffOperation operation;
    QString text;
    int lineNumber;
    int position;

    DiffChange(DiffOperation op = DiffOperation::Equal,
               QString txt = QString(),
               int line = -1,
               int pos = -1)
        :operation(op), text(txt), lineNumber(line), position(pos) {}
};




}//namespace QDiffX

#endif // QDIFFALGORITHM_H
