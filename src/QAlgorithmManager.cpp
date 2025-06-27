#include "QAlgorithmManager.h"

namespace QDiffX{
const QString QAlgorithmManager::DEFAULT_ALGORITHM = "dtl";
const QString QAlgorithmManager::DEFAULT_FALLBACK = "dmp";


QAlgorithmManager::QAlgorithmManager(QObject *parent)
    : QObject(parent),
    m_currentAlgorithm(DEFAULT_ALGORITHM),
    m_fallBackAlgorithm(DEFAULT_FALLBACK),
    m_selectionMode(AlgorithmSelectionMode::Auto),
    m_executionMode(ExecutionMode::Synchronous)
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
    if (m_currentAlgorithm == algorithmId)
        return;
    if(!isAlgorithmAvailable(algorithmId)) {
        //TODO: set error when the error system is implemented
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
    if (m_fallBackAlgorithm == algorithmId)
        return;
    if(!isAlgorithmAvailable(algorithmId)) {
        //TODO: set error when the error system is implemented
        return;
    }
    m_fallBackAlgorithm = algorithmId;
    emit fallBackAlgorithmChanged();
}



}//namespace QDiffX
