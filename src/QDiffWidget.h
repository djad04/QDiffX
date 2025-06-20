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
    enum class FileOperationResult{
        Success,
        LeftFileNotFound,
        RightFileNotFound,
        LeftFileReadError,
        RightFileReadError
    };

    explicit QDiffWidget(QWidget *parent = nullptr,
                const QString &leftLabelText = "Original",
                const QString &rightLabelText = "Modified");
    ~QDiffWidget();

    // Content Setting :
    void setLeftContent(const QString &leftContent);
    void setRightContent(const QString &rightContent);
    void setContent(const QString &leftContent,const QString &rightContent);

    // File Content Setting :
    bool setLeftContentFromFile(const QString &path);
    bool setRightContentFromFile(const QString &path);
    bool setContentFromFiles(const QString &leftPath, const QString &rightPath);

    // Labels
    void setLeftLabel(const QString &leftlabel);
    void setRightLabel(const QString &rightlabel);

    //Error handling :
    FileOperationResult lastError() const;
    QString errorMessage(FileOperationResult result) const;

    // Content retrieval :
    QString leftContent() const;
    QString rightContent() const;
    QString leftLabel() const;
    QString rightLabel() const;

    // Content Management :
    void resetLeftContent();
    void resetRightContent();
    void resetAll();

signals:
    void contentChanged();

private:
    void setupUI();
    void updateDiff();
    void setupConnections();

    // Helper Functions
    QString readFileToQString(const QString &filePath, FileOperationResult &result);

private:
    QSplitter *m_splitter;
    QTextBrowser *m_leftTextBrowser;
    QTextBrowser *m_rightTextBrowser;

    QString m_leftContent;
    QString m_rightContent;
    QString m_leftLabel;
    QString m_rightLabel;

    // Error Handeling
    FileOperationResult m_lastError = FileOperationResult::Success;

};
#endif // QDIFFWIDGET_H
