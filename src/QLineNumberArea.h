#pragma once

#include <QWidget>
#include "QDiffTextBrowser.h"

namespace QDiffX{

class QDiffTextBrowser;

class QLineNumberArea : public QWidget
{
    Q_OBJECT
public:
    explicit QLineNumberArea(QDiffTextBrowser* editor);
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QDiffTextBrowser* m_editor;
};


}//namespace QDiffX
