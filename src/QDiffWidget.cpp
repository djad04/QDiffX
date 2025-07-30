#include "QDiffWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

namespace QDiffX {


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
}

QDiffWidget::~QDiffWidget() {}

void QDiffWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *leftLabel = new QLabel(m_leftLabel);
    QLabel *rightLabel = new QLabel(m_rightLabel);
    headerLayout->addWidget(leftLabel);
    headerLayout->addWidget(rightLabel);
    mainLayout->addLayout(headerLayout);

    m_splitter = new QSplitter(Qt::Horizontal);
    m_leftTextBrowser = new QDiffX::QDiffTextBrowser();
    m_leftTextBrowser->setIsLeftBrowser(true);
    m_rightTextBrowser = new QDiffX::QDiffTextBrowser();
    m_rightTextBrowser->setIsLeftBrowser(false);
    m_splitter->addWidget(m_leftTextBrowser);
    m_splitter->addWidget(m_rightTextBrowser);

    mainLayout->addWidget(m_splitter);
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
    
    if (m_displayMode == DisplayMode::SideBySide) {
        // Calculate side-by-side diff asynchronously
        m_algorithmManager->calculateSideBySideDiffAsync(m_leftContent, m_rightContent);
        // Result will be handled by onSideBySideDiffCalculated slot
    } else {
        // Calculate unified diff for inline mode asynchronously
        m_algorithmManager->calculateDiffAsync(m_leftContent, m_rightContent);
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
    } else {
        // Fallback to plain text display on error
        m_leftTextBrowser->setPlainText(m_leftContent);
        m_rightTextBrowser->setPlainText(m_rightContent);
    }
}

}//namespace QDiffX
