#ifndef QDIFFWIDGET_H
#define QDIFFWIDGET_H

#include <QSplitter>
#include <QTextBrowser>
#include <QWidget>

class QDiffWidget : public QWidget
{
    Q_OBJECT

public:
    enum class DisplayMode{
        SideBySide,
        Inline
    };
    explicit QDiffWidget(QWidget *parent = nullptr,
                const QString &leftLabelText = "Original",
                const QString &rightLabelText = "Modified");
    ~QDiffWidget();

    // Content Setting :
    void setLeftContent(const QString &leftContent);
    void setRightContent(const QString &rightContent);
    void setContent(const QString &leftContent,const QString &rightContent);

    // Labels
    void setLeftLabel(const QString &leftlabel);
    void setRightLabel(const QString &rightlabel);

    // Content retrieval :
    QString leftContent() const;
    QString rightContent() const;
    QString leftLabel() const;
    QString rightLabel() const;

    // Content Management :
    void resetLeftContent();
    void resetRightContent();
    void resetAll();

private:
    void setupUI();
    void updateDiff();

private:
    QSplitter *m_splitter;
    QTextBrowser *m_leftTextBrowser;
    QTextBrowser *m_rightTextBrowser;

    QString m_leftContent;
    QString m_rightContent;
    QString m_leftLabel;
    QString m_rightLabel;


};
#endif // QDIFFWIDGET_H
