#include "QDiffWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QScrollBar>

namespace QDiffX {

// Helper to interpret number of lines represented by a DiffChange::text
static int countLinesInChangeText(const QString &text) {
    // Count newline characters. If none, treat as single line.
    int lines = text.count('\n');
    return lines > 0 ? lines : 1;
}


QDiffWidget::QDiffWidget(QWidget *parent, const QString &leftLabelText, const QString &rightLabelText)
    : QWidget(parent),
    m_leftLabel(leftLabelText),
    m_rightLabel(rightLabelText)
{
    setupUI();
    setupConnections();

    // Create and set up the algorithm manager
    m_algorithmManager = new QAlgorithmManager(this);
    connectAlgorithmManagerSignals();
    // Populate algorithm combo when manager is available
    if (m_algorithmCombo) {
        QStringList algs = m_algorithmManager->getAvailableAlgorithms();
        m_algorithmCombo->addItems(algs);
        m_algorithmCombo->setEnabled(!algs.isEmpty());
    }
}

QDiffWidget::~QDiffWidget() {}

void QDiffWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Top header (labels)
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *leftLabel = new QLabel(m_leftLabel);
    QLabel *rightLabel = new QLabel(m_rightLabel);
    headerLayout->addWidget(leftLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(rightLabel);
    mainLayout->addLayout(headerLayout);

    // Toolbar with controls (theme, algorithm selector, display mode, sync)
    QHBoxLayout *toolbarLayout = new QHBoxLayout();

    if (m_showThemeControls) {
        m_themeButton = new QPushButton(tr("Theme"));
        QMenu *themeMenu = new QMenu(m_themeButton);
        QAction *lightAction = themeMenu->addAction(tr("Light"));
        QAction *darkAction = themeMenu->addAction(tr("Dark"));
        connect(lightAction, &QAction::triggered, this, [this]() { setTheme(Theme::Light); });
        connect(darkAction, &QAction::triggered, this, [this]() { setTheme(Theme::Dark); });
        m_themeButton->setMenu(themeMenu);
        toolbarLayout->addWidget(m_themeButton);
    }

    if (m_showAlgorithmSelector) {
        m_algorithmCombo = new QComboBox();
        m_algorithmCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        m_algorithmCombo->setEditable(false);
        m_algorithmCombo->setToolTip(tr("Select algorithm (scroll to change)"));
        m_algorithmCombo->setMinimumWidth(180);
        m_algorithmCombo->setEnabled(false); // enabled when manager provides list
        connect(m_algorithmCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx){
            if (!m_algorithmManager || idx < 0) return;
            QString id = m_algorithmCombo->itemText(idx);
            m_algorithmManager->setSelectionMode(QAlgorithmSelectionMode::Manual);
            m_algorithmManager->setCurrentAlgorithm(id);
            updateDiff();
        });
        toolbarLayout->addWidget(m_algorithmCombo);
    }

    toolbarLayout->addStretch();

    if (m_showDisplayModeButtons) {
        m_displayModeCombo = new QComboBox();
        m_displayModeCombo->addItem(tr("Side by Side"));
        m_displayModeCombo->addItem(tr("Inline"));
        m_displayModeCombo->setCurrentIndex(m_displayMode == DisplayMode::SideBySide ? 0 : 1);
        connect(m_displayModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx){
            setDisplayMode(idx == 0 ? DisplayMode::SideBySide : DisplayMode::Inline);
        });
        toolbarLayout->addWidget(m_displayModeCombo);
    }

    // (Sync checkbox will be in the bottom status bar for a cleaner layout)

    mainLayout->addLayout(toolbarLayout);

    // Main splitter and editor panels
    m_splitter = new QSplitter(Qt::Horizontal);

    QWidget *leftPanel = new QWidget();
    leftPanel->setObjectName("editorPanel");
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftPanel);
    QLabel *leftHeader = new QLabel(m_leftLabel);
    leftHeader->setObjectName("editorHeader");
    leftPanelLayout->addWidget(leftHeader);
    m_leftTextBrowser = new QDiffX::QDiffTextBrowser();
    leftPanelLayout->addWidget(m_leftTextBrowser);

    QWidget *rightPanel = new QWidget();
    rightPanel->setObjectName("editorPanel");
    QVBoxLayout *rightPanelLayout = new QVBoxLayout(rightPanel);
    QLabel *rightHeader = new QLabel(m_rightLabel);
    rightHeader->setObjectName("editorHeader");
    rightPanelLayout->addWidget(rightHeader);
    m_rightTextBrowser = new QDiffX::QDiffTextBrowser();
    rightPanelLayout->addWidget(m_rightTextBrowser);

    m_splitter->addWidget(leftPanel);
    m_splitter->addWidget(rightPanel);

    mainLayout->addWidget(m_splitter);

    // Bottom status bar
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    m_addedLabel = new QLabel(tr("Added: 0"));
    m_removedLabel = new QLabel(tr("Removed: 0"));
    m_addedLabel->setObjectName("addedLabel");
    m_removedLabel->setObjectName("removedLabel");
    bottomLayout->addWidget(m_addedLabel);
    bottomLayout->addSpacing(12);
    bottomLayout->addWidget(m_removedLabel);
    bottomLayout->addStretch();

    if (m_showSyncToggle) {
        m_syncScrollCheck = new QCheckBox(tr("Scroll Sync"));
        connect(m_syncScrollCheck, &QCheckBox::toggled, this, &QDiffWidget::enableSyncScrolling);
        bottomLayout->addWidget(m_syncScrollCheck);
    }

    mainLayout->addLayout(bottomLayout);
}

