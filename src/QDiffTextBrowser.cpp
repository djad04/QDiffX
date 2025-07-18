#include "QDiffTextBrowser.h"
#include <QString>
#include <qevent.h>
#include <qpainter.h>
#include <qtextobject.h>
#include <QScrollArea>
#include <QAbstractTextDocumentLayout>
#include<QScrollBar>

namespace QDiffX{

QDiffTextBrowser::QDiffTextBrowser(QWidget* parent) {
    m_lineNumberArea = new QLineNumberArea(this);
    
}



int QDiffTextBrowser::lineNumberAreaWidth() const
{
    int lineCount = this->document()->blockCount();
    int lineDigitCount = std::to_string(lineCount).length() ;
    int charWidth = fontMetrics().horizontalAdvance(QLatin1Char('9'));

    int padding = 20;

    return padding + charWidth * lineDigitCount;
}

void QDiffTextBrowser::resizeEvent(QResizeEvent *event)
{
    QTextBrowser::resizeEvent(event);
    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()-2));
    setViewportMargins(lineNumberAreaWidth() + 2, 0, 0, 0);
    adjustFontSize();
}

void QDiffTextBrowser::scrollContentsBy(int dx, int dy)
{
    QTextBrowser::scrollContentsBy(dx, dy);
    m_lineNumberArea->update();
}

void QDiffTextBrowser::paintEvent(QPaintEvent *event)
{
    QTextBrowser::paintEvent(event);
    //Todo : custom paint and highlights
}

void QDiffTextBrowser::paintLineNumberArea(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(),QColor(0xFFFEFC));

    painter.setPen(QColor(0xDDDDDD));
    painter.drawLine(m_lineNumberArea->width() - 1, event->rect().top(),
                     m_lineNumberArea->width() - 1, event->rect().bottom());

    QTextBlock block =  firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int blocktop = static_cast<int>(blockTop(block));
    int blockbottom = static_cast<int>(blockBottom(block));

    while(block.isValid() && blocktop <= event->rect().bottom()){
        if(block.isVisible() && blockbottom >= event->rect().top()){
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(0x999999));
            painter.drawText(4 , blocktop , m_lineNumberArea->width() - 8, fontMetrics().height(),
                              Qt::AlignRight | Qt::AlignVCenter, number);
        }

        block = block.next();
        if(block.isValid()) {
            blocktop = blockbottom;
            blockbottom = static_cast<int>(blockBottom(block));
        }
        ++blockNumber;

    }

}

void QDiffTextBrowser::adjustFontSize()
{
    int baseSize = 14;
    int scaledSize = baseSize * height() / 400; // Adjust 400 as needed
    scaledSize = qBound(10, scaledSize, 18);

    QFont font = this->font();
    font.setPointSize(scaledSize);
    setFont(font);
}

QTextBlock QDiffTextBrowser::firstVisibleBlock()
{
    QTextBlock block = document()->firstBlock() ;
    QPointF scrollOffset(
        this->horizontalScrollBar()->value(),
        this->verticalScrollBar()->value()
        );

    while(block.isValid()){
        QRectF blockRect = this->document()->documentLayout()->blockBoundingRect(block);
        QPointF visualPos = blockRect.topLeft() - scrollOffset;

        if (visualPos.y() + blockRect.height() >= 0) {
            break;
        }

        block = block.next();
    }
    return block;
}

qreal QDiffTextBrowser::blockTop(const QTextBlock &block)
{
    QRectF blockRect = document()->documentLayout()->blockBoundingRect(block);
    return blockRect.top() - verticalScrollBar()->value();
}

qreal QDiffTextBrowser::blockBottom(const QTextBlock &block)
{
    QRectF blockRect = document()->documentLayout()->blockBoundingRect(block);
    return blockRect.bottom() - verticalScrollBar()->value();
}


}//namespace QDiffX
