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
}

QDiffWidget::~QDiffWidget() {}

void QDiffWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *headerLayout = new QHBoxLayout(this);
    QLabel *leftLabel = new QLabel(m_leftLabel, this);
    QLabel *rightLabel = new QLabel(m_rightLabel, this);
    headerLayout->addWidget(leftLabel);
    headerLayout->addWidget(rightLabel);
    mainLayout->addLayout(headerLayout);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_leftTextBrowser = new QTextBrowser(m_splitter);
    m_rightTextBrowser = new QTextBrowser(m_splitter);

    mainLayout->addWidget(m_splitter);
}

void QDiffWidget::updateDiff()
{
// TODO: implement the definition (requires the algorythme manager)
}

void QDiffWidget::setupConnections()
{
// NOTE: all signal connections here
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
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    resetLeftContent();
    resetRightContent();

    blockSignals(wasBlocked);

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

}//namespace QDiffX
