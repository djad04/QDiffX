#include "LineNumberArea.h"
#include "QDiffTextBrowser.h"
#include <QPainter>
#include <QTextBlock>

LineNumberArea::LineNumberArea(QDiffTextBrowser* editor)
    : QWidget(editor), m_editor(editor) {}

QSize LineNumberArea::sizeHint() const {
    return QSize(m_editor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    m_editor->paintLineNumberArea(event);
}
