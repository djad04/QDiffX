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

}// namespace QDiffX
