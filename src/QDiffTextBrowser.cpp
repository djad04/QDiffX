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

    int padding = 10;

    return padding + charWidth * lineDigitCount;
}

void QDiffTextBrowser::resizeEvent(QResizeEvent *event)
{
    QTextBrowser::resizeEvent(event);
    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
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

    QTextBlock block =  firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int blocktop = static_cast<int>(blockTop(block));
    int blockbottom = static_cast<int>(blockBottom(block));

    while(block.isValid() && blocktop <= event->rect().bottom()){
        if(block.isVisible() && blockbottom >= event->rect().top()){
            QString number = QString::number(blockNumber + 1);
            painter.setPen(0xB4B4B2);
            painter.drawText(0 , blocktop , m_lineNumberArea->width() - 4, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        if(block.isValid()) {
            blocktop = blockbottom;
            blockbottom = static_cast<int>(blockBottom(block));
        }
        ++blockNumber;

    }

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
