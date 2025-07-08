#pragma once

#include <QObject>
#include <QtWidgets/QTextBrowser>
#include "LineNumberArea.h"

class QDiffTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit QDiffTextBrowser(QWidget* parent = nullptr);

    int lineNumberAreaWidth() const;
    void setDiffResult(const QDiffResult& result); // To be implemented later

protected:
    void resizeEvent(QResizeEvent* event) override;
    void scrollContentsBy(int dx, int dy) override;
    void paintEvent(QPaintEvent* event) override;

public:
    void paintLineNumberArea(QPaintEvent* event);

private:
    LineNumberArea* m_lineNumberArea;
};

