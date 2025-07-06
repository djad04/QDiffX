#include "QAlgorithmManager.h"
#include <QtConcurrent/QtConcurrent>

namespace QDiffX{
const QString QAlgorithmManager::DEFAULT_ALGORITHM = "dtl";
const QString QAlgorithmManager::DEFAULT_FALLBACK = "dmp";


QAlgorithmManager::QAlgorithmManager(QObject *parent)
    : QObject(parent),
    m_currentAlgorithm(DEFAULT_ALGORITHM),
    m_fallBackAlgorithm(DEFAULT_FALLBACK),
    m_selectionMode(QAlgorithmSelectionMode::Auto),
    m_executionMode(QExecutionMode::Synchronous),
    m_lastError(QAlgorithmManagerError::None),
    m_isCalculating(false)
{

}

QFuture<QDiffResult> QAlgorithmManager::calculateDiff(const QString &leftText, const QString &rightText, QExecutionMode executionMode, QAlgorithmSelectionMode selectionMode, QString algorithmId)
{
    if (executionMode == QExecutionMode::Synchronous) {
        QPromise<QDiffResult> promise;
        promise.start();
        promise.addResult(calculateDiffSync(leftText, rightText, selectionMode, algorithmId));
        promise.finish();
        return promise.future();
    } else {
        return calculateDiffAsync(leftText, rightText, selectionMode, algorithmId);
    }
}

QFuture<QDiffResult> QAlgorithmManager::calculateDiffAsync(const QString &leftText, const QString &rightText, QAlgorithmSelectionMode selectionMode, QString algorithmId)
{
    QString algorithm;
    if(selectionMode == QAlgorithmSelectionMode::Manual)
    {
        if(algorithmId.isEmpty()) {
            if (m_currentAlgorithm.isEmpty()) {
                setLastError(QAlgorithmManagerError::InvalidAlgorithmId);
                if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::calculateDiffAsync:: Algorithm ID is empty no selected algorithm";
                emit errorOccurred(QAlgorithmManagerError::InvalidAlgorithmId, errorMessage(QAlgorithmManagerError::InvalidAlgorithmId));
                QPromise<QDiffResult> promise;
                promise.start();
                promise.addResult(QDiffResult(errorMessage(QAlgorithmManagerError::InvalidAlgorithmId)));
                promise.finish();
                return promise.future();
            }
            algorithm = m_currentAlgorithm;
        }
        else {
            if(!isAlgorithmAvailable(algorithmId)) {
                setLastError(QAlgorithmManagerError::AlgorithmNotFound);
                if(m_errorOutputEnabled) qWarning() << "QAlgorithmManager::setCurrentAlgorithm:: Algorithm " << '"' << algorithmId << '"' << " is not Found ";
                emit errorOccurred(QAlgorithmManagerError::AlgorithmNotFound, errorMessage(QAlgorithmManagerError::AlgorithmNotFound));
                QPromise<QDiffResult> promise;
                promise.start();
                promise.addResult(QDiffResult(errorMessage(QAlgorithmManagerError::AlgorithmNotFound)));
                promise.finish();
                return promise.future();
            }
            algorithm = algorithmId;
        }
    }
    else {
        algorithm = autoSelectAlgorithm(leftText, rightText);
    }
    auto future =  QtConcurrent::run(&QAlgorithmManager::executeAlgorithm,
                                    this,
                                    algorithm,
                                    leftText,
                                    rightText);
    auto *watcher = new QFutureWatcher<QDiffResult>(this);
    connect(watcher, &QFutureWatcher<QDiffResult>::finished, this, [this, watcher]() {
        emit diffCalculated(watcher->result());
        watcher->deleteLater();
    });
    watcher->setFuture(future);
    return future;
}

QDiffResult QAlgorithmManager::calculateDiffSync(const QString &leftText, const QString &rightText, QAlgorithmSelectionMode selectionMode, QString algorithmId)
{
    QString algorithm;
    if (selectionMode == QAlgorithmSelectionMode::Manual) {
        if (algorithmId.isEmpty()) {
            if (m_currentAlgorithm.isEmpty()) {
                setLastError(QAlgorithmManagerError::InvalidAlgorithmId);
                if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::calculateDiffSync:: Algorithm ID is empty no selected algorithm";
                emit errorOccurred(QAlgorithmManagerError::InvalidAlgorithmId, errorMessage(QAlgorithmManagerError::InvalidAlgorithmId));
                return QDiffResult(errorMessage(QAlgorithmManagerError::InvalidAlgorithmId));
            }
            algorithm = m_currentAlgorithm;
        } else {
            if (!isAlgorithmAvailable(algorithmId)) {
                setLastError(QAlgorithmManagerError::AlgorithmNotFound);
                if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::calculateDiffSync:: Algorithm " << '"' << algorithmId << '"' << " is not Found ";
                emit errorOccurred(QAlgorithmManagerError::AlgorithmNotFound, errorMessage(QAlgorithmManagerError::AlgorithmNotFound));
                return QDiffResult(errorMessage(QAlgorithmManagerError::AlgorithmNotFound));
            }
            algorithm = algorithmId;
        }
    } else {
        algorithm = autoSelectAlgorithm(leftText, rightText);
    }
    QDiffResult result = executeAlgorithm(algorithm, leftText, rightText);
    if (result.success()) {
        emit diffCalculated(result);
    }
    return result;
}

QDiffResult QAlgorithmManager::calculateDiffWithAlgorithm(const QString &algorithmId, const QString &leftText, const QString &rightText)
{
    if (algorithmId.isEmpty()) {
        setLastError(QAlgorithmManagerError::InvalidAlgorithmId);
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::calculateDiffWithAlgorithm:: Algorithm ID is empty";
        emit errorOccurred(QAlgorithmManagerError::InvalidAlgorithmId, errorMessage(QAlgorithmManagerError::InvalidAlgorithmId));
        return QDiffResult(errorMessage(QAlgorithmManagerError::InvalidAlgorithmId));
    }
    if (!isAlgorithmAvailable(algorithmId)) {
        setLastError(QAlgorithmManagerError::AlgorithmNotFound);
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::calculateDiffWithAlgorithm:: Algorithm " << '"' << algorithmId << '"' << " is not Found ";
        emit errorOccurred(QAlgorithmManagerError::AlgorithmNotFound, errorMessage(QAlgorithmManagerError::AlgorithmNotFound));
        return QDiffResult(errorMessage(QAlgorithmManagerError::AlgorithmNotFound));
    }
    return executeAlgorithm(algorithmId, leftText, rightText);
}

bool QAlgorithmManager::isAlgorithmAvailable(const QString &algorithmId) const
{
    auto &registry = QAlgorithmRegistry::get_Instance();
    return registry.isAlgorithmAvailable(algorithmId);
}

QAlgorithmSelectionMode QAlgorithmManager::selectionMode() const
{
    return m_selectionMode;
}

void QAlgorithmManager::setSelectionMode(QAlgorithmSelectionMode newSelectionMode)
{
    if (m_selectionMode == newSelectionMode)
        return;
    m_selectionMode = newSelectionMode;
    emit selectionModeChanged();
}

QExecutionMode QAlgorithmManager::executionMode() const
{
    return m_executionMode;
}

void QAlgorithmManager::setExecutionMode(QExecutionMode newExecutionMode)
{
    if (m_executionMode == newExecutionMode)
        return;
    m_executionMode = newExecutionMode;
    emit executionModeChanged();
}

QString QAlgorithmManager::currentAlgorithm() const
{
    return m_currentAlgorithm;
}

void QAlgorithmManager::setCurrentAlgorithm(const QString &algorithmId)
{
    if (algorithmId.isEmpty()) {
        setLastError(QAlgorithmManagerError::InvalidAlgorithmId);
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::setCurrentAlgorithm:: Algorithm ID is empty";
        emit errorOccurred(QAlgorithmManagerError::InvalidAlgorithmId, errorMessage(QAlgorithmManagerError::InvalidAlgorithmId));
        return;
    }
    if (m_currentAlgorithm == algorithmId)
        return;
    if(!isAlgorithmAvailable(algorithmId)) {
        setLastError(QAlgorithmManagerError::AlgorithmNotFound);
        if(m_errorOutputEnabled) qWarning() << "QAlgorithmManager::setCurrentAlgorithm:: Algorithm " << '"' << algorithmId << '"' << " is not Found ";
        emit errorOccurred(QAlgorithmManagerError::AlgorithmNotFound, errorMessage(QAlgorithmManagerError::AlgorithmNotFound));
        return;
    }
    m_currentAlgorithm = algorithmId;
    emit currentAlgorithmChanged();
}

QString QAlgorithmManager::fallBackAlgorithm() const
{
    return m_fallBackAlgorithm;
}

void QAlgorithmManager::setFallBackAlgorithm(const QString &algorithmId)
{
    if (algorithmId.isEmpty()) {
        setLastError(QAlgorithmManagerError::InvalidAlgorithmId);
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::setFallBackAlgorithm:: Algorithm ID is empty";
        emit errorOccurred(QAlgorithmManagerError::InvalidAlgorithmId, errorMessage(QAlgorithmManagerError::InvalidAlgorithmId));
        return;
    }
    if (m_fallBackAlgorithm == algorithmId)
        return;
    if(!isAlgorithmAvailable(algorithmId)) {
        setLastError(QAlgorithmManagerError::AlgorithmNotFound);
        if(m_errorOutputEnabled) qWarning() << "QAlgorithmManager::setFallBackAlgorithm:: Algorithm " << '"' << algorithmId << '"' << " is not Found ";
        emit errorOccurred(QAlgorithmManagerError::AlgorithmNotFound, errorMessage(QAlgorithmManagerError::AlgorithmNotFound));
        return;
    }
    m_fallBackAlgorithm = algorithmId;
    emit fallBackAlgorithmChanged();
}

QString QAlgorithmManager::errorMessage(const QAlgorithmManagerError &error) const
{
    const QString context = QStringLiteral("QAlgorithmManager::");
    switch (error) {
    case QAlgorithmManagerError::None:
        return QString();
    case QAlgorithmManagerError::AlgorithmNotFound:
        return context + tr("Algorithm is not Found");
    case QAlgorithmManagerError::AlgorithmCreationFailed:
        return context + tr("Failed to create algorithm instance");
    case QAlgorithmManagerError::InvalidAlgorithmId:
        return context + tr("Invalid or empty algorithm ID");
    case QAlgorithmManagerError::DiffExecutionFailed:
        return context + tr("Algorithm execution failed");
    case QAlgorithmManagerError::ConfigurationError:
        return context + tr("Error applying configuration to algorithm");
    case QAlgorithmManagerError::Timeout:
        return context + tr("Diff operation timed out");
    case QAlgorithmManagerError::OperationCancelled:
        return context + tr("Operation was cancelled");
    case QAlgorithmManagerError::Unknown:
    default:
        return context + tr("Unknown error");
    }
}

QString QAlgorithmManager::lastErrorMessage() const
{
    return errorMessage(m_lastError);
}

bool QAlgorithmManager::errorOutputEnabled() const
{
    return m_errorOutputEnabled;
}

void QAlgorithmManager::setErrorOutputEnabled(bool newErrorOutputEnabled)
{
    m_errorOutputEnabled = newErrorOutputEnabled;
}

void QAlgorithmManager::setLastError(QAlgorithmManagerError newLastError)
{
    m_lastError = newLastError;
}

QString QAlgorithmManager::autoSelectAlgorithm(const QString& leftText, const QString& rightText) const
{
    const int threshold = 1000;
    int totalLength = leftText.length() + rightText.length();

    if (totalLength < threshold && isAlgorithmAvailable("dmp")) {
        return "dmp";
    }
    if (isAlgorithmAvailable("dtl")) {
        return "dtl";
    }
    if (!m_currentAlgorithm.isEmpty() && isAlgorithmAvailable(m_currentAlgorithm)) {
        return m_currentAlgorithm;
    }
    if (isAlgorithmAvailable(DEFAULT_ALGORITHM)) {
        return DEFAULT_ALGORITHM;
    }

    return QString();
}

bool QAlgorithmManager::isCalculating() const {
    return m_isCalculating;
}

QDiffResult QAlgorithmManager::executeAlgorithm(const QString& algorithmId, const QString& leftText, const QString& rightText)
{
    m_isCalculating = true;
    emit aboutToCalculateDiff(leftText, rightText, algorithmId);
    emit calculationStarted();
    QMutexLocker locker(&m_mutex);
    auto& registry = QAlgorithmRegistry::get_Instance();
    auto algorithm = registry.createAlgorithm(algorithmId);

    if (!algorithm) {
        auto regErrorMsg = registry.lastErrorMessage();
        setLastError(QAlgorithmManagerError::AlgorithmCreationFailed);
        QString msg = errorMessage(QAlgorithmManagerError::AlgorithmCreationFailed);
        if (!regErrorMsg.isEmpty())
            msg += ": " + regErrorMsg;
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::executeAlgorithm:: Failed to create algorithm instance for" << algorithmId << ", :" << regErrorMsg;
        emit errorOccurred(QAlgorithmManagerError::AlgorithmCreationFailed, msg);
        QDiffResult failResult(msg);
        m_isCalculating = false;
        emit calculationFinished(failResult);
        return failResult;
    }

    QDiffResult result = algorithm->calculateDiff(leftText, rightText);
    m_isCalculating = false;

    if (!result.success()) {
        setLastError(QAlgorithmManagerError::DiffExecutionFailed);
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmManager::executeAlgorithm:: Diff failed:" << result.errorMessage();
        emit errorOccurred(QAlgorithmManagerError::DiffExecutionFailed, result.errorMessage());
    } else {
        setLastError(QAlgorithmManagerError::None);
    }

    emit calculationFinished(result);
    return result;
}

void QAlgorithmManager::resetManager() {
    setSelectionMode(QDiffX::QAlgorithmSelectionMode::Auto);
    setExecutionMode(QDiffX::QExecutionMode::Synchronous);
    setCurrentAlgorithm(DEFAULT_ALGORITHM);
    setFallBackAlgorithm(DEFAULT_FALLBACK);
    setErrorOutputEnabled(false);
    setLastError(QAlgorithmManagerError::None);
    emit managerReset();
}

QMap<QString, QVariant> QAlgorithmManager::getAlgorithmConfiguration(const QString& algorithmId) const
{
    auto& registry = QAlgorithmRegistry::get_Instance();
    return registry.getAlgorithmConfiguration(algorithmId);
}

bool QAlgorithmManager::setAlgorithmConfiguration(const QString& algorithmId, const QMap<QString, QVariant>& config)
{
    auto& registry = QAlgorithmRegistry::get_Instance();
    bool success = registry.setAlgorithmConfiguration(algorithmId, config);
    if (success) {
        emit algorithmConfigurationChanged(algorithmId, config);
    } else {
        emit errorOccurred(QAlgorithmManagerError::ConfigurationError, "Failed to set configuration for " + algorithmId + ": " + registry.lastErrorMessage());
    }
    return success;
}

QStringList QAlgorithmManager::getAlgorithmConfigurationKeys(const QString& algorithmId) const
{
    auto& registry = QAlgorithmRegistry::get_Instance();
    return registry.getAlgorithmConfigurationKeys(algorithmId);
}

QStringList QAlgorithmManager::getAvailableAlgorithms() const {
    return QAlgorithmRegistry::get_Instance().getAvailableAlgorithms();
}

}//namespace QDiffX