void QDiffWidget::updateDiff()
{
    if (!m_algorithmManager) {
        // If no algorithm manager is set, just display plain text
        m_leftTextBrowser->setPlainText(m_leftContent);
        m_rightTextBrowser->setPlainText(m_rightContent);
        return;
    }
    
    if (m_leftContent.isEmpty() && m_rightContent.isEmpty()) {
        m_leftTextBrowser->clear();
        m_rightTextBrowser->clear();
        return;
    }
    
    QString algorithmId;
    QAlgorithmSelectionMode selMode = QAlgorithmSelectionMode::Auto;
    if (m_algorithmManager) {
        selMode = m_algorithmManager->selectionMode();
        algorithmId = m_algorithmManager->currentAlgorithm();
    }

    if (m_displayMode == DisplayMode::SideBySide) {
        // Calculate side-by-side diff asynchronously, passing selection mode and algorithm id
        m_algorithmManager->calculateSideBySideDiffAsync(m_leftContent, m_rightContent, selMode, algorithmId);
        // Result will be handled by onSideBySideDiffCalculated slot
    } else {
        // Calculate unified diff for inline mode asynchronously
        m_algorithmManager->calculateDiffAsync(m_leftContent, m_rightContent, selMode, algorithmId);
        // Result will be handled by onDiffCalculated slot
        // Hide right panel in inline mode
        m_rightTextBrowser->hide();
    }
}

void QDiffWidget::setupConnections()
{
    // Connect content changes to diff updates
    connect(this, &QDiffWidget::contentChanged, this, &QDiffWidget::updateDiff);
}

// ---------------Content Setting----------------------

void QDiffWidget::setLeftContent(const QString &leftContent)
{
    m_leftContent = leftContent ;
    m_leftTextBrowser ->setPlainText(m_leftContent);
    emit contentChanged();
}

void QDiffWidget::setRightContent(const QString &rightContent)
{
    m_rightContent = rightContent ;
    m_rightTextBrowser ->setPlainText(m_rightContent);
    emit contentChanged();
}

void QDiffWidget::setContent(const QString &leftContent, const QString &rightContent)
{
    m_leftContent = leftContent ;
    m_rightContent = rightContent ;
    m_leftTextBrowser ->setPlainText(m_leftContent);
    m_rightTextBrowser ->setPlainText(m_rightContent);

    emit contentChanged();
}
// -----------Labels ------------

void QDiffWidget::setLeftLabel(const QString &leftlabel)
{
    m_leftLabel = leftlabel;
}
void QDiffWidget::setRightLabel(const QString &rightlabel)
{
    m_rightLabel = rightlabel;
}

// ------------------- file operation : ---------------------------

bool QDiffWidget::setLeftContentFromFile(const QString &path)
{
    FileOperationResult result;
    QString content = readFileToQString(path, result);

    if (result != FileOperationResult::Success) {
        // Set appropriate error for left file
        if (result == FileOperationResult::LeftFileNotFound) {
            m_lastError = FileOperationResult::LeftFileNotFound;
        } else {
            m_lastError = FileOperationResult::LeftFileReadError;
        }
        return false;
    }

    setLeftContent(content);
    m_lastError = FileOperationResult::Success;
    return true;
}

bool QDiffWidget::setRightContentFromFile(const QString &path)
{
    FileOperationResult result;
    QString content = readFileToQString(path, result);

    if (result != FileOperationResult::Success) {
        // Set appropriate error for right file
        if (result == FileOperationResult::LeftFileNotFound) {
            m_lastError = FileOperationResult::RightFileNotFound;
        } else {
            m_lastError = FileOperationResult::RightFileReadError;
        }
        return false;
    }

    setRightContent(content);
    m_lastError = FileOperationResult::Success;
    return true;
}

