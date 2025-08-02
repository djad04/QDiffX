#include "QLineNumberArea.h"
#include "QDiffTextBrowser.h"
#include <QPainter>
#include <QTextBlock>

namespace QDiffX{


QLineNumberArea::QLineNumberArea(QDiffTextBrowser *editor)
    : QWidget(editor) , m_editor(editor)
{}

QSize QLineNumberArea::sizeHint() const {
    return QSize(m_editor->lineNumberAreaWidth(), 0);
}

void QLineNumberArea::paintEvent(QPaintEvent* event) {
    m_editor->paintLineNumberArea(event);
}

}//namespace QDiffX
