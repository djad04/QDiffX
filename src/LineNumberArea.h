#pragma once

#include <QWidget>

class QDiffTextBrowser;

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(QDiffTextBrowser* editor);
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QDiffTextBrowser* m_editor;
};