bool QDiffWidget::setContentFromFiles(const QString &leftPath, const QString &rightPath)
{
    FileOperationResult leftResult, rightResult;
    QString leftContent = readFileToQString(leftPath, leftResult);
    QString rightContent = readFileToQString(rightPath, rightResult);

    // Check for errors in left file
    if (leftResult != FileOperationResult::Success) {
        if (leftResult == FileOperationResult::LeftFileNotFound) {
            m_lastError = FileOperationResult::LeftFileNotFound;
        } else {
            m_lastError = FileOperationResult::LeftFileReadError;
        }
        return false;
    }

    // Check for errors in right file
    if (rightResult != FileOperationResult::Success) {
        if (rightResult == FileOperationResult::LeftFileNotFound) {
            m_lastError = FileOperationResult::RightFileNotFound;
        } else {
            m_lastError = FileOperationResult::RightFileReadError;
    }

    return false;
}


    setContent(leftContent, rightContent);
    m_lastError = FileOperationResult::Success;
    return true;
}

//- -----------------Error handling: -------------------
QDiffWidget::FileOperationResult QDiffWidget::lastError() const
{
    return m_lastError;
}

QString QDiffWidget::errorMessage(FileOperationResult result) const
{
    switch (result) {
    case FileOperationResult::Success:
        return tr("Success");
    case FileOperationResult::LeftFileNotFound:
        return tr("Left file not found");
    case FileOperationResult::RightFileNotFound:
        return tr("Right file not found");
    case FileOperationResult::RightFileReadError:
        return tr("Error reading  the right file");
    case FileOperationResult::LeftFileReadError:
        return tr("Error reading  the right file");
    default:
        return tr("Unknown error");
    }
}

// -----------------Content Management :---------------------

void QDiffWidget::resetLeftContent()
{
    setLeftContent({});
    // contentChanged() already emitted by setLeftContent()
}

void QDiffWidget::resetRightContent()
{
    setRightContent({});
    // contentChanged() already emitted by setRightContent()
}

void QDiffWidget::resetAll()
{
    m_leftContent.clear();
    m_rightContent.clear();
    m_leftTextBrowser->clear();
    m_rightTextBrowser->clear();
    m_lastError = FileOperationResult::Success;
    
    emit contentChanged();
}

QString QDiffWidget::rightContent() const
{
    return m_rightContent;
}

QString QDiffWidget::leftLabel() const
{
    return m_leftLabel;
}
QString QDiffWidget::rightLabel() const
{
    return m_rightLabel;
}

QString QDiffWidget::leftContent() const
{
    return m_leftContent;
}

// -----------------Helper Functions---------------------

QString QDiffWidget::readFileToQString(const QString &filePath, FileOperationResult &result)
{
    result = FileOperationResult::Success;


    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        result = FileOperationResult::LeftFileNotFound; // Will be overridden by caller for right file
        return QString();
    }

    if (!fileInfo.isReadable()) {
        result = FileOperationResult::LeftFileReadError; // Will be overridden by caller for right file
        return QString();
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result = FileOperationResult::LeftFileReadError; // Will be overridden by caller for right file
        return QString();
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    QString content = stream.readAll();

    if (stream.status() != QTextStream::Ok) {
        result = FileOperationResult::LeftFileReadError; // Will be overridden by caller for right file
        return QString();
    }

    return content;
}

// ----------------------- Display Mode Management -------------------------

QDiffWidget::DisplayMode QDiffWidget::displayMode() const
{
    return m_displayMode;
}

void QDiffWidget::setDisplayMode(DisplayMode mode)
{
    if (m_displayMode == mode) {
        return; // No change needed
    }
    
    m_displayMode = mode;
    
    if (mode == DisplayMode::SideBySide) {
        m_rightTextBrowser->show();
    } else {
        m_rightTextBrowser->hide();
    }
    
    updateDiff(); // Refresh display with new mode
}

// ----------------------- Algorithm Manager Integration -------------------------

void QDiffWidget::setAlgorithmManager(QAlgorithmManager* manager)
{
    // Disconnect from previous manager if any
    if (m_algorithmManager) {
        disconnectAlgorithmManagerSignals();
    }
    
    m_algorithmManager = manager;
    
    // Connect to new manager if provided
    if (m_algorithmManager) {
        connectAlgorithmManagerSignals();
    }
    
    updateDiff(); // Refresh diff with new manager
}

