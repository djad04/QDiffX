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



}//namespace QDiffX
