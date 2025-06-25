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
        qWarning() << "Cannot register algorithm with empty ID ";
        return false;
    }
    if(m_algorithms.contains(algorithmId)) {
        qWarning() << "algorithm already Regestred \n" ;
        return false;
    }
    if(!info.factory) {
        qWarning() << "cant register an algorithm without a factory function ";
    }
    m_algorithms[algorithmId] = info;


    qDebug() << "Registered algorithm:" << algorithmId << "(" << info.name << ")";
    return true;
}

bool QAlgorithmRegistry::unregisterAlgorithm(const QString &algorithmId)
{
    if(!m_algorithms.contains(algorithmId)) {
        qWarning() << "cant unregister a non registered algorithm";
        return false;
    }

    m_algorithms.remove(algorithmId) ;
    qDebug() << "Unregistered algorithm:" << algorithmId;
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
        qWarning() << "QAlgorithmRegistry::getAlgorithmInfo: empty algorithm id provided";
        return false;
    }

    if(m_algorithms.contains(algorithmId))
        return true;
    else
        return false;
}

}// namespace QDiffX