// UI control visibility setters
void QDiffWidget::setShowThemeControls(bool show)
{
    m_showThemeControls = show;
    if (m_themeButton) m_themeButton->setVisible(show);
}

void QDiffWidget::setShowAlgorithmSelector(bool show)
{
    m_showAlgorithmSelector = show;
    if (m_algorithmCombo) m_algorithmCombo->setVisible(show);
}

void QDiffWidget::setShowDisplayModeButtons(bool show)
{
    m_showDisplayModeButtons = show;
    if (m_displayModeCombo) m_displayModeCombo->setVisible(show);
}

void QDiffWidget::setShowSyncToggle(bool show)
{
    m_showSyncToggle = show;
    if (m_syncScrollCheck) m_syncScrollCheck->setVisible(show);
}

// Sync scrolling implementation
void QDiffWidget::enableSyncScrolling(bool enable)
{
    if (enable) {
        if (m_leftTextBrowser && m_rightTextBrowser) {
            // Connect left -> right
            m_leftScrollConn = connect(m_leftTextBrowser->verticalScrollBar(), &QScrollBar::valueChanged,
                                       this, [this](int v){
                if (m_syncingScroll) return;
                m_syncingScroll = true;
                m_rightTextBrowser->verticalScrollBar()->setValue(v);
                m_syncingScroll = false;
            });

            // Connect right -> left
            m_rightScrollConn = connect(m_rightTextBrowser->verticalScrollBar(), &QScrollBar::valueChanged,
                                        this, [this](int v){
                if (m_syncingScroll) return;
                m_syncingScroll = true;
                m_leftTextBrowser->verticalScrollBar()->setValue(v);
                m_syncingScroll = false;
            });
        }
    } else {
        if (m_leftTextBrowser && m_rightTextBrowser) {
            QObject::disconnect(m_leftScrollConn);
            QObject::disconnect(m_rightScrollConn);
        }
    }
}

