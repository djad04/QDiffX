#include "QAlgorithmManager.h"

namespace QDiffX{
const QString QAlgorithmManager::DEFAULT_ALGORITHM = "dtl";
const QString QAlgorithmManager::DEFAULT_FALLBACK = "dmp";


QAlgorithmManager::QAlgorithmManager(QObject *parent)
    : QObject(parent),
    m_currentAlgorithm(DEFAULT_ALGORITHM),
    m_fallBackAlgorithm(DEFAULT_FALLBACK),
    m_selectionMode(AlgorithmSelectionMode::Auto),
    m_executionMode(ExecutionMode::Synchronous),
    m_lastError(QAlgorithmManagerError::None)
{

}

bool QAlgorithmManager::isAlgorithmAvailable(const QString &algorithmId) const
{
    auto &registry = QAlgorithmRegistry::get_Instance();
    return registry.isAlgorithmAvailable(algorithmId);
}

AlgorithmSelectionMode QAlgorithmManager::selectionMode() const
{
    return m_selectionMode;
}

void QAlgorithmManager::setSelectionMode(AlgorithmSelectionMode newSelectionMode)
{
    if (m_selectionMode == newSelectionMode)
        return;
    m_selectionMode = newSelectionMode;
    emit selectionModeChanged();
}

ExecutionMode QAlgorithmManager::executionMode() const
{
    return m_executionMode;
}

void QAlgorithmManager::setExecutionMode(ExecutionMode newExecutionMode)
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



}//namespace QDiffX
