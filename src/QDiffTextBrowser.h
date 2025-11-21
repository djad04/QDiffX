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
    ~QDiffTextBrowser();

    int lineNumberAreaWidth() const;
    void setDiffResult(const QDiffResult& result);

    void paintLineNumberArea(QPaintEvent* event);
    void applyDiffHighlighting();
    void applyBlockSpacing();



    QTextCharFormat getFormatForOperation(DiffOperation operation) const;
    QColor getBackgroundColorForOperation(DiffOperation operation) const;

    // Line Number Area
    static constexpr double LINE_NUMBER_AREA_PADDING_RATIO = 0.07;
    static constexpr double LINE_NUMBER_TEXT_WIDTH_RATIO = 0.3;

    //Text
    static constexpr int MIN_FONT_SIZE = 14;
    static constexpr int MAX_FONT_SIZE = 36;
    static constexpr int BASE_FONT_SIZE = 18;
    static constexpr int FONT_SCALE_DIVISOR = 400;
    static constexpr int TEXT_LEFT_MARGIN = 25;
    static constexpr int TEXT_TOP_BOTTOM_MARGIN = 8;

    //Colors
    static constexpr uint32_t LINE_NUMBER_BG_COLOR = 0xFFFEFC;
    static constexpr uint32_t LINE_NUMBER_BORDER_COLOR = 0xDDDDDD;
    static constexpr uint32_t LINE_NUMBER_TEXT_COLOR = 0x999999;
    static constexpr uint32_t INSERT_BG_COLOR = 0xD4EDDA;
    static constexpr uint32_t DELETE_BG_COLOR = 0xF8D7DA;
    static constexpr uint32_t REPLACE_BG_COLOR = 0xFFF3CD;
    static constexpr uint32_t INSERT_TEXT_COLOR = 0x155724;
    static constexpr uint32_t DELETE_TEXT_COLOR = 0x721C24;
    static constexpr uint32_t REPLACE_TEXT_COLOR = 0x856404;


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
