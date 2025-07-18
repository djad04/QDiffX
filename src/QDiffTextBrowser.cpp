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

    setLineWrapMode(QTextEdit::NoWrap);
    setReadOnly(true);

    connect(this->verticalScrollBar(), &QScrollBar::valueChanged,
            m_lineNumberArea, QOverload<>::of(&QWidget::update));
    connect(this->horizontalScrollBar(), &QScrollBar::valueChanged,
            m_lineNumberArea, QOverload<>::of(&QWidget::update));
    connect(this->document(), &QTextDocument::blockCountChanged,
            this, [this]() { m_lineNumberArea->update(); });
    
}



int QDiffTextBrowser::lineNumberAreaWidth() const
{
    int lineCount = this->document()->blockCount();
    int lineDigitCount = std::to_string(lineCount).length() ;
    int charWidth = fontMetrics().horizontalAdvance(QLatin1Char('9'));

    int padding = LINE_NUMBER_AREA_PADDING;

    return padding + charWidth * lineDigitCount;
}

void QDiffTextBrowser::setDiffResult(const QDiffResult &result)
{
    m_diffResult = result;
    m_lineOperations.clear();

    if (!result.success()) {
        setPlainText(tr("Error: %1").arg(result.errorMessage()));
        return;
    }

    // Create a map to store content for each line position
    QMap<int, QString> lineContent;
    int maxLineNumber = -1;

    // First pass: collect all line content and find the highest line number
    for (const auto &change : result.changes()) {
        if (change.lineNumber >= 0) {
            lineContent[change.lineNumber] = change.text;
            m_lineOperations[change.lineNumber] = change.operation;
            maxLineNumber = qMax(maxLineNumber, change.lineNumber);
        }
    }

    // Build the document content
    QString content;
    QStringList lines;

    // Create enough lines to accommodate the highest line number
    for (int i = 1; i <= maxLineNumber; ++i) {
        if (lineContent.contains(i)) {
            lines.append(lineContent[i]);
        } else {
            lines.append(QString()); // Empty line
            m_lineOperations[i] = DiffOperation::Equal;
        }
    }

    // Handle changes without line numbers (append at end)
    for (const auto &change : result.changes()) {
        if (change.lineNumber < 0) {
            lines.append(change.text);
            m_lineOperations[lines.size() - 1] = change.operation;
        }
    }

    content = lines.join('\n');
    setPlainText(content);
    applyDiffHighlighting();
}

void QDiffTextBrowser::applyDiffHighlighting() {
    QTextCursor cursor(document());
    cursor.beginEditBlock();

    QTextBlock block = document()->firstBlock();
    int blockNumber = 1;

    while (block.isValid()) {
        if (m_lineOperations.contains(blockNumber)) {
            QTextCharFormat format = getFormatForOperation(m_lineOperations[blockNumber]);

            cursor.setPosition(block.position());
            cursor.setPosition(block.position() + block.length() - 1, QTextCursor::KeepAnchor);
            cursor.mergeCharFormat(format);
        }

        block = block.next();
        blockNumber++;
    }

    cursor.endEditBlock();
}

QColor QDiffTextBrowser::getBackgroundColorForOperation(DiffOperation operation) const {
    switch (operation) {
    case DiffOperation::Insert:
        return QColor(INSERT_BG_COLOR);
    case DiffOperation::Delete:
        return QColor(DELETE_BG_COLOR);
    case DiffOperation::Replace:
        return QColor(REPLACE_BG_COLOR);
    case DiffOperation::Equal:
    default:
        return QColor();
    }
}

QTextCharFormat QDiffTextBrowser::getFormatForOperation(DiffOperation operation) const {
    QTextCharFormat format;

    switch (operation) {
    case DiffOperation::Insert:
        format.setForeground(QColor(0x155724)); // Dark green text
        break;
    case DiffOperation::Delete:
        format.setForeground(QColor(0x721C24)); // Dark red text
        break;
    case DiffOperation::Replace:
        format.setForeground(QColor(0x856404)); // Dark yellow text
        break;
    case DiffOperation::Equal:
    default:
        // No special formatting for equal lines
        break;
    }

    return format;
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

    QPainter painter(viewport());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber()+1;

    QPointF scrollOffset(
        this->horizontalScrollBar()->value(),
        this->verticalScrollBar()->value()
        );

    while (block.isValid()) {
        QRectF blockRect = document()->documentLayout()->blockBoundingRect(block);
        QPointF visualPos = blockRect.topLeft() - scrollOffset;

        // Calculate the visual rectangle for the full width
        QRectF visualRect = QRectF(
            0, // Start from left edge of viewport
            visualPos.y(),
            viewport()->width(), // Full width of the viewport
            blockRect.height()
            );

        // Only paint if the block is visible
        if (visualRect.intersects(event->rect()) && visualRect.bottom() >= 0) {
            if (m_lineOperations.contains(blockNumber )) {
                QColor backgroundColor = getBackgroundColorForOperation(m_lineOperations[blockNumber]);
                if (backgroundColor.isValid()) {
                    painter.fillRect(visualRect, backgroundColor);
                }
            }
        }

        block = block.next();
        blockNumber++;

        // Break if we've gone past the visible area
        if (visualPos.y() > event->rect().bottom()) {
            break;
        }
    }


    QTextBrowser::paintEvent(event);
}

void QDiffTextBrowser::paintLineNumberArea(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(LINE_NUMBER_BG_COLOR));

    painter.setPen(QColor(LINE_NUMBER_BORDER_COLOR));
    painter.drawLine(m_lineNumberArea->width() - 1, event->rect().top(),
                     m_lineNumberArea->width() - 1, event->rect().bottom());

    QTextBlock block =  firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int blocktop = static_cast<int>(blockTop(block));
    int blockbottom = static_cast<int>(blockBottom(block));

    while(block.isValid() && blocktop <= event->rect().bottom()){
        if(block.isVisible() && blockbottom >= event->rect().top()){
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(LINE_NUMBER_TEXT_COLOR));
            painter.drawText(0 , blocktop , m_lineNumberArea->width() - m_lineNumberArea->width() * 0.3, fontMetrics().height(),
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
    int baseSize = BASE_FONT_SIZE;
    int scaledSize = baseSize * height() / FONT_SCALE_DIVISOR;
    scaledSize = qBound(MIN_FONT_SIZE, scaledSize, MAX_FONT_SIZE);

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
