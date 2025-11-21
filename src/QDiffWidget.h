#pragma once

#include <QSplitter>
#include <QTextBrowser>
#include <QDiffTextBrowser.h>
#include <QWidget>
#include "QAlgorithmManager.h"

namespace QDiffX {


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

    // Display Mode Management:
    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    // Algorithm Manager Integration:
    void setAlgorithmManager(QAlgorithmManager* manager);
    QAlgorithmManager* algorithmManager() const;

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

private slots:
    void onDiffCalculated(const QDiffX::QDiffResult& result);
    void onSideBySideDiffCalculated(const QDiffX::QSideBySideDiffResult& result);

private:
    void setupUI();
    void updateDiff();
    void setupConnections();
    void connectAlgorithmManagerSignals();
    void disconnectAlgorithmManagerSignals();

    // Helper Functions
    QString readFileToQString(const QString &filePath, FileOperationResult &result);

private:
    QSplitter *m_splitter;
    QDiffTextBrowser *m_leftTextBrowser;
    QDiffTextBrowser *m_rightTextBrowser;

    QString m_leftContent;
    QString m_rightContent;
    QString m_leftLabel;
    QString m_rightLabel;

    // Display and Algorithm Management
    DisplayMode m_displayMode = DisplayMode::SideBySide;
    QAlgorithmManager* m_algorithmManager = nullptr;

    // Error Handeling
    FileOperationResult m_lastError = FileOperationResult::Success;

    // Helper methods for diff display
    void displayUnifiedDiff(const QDiffResult& result);
    void displaySideBySideDiff(const QSideBySideDiffResult& result);

};

}//namespace QDiffX
