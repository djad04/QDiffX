#ifndef QDIFFWIDGET_H
#define QDIFFWIDGET_H

#include <QSplitter>
#include <QTextBrowser>
#include <QWidget>

class QDiffWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QDiffWidget(QWidget *parent = nullptr,
                const QString &leftLabelText = "Original",
                const QString &rightLabelText = "Modified");
    ~QDiffWidget();

    void setLeftContent(const QString leftContent);
    void setRightContent(const QString rightContent);
    void setContent(const QString &leftContent,const QString &rightContent);


    QString leftContent() const;
    QString rightContent() const;
    void resetLeftContent();
    void resetRightContent();
    void resetAll();

private:
    QSplitter *m_splitter;
    QTextBrowser *m_leftTextBrowser;
    QTextBrowser *m_rightTextBrowser;

    QString m_leftContent;
    QString m_rightContent;

};
#endif // QDIFFWIDGET_H
