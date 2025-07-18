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
