#pragma once

#include <QObject>
#include <QtWidgets/QTextBrowser>
#include "QLineNumberArea.h"
#include "QDiffAlgorithm.h"

namespace QDiffX{

class QLineNumberArea;

class QDiffTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit QDiffTextBrowser(QWidget* parent = nullptr);
    
    int lineNumberAreaWidth() const;
    void setDiffResult(const QDiffResult& result);
    void paintLineNumberArea(QPaintEvent* event);
    void applyDiffHighlighting();
    QTextCharFormat getFormatForOperation(DiffOperation operation) const;
    QColor getBackgroundColorForOperation(DiffOperation operation) const;

    // Constants
    static constexpr int LINE_NUMBER_AREA_PADDING = 40;
    static constexpr double LINE_NUMBER_TEXT_WIDTH_RATIO = 0.3;
    static constexpr int MIN_FONT_SIZE = 10;
    static constexpr int MAX_FONT_SIZE = 18;
    static constexpr int BASE_FONT_SIZE = 14;
    static constexpr int FONT_SCALE_DIVISOR = 400;
    static constexpr uint32_t LINE_NUMBER_BG_COLOR = 0xFFFEFC;
    static constexpr uint32_t LINE_NUMBER_BORDER_COLOR = 0xDDDDDD;
    static constexpr uint32_t LINE_NUMBER_TEXT_COLOR = 0x999999;
    static constexpr uint32_t INSERT_BG_COLOR = 0xD4EDDA;
    static constexpr uint32_t DELETE_BG_COLOR = 0xF8D7DA;
    static constexpr uint32_t REPLACE_BG_COLOR = 0xFFF3CD;
protected:
    void resizeEvent(QResizeEvent* event) override;
    void scrollContentsBy(int dx, int dy) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void adjustFontSize();
    //Helpers
    QTextBlock firstVisibleBlock();
    qreal blockTop(const QTextBlock& block);
    qreal blockBottom(const QTextBlock& block);
private:
    QLineNumberArea* m_lineNumberArea;
    QDiffResult m_diffResult;
    QMap<int,DiffOperation> m_lineOperations;
};

}// namespace QDiffX
