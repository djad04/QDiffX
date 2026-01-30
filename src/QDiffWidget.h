#pragma once

#include <QSplitter>
#include <QTextBrowser>
#include <QDiffTextBrowser.h>
#include <QWidget>
#include "QAlgorithmManager.h"
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QMenu>

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

    enum class Theme {
        Light,
        Dark
    };

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

    // UI control visibility (can be turned off in code)
    void setShowThemeControls(bool show);
    void setShowAlgorithmSelector(bool show);
    void setShowDisplayModeButtons(bool show);
    void setShowSyncToggle(bool show);

    // Sync scrolling and theme
    void enableSyncScrolling(bool enable);
    void setTheme(Theme theme);

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
    QWidget* m_leftPanel = nullptr;
    QWidget* m_rightPanel = nullptr;

    // Toolbar controls
    QPushButton* m_themeButton = nullptr;
    QPushButton* m_algorithmButton = nullptr;
    QPushButton* m_displayModeButton = nullptr;
    QCheckBox* m_syncScrollCheck = nullptr;
    QLabel* m_addedLabel = nullptr;
    QLabel* m_removedLabel = nullptr;

    // Visibility flags for controls (default: enabled)
    bool m_showThemeControls = true;
    bool m_showAlgorithmSelector = true;
    bool m_showDisplayModeButtons = true;
    bool m_showSyncToggle = true;

    // Sync scroll helpers
    bool m_syncingScroll = false;
    QMetaObject::Connection m_leftScrollConn;
    QMetaObject::Connection m_rightScrollConn;

    Theme m_theme = Theme::Light;

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
