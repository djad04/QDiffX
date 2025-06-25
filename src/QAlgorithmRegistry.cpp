#include "QAlgorithmRegistry.h"

namespace QDiffX{

QAlgorithmRegistry &QAlgorithmRegistry::get_Instance()
{
    static QAlgorithmRegistry instance;
    return instance;
}

bool QDiffX::QAlgorithmRegistry::registerAlgorithm(const QString &algorithmId, const QAlgorithmInfo &info)
{
    if (algorithmId.isEmpty()) {
        qWarning() << "QAlgorithmRegistry::registerAlgorithm: Empty algorithm ID provided";
        return false;
    }

    if (m_algorithms.contains(algorithmId)) {
        qWarning() << "QAlgorithmRegistry::registerAlgorithm: Algorithm already registered:" << algorithmId;
        return false;
    }

    if (!info.factory) {
        qWarning() << "QAlgorithmRegistry::registerAlgorithm: No factory function provided for algorithm:" << algorithmId;
        return false;
    }
    m_algorithms[algorithmId] = info;


    qDebug() << "QAlgorithmRegistry: Registered algorithm " << algorithmId << "(" << info.name << ")";
    return true;
}

bool QAlgorithmRegistry::unregisterAlgorithm(const QString &algorithmId)
{
    if (algorithmId.isEmpty()) {
        qWarning() << "QAlgorithmRegistry::unregisterAlgorithm: Empty algorithm ID provided";
        return false;
    }

    if (!m_algorithms.contains(algorithmId)) {
        qWarning() << "QAlgorithmRegistry::unregisterAlgorithm: Algorithm not registered:" << algorithmId;
        return false;
    }

    m_algorithms.remove(algorithmId);
    qDebug() << "QAlgorithmRegistry: unregistered algorithm" << algorithmId;
    return true;
}

QStringList QAlgorithmRegistry::getAvailableAlgorithms() const
{
    return m_algorithms.keys() ;
}

const QAlgorithmInfo* QAlgorithmRegistry::getAlgorithmInfo(const QString &algorithmId) const
{
    if (algorithmId.isEmpty()) {
        qWarning() << "QAlgorithmRegistry::getAlgorithmInfo: empty algorithm id provided";
        return nullptr;
    }

    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        qWarning() << "QAlgorithmRegistry::getAlgorithmInfo: algorithm not found:" << algorithmId;
        return nullptr;
    }

    return &it.value();
}

bool QAlgorithmRegistry::isAlgorithmAvailable(const QString &algorithmId) const
{
    if (algorithmId.isEmpty()) {
        qWarning() << "QAlgorithmRegistry::isAlgorithmAvailable: Empty algorithm ID provided";
        return false;
    }

    if(m_algorithms.contains(algorithmId))
        return true;
    else
        return false;
}

void QAlgorithmRegistry::clear()
{
    m_algorithms.clear();
}

int QAlgorithmRegistry::getAlgorithmCount()
{
    return m_algorithms.size();
}

}// namespace QDiffX
