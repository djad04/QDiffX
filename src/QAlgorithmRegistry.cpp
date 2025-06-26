#include "QAlgorithmRegistry.h"
#include <QMutexLocker>

namespace QDiffX{

QAlgorithmRegistry &QAlgorithmRegistry::get_Instance()
{
    static QAlgorithmRegistry instance;
    return instance;
}

bool QDiffX::QAlgorithmRegistry::registerAlgorithm(const QString &algorithmId, const QAlgorithmInfo &info)
{
    {
        QMutexLocker locker(&m_mutex);

        if (algorithmId.isEmpty()) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::registerAlgorithm: Empty algorithm ID provided";
            setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
            return false;
        }

        if (m_algorithms.contains(algorithmId)) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::registerAlgorithm: Algorithm already registered:" << algorithmId;
            setLastError(QAlgorithmRegistryError::AlgorithmAlreadyRegistered);
            return false;
        }

        if (!info.factory) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::registerAlgorithm: No factory function provided for algorithm:" << algorithmId;
            setLastError(QAlgorithmRegistryError::InvalidFactory);
            return false;
        }
        m_algorithms[algorithmId] = info;
    }

    emit algorithmRegistered(algorithmId);
    qDebug() << "QAlgorithmRegistry: Registered algorithm " << algorithmId << "(" << info.name << ")";
    return true;
}

bool QAlgorithmRegistry::unregisterAlgorithm(const QString &algorithmId)
{
    {
        QMutexLocker locker(&m_mutex);

        if (algorithmId.isEmpty()) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::unregisterAlgorithm: Empty algorithm ID provided";
            setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
            return false;
        }

        if (!m_algorithms.contains(algorithmId)) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::unregisterAlgorithm: Algorithm not registered:" << algorithmId;
            setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
            return false;
        }

        m_algorithms.remove(algorithmId);
    }

    emit  algorithmUnregistered(algorithmId) ;
    qDebug() << "QAlgorithmRegistry: unregistered algorithm" << algorithmId;
    return true;
}

QStringList QAlgorithmRegistry::getAvailableAlgorithms() const
{
    QMutexLocker locker(&m_mutex);
    return m_algorithms.keys() ;
}

std::optional<QAlgorithmInfo> QAlgorithmRegistry::getAlgorithmInfo(const QString &algorithmId) const
{
    QMutexLocker locker(&m_mutex);

    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmInfo: empty algorithm id provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        return std::nullopt;
    }

    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmInfo: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        return std::nullopt;
    }

    return it.value();
}

bool QAlgorithmRegistry::isAlgorithmAvailable(const QString &algorithmId) const
{
    QMutexLocker locker(&m_mutex);

    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::isAlgorithmAvailable: Empty algorithm ID provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        return false;
    }

    if(m_algorithms.contains(algorithmId))
        return true;
    else
        return false;
}

void QAlgorithmRegistry::clear()
{
    QMutexLocker locker(&m_mutex);
    m_algorithms.clear();
}

int QAlgorithmRegistry::getAlgorithmCount()
{
    QMutexLocker locker(&m_mutex);
    return m_algorithms.size();
}

std::unique_ptr<QDiffAlgorithm> QAlgorithmRegistry::createAlgorithm(const QString &algorithmId) const
{
    QMutexLocker locker(&m_mutex);

    auto it = m_algorithms.find(algorithmId);
    if(it == m_algorithms.end()){
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::createAlgorithm: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        return nullptr;
    }
    return it->factory();
}

QString QAlgorithmRegistry::errorMessage(const QAlgorithmRegistryError &error) const
{
    QString context = QStringLiteral("QAlgorithmRegistry: ");
    switch (error) {
    case QAlgorithmRegistryError::None:
        return QString();

    case QAlgorithmRegistryError::EmptyAlgorithmId:
        return context + tr("Algorithm ID cannot be empty");

    case QAlgorithmRegistryError::AlgorithmAlreadyRegistered:
        return context + tr("Algorithm is already registered");

    case QAlgorithmRegistryError::AlgorithmNotFound:
        return context + tr("Algorithm not found");

    case QAlgorithmRegistryError::InvalidFactory:
        return context + tr("Invalid or missing factory function");

    case QAlgorithmRegistryError::FactoryCreationFailed:
        return context + tr("Failed to create algorithm instance");

    default:
        return context + tr("Unknown error");
    }
}

QString QAlgorithmRegistry::lastErrorMessage() const
{
     QMutexLocker locker(&m_mutex);
    return errorMessage(m_lastError);
}

}// namespace QDiffX