// Theme application
void QDiffWidget::setTheme(Theme theme)
{
    m_theme = theme;

    // Build a global stylesheet for the whole application/window
    QString style;
    if (theme == Theme::Dark) {
        style = R"(
            QWidget { background-color: #121218; color: #e6e6e6; }
            #editorHeader { font-weight:600; padding:10px 14px; color:#d7dde3; }
            #editorPanel { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #141416, stop:1 #0f1012); border-radius:10px; border:1px solid #232326; }
            QLineNumberArea { background: #0f0f10; color: #8b95a1; }
            QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2b2b2b, stop:1 #1e1e1e); border: 1px solid #2f3136; padding:6px 10px; border-radius:8px; }
            QPushButton:hover { border-color: #4a4d52; }
            QComboBox { background: #171717; color: #e6e6e6; border: 1px solid #2b2b2b; padding:6px; border-radius:6px; }
            QCheckBox { color: #e6e6e6; }
            QMenu { background-color: #19191c; color: #e6e6e6; }
            QTextBrowser { background-color: transparent; color: #d6e6ff; padding:12px; }
            QScrollBar:vertical { background: transparent; width:10px; }
            QScrollBar::handle:vertical { background: #2b2b2b; border-radius:5px; }
            #addedLabel { color: #59c36a; font-weight:600; }
            #removedLabel { color: #e07a7a; font-weight:600; }
            QSplitter::handle { background: transparent; }
        )";
    } else {
        style = R"(
            QWidget { background-color: #fbfdff; color: #1c2430; }
            #editorHeader { font-weight:600; padding:10px 14px; color:#1b2430; }
            #editorPanel { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #ffffff, stop:1 #fbfcff); border-radius:10px; border:1px solid #e6edf6; }
            QLineNumberArea { background: #fff; color: #9aa3ad; }
            QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #ffffff, stop:1 #f3f6fa); border: 1px solid #d6dce6; padding:6px 10px; border-radius:8px; }
            QPushButton:hover { border-color: #b9c6d8; }
            QComboBox { background: #ffffff; color: #1c2430; border: 1px solid #dbe7f2; padding:6px; border-radius:6px; }
            QCheckBox { color: #1c2430; }
            QMenu { background-color: #ffffff; color: #1c2430; }
            QTextBrowser { background-color: transparent; color: #0f1720; padding:12px; }
            QScrollBar:vertical { background: transparent; width:10px; }
            QScrollBar::handle:vertical { background: #d6dbe1; border-radius:5px; }
            #addedLabel { color: #0ea44f; font-weight:600; }
            #removedLabel { color: #d9483b; font-weight:600; }
            QSplitter::handle { background: transparent; }
        )";
    }

    // Apply globally to the application so the whole window changes
    qApp->setStyleSheet(style);
}

QAlgorithmManager* QDiffWidget::algorithmManager() const
{
    return m_algorithmManager;
}

// Helper methods for diff display
void QDiffWidget::displayUnifiedDiff(const QDiffResult& result)
{
    m_leftTextBrowser->setDiffResult(result);
}

void QDiffWidget::displaySideBySideDiff(const QSideBySideDiffResult& result)
{
    m_leftTextBrowser->setDiffResult(result.leftSide);
    m_rightTextBrowser->setDiffResult(result.rightSide);
}

// Signal connection management
void QDiffWidget::connectAlgorithmManagerSignals()
{
    if (!m_algorithmManager) return;
    
    connect(m_algorithmManager, &QAlgorithmManager::diffCalculated,
            this, &QDiffWidget::onDiffCalculated);
    connect(m_algorithmManager, &QAlgorithmManager::sideBySideDiffCalculated,
            this, &QDiffWidget::onSideBySideDiffCalculated);
    connect(m_algorithmManager, &QAlgorithmManager::availableAlgorithmsChanged, this, [this](const QStringList &list){
        if (!m_algorithmCombo) return;
        m_algorithmCombo->clear();
        m_algorithmCombo->addItems(list);
        m_algorithmCombo->setEnabled(!list.isEmpty());
    });
    connect(m_algorithmManager, &QAlgorithmManager::currentAlgorithmChanged, this, [this](){
        if (!m_algorithmCombo) return;
        QString cur = m_algorithmManager->currentAlgorithm();
        int idx = m_algorithmCombo->findText(cur);
        if (idx >= 0) m_algorithmCombo->setCurrentIndex(idx);
    });
}

void QDiffWidget::disconnectAlgorithmManagerSignals()
{
    if (!m_algorithmManager) return;
    
    disconnect(m_algorithmManager, &QAlgorithmManager::diffCalculated,
               this, &QDiffWidget::onDiffCalculated);
    disconnect(m_algorithmManager, &QAlgorithmManager::sideBySideDiffCalculated,
               this, &QDiffWidget::onSideBySideDiffCalculated);
}

// Slot implementations
void QDiffWidget::onDiffCalculated(const QDiffX::QDiffResult& result)
{
    if (m_displayMode != DisplayMode::Inline) {
        return; // Ignore if not in inline mode
    }
    
    if (result.success()) {
        displayUnifiedDiff(result);
        // Update status counts
        int added = 0, removed = 0;
        for (const auto &c : result.changes()) {
            int lines = countLinesInChangeText(c.text);
            switch (c.operation) {
                case DiffOperation::Insert: added += lines; break;
                case DiffOperation::Delete: removed += lines; break;
                case DiffOperation::Replace: added += lines; removed += lines; break;
                default: break;
            }
        }
        if (m_addedLabel) m_addedLabel->setText(tr("Added: %1").arg(added));
        if (m_removedLabel) m_removedLabel->setText(tr("Removed: %1").arg(removed));
    } else {
        // Fallback to plain text display on error
        m_leftTextBrowser->setPlainText(m_leftContent);
        m_rightTextBrowser->setPlainText(m_rightContent);
    }
}

void QDiffWidget::onSideBySideDiffCalculated(const QDiffX::QSideBySideDiffResult& result)
{
    if (m_displayMode != DisplayMode::SideBySide) {
        return; // Ignore if not in side-by-side mode
    }
    
    if (result.success()) {
        displaySideBySideDiff(result);
        // Show both panels in side-by-side mode
        m_rightTextBrowser->show();
        // Update status counts using both sides
        int added = 0, removed = 0;
        for (const auto &c : result.rightSide.changes()) {
            int lines = countLinesInChangeText(c.text);
            if (c.operation == DiffOperation::Insert) added += lines;
            if (c.operation == DiffOperation::Replace) added += lines; // defensive
        }
        for (const auto &c : result.leftSide.changes()) {
            int lines = countLinesInChangeText(c.text);
            if (c.operation == DiffOperation::Delete) removed += lines;
            if (c.operation == DiffOperation::Replace) removed += lines; // defensive
        }
        if (m_addedLabel) m_addedLabel->setText(tr("Added: %1").arg(added));
        if (m_removedLabel) m_removedLabel->setText(tr("Removed: %1").arg(removed));
    } else {
        // Fallback to plain text display on error
        m_leftTextBrowser->setPlainText(m_leftContent);
        m_rightTextBrowser->setPlainText(m_rightContent);
    }
}

}//namespace QDiffX
