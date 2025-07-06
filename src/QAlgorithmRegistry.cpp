#include "QAlgorithmRegistry.h"
#include "DTLAlgorithm.h"
#include "DMPAlgorithm.h"
#include <QMutexLocker>

namespace QDiffX{

QAlgorithmRegistry &QAlgorithmRegistry::get_Instance()
{
    static QAlgorithmRegistry instance;
    return instance;
}

QAlgorithmRegistry::QAlgorithmRegistry()
{
    initializeDefaultAlgorithms();
    // Initialize config maps for each algorithm with their default config
    for (auto it = m_algorithms.begin(); it != m_algorithms.end(); ++it) {
        if (it.value().factory) {
            std::unique_ptr<QDiffAlgorithm> algo = it.value().factory();
            if (algo) {
                m_algorithmConfigs[it.key()] = algo->getConfiguration();
            }
        }
    }
    setLastError(QAlgorithmRegistryError::None);
}

void QAlgorithmRegistry::initializeDefaultAlgorithms()
{
    DTLAlgorithm dtl;
    QAlgorithmInfo dtlInfo;
    dtlInfo.name = dtl.getName();
    dtlInfo.description = dtl.getDescription();
    dtlInfo.capabilities = dtl.getCapabilities();
    dtlInfo.factory = []() { return std::make_unique<DTLAlgorithm>(); };
    registerAlgorithm("dtl", dtlInfo);

    // Register DMP Algorithm
    DMPAlgorithm dmp;
    QAlgorithmInfo dmpInfo;
    dmpInfo.name = dmp.getName();
    dmpInfo.description = dmp.getDescription();
    dmpInfo.capabilities = dmp.getCapabilities();
    dmpInfo.factory = []() { return std::make_unique<DMPAlgorithm>(); };
    registerAlgorithm("dmp", dmpInfo);
}

bool QAlgorithmRegistry::registerAlgorithm(const QString &algorithmId, const QAlgorithmInfo &info)
{
    {
        QMutexLocker locker(&m_mutex);
        if (algorithmId.isEmpty()) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::registerAlgorithm: Empty algorithm ID provided";
            setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
            emit errorOccurred(QAlgorithmRegistryError::EmptyAlgorithmId, errorMessage(QAlgorithmRegistryError::EmptyAlgorithmId));
            return false;
        }
        if (m_algorithms.contains(algorithmId)) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::registerAlgorithm: Algorithm already registered:" << algorithmId;
            setLastError(QAlgorithmRegistryError::AlgorithmAlreadyRegistered);
            emit errorOccurred(QAlgorithmRegistryError::AlgorithmAlreadyRegistered, errorMessage(QAlgorithmRegistryError::AlgorithmAlreadyRegistered) + ": " + algorithmId);
            return false;
        }
        if (!info.factory) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::registerAlgorithm: No factory function provided for algorithm:" << algorithmId;
            setLastError(QAlgorithmRegistryError::InvalidFactory);
            emit errorOccurred(QAlgorithmRegistryError::InvalidFactory, errorMessage(QAlgorithmRegistryError::InvalidFactory) + ": " + algorithmId);
            return false;
        }
        m_algorithms[algorithmId] = info;
    }
    emit algorithmRegistered(algorithmId);
    emit algorithmAvailabilityChanged(algorithmId, true);
    emit algorithmsChanged(getAvailableAlgorithms());
    qDebug() << "QAlgorithmRegistry: Registered algorithm " << algorithmId << "(" << info.name << ")";
    setLastError(QAlgorithmRegistryError::None);
    return true;
}

bool QAlgorithmRegistry::unregisterAlgorithm(const QString &algorithmId)
{
    {
        QMutexLocker locker(&m_mutex);
        if (algorithmId.isEmpty()) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::unregisterAlgorithm: Empty algorithm ID provided";
            setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
            emit errorOccurred(QAlgorithmRegistryError::EmptyAlgorithmId, errorMessage(QAlgorithmRegistryError::EmptyAlgorithmId));
            return false;
        }
        if (!m_algorithms.contains(algorithmId)) {
            if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::unregisterAlgorithm: Algorithm not registered:" << algorithmId;
            setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
            emit errorOccurred(QAlgorithmRegistryError::AlgorithmNotFound, errorMessage(QAlgorithmRegistryError::AlgorithmNotFound) + ": " + algorithmId);
            return false;
        }
        m_algorithms.remove(algorithmId);
    }
    emit algorithmUnregistered(algorithmId);
    emit algorithmAvailabilityChanged(algorithmId, false);
    emit algorithmsChanged(getAvailableAlgorithms());
    qDebug() << "QAlgorithmRegistry: unregistered algorithm" << algorithmId;
    setLastError(QAlgorithmRegistryError::None);
    return true;
}

QStringList QAlgorithmRegistry::getAvailableAlgorithms() const
{
    QMutexLocker locker(&m_mutex);
    setLastError(QAlgorithmRegistryError::None);
    return m_algorithms.keys();
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
    setLastError(QAlgorithmRegistryError::None);
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
    bool available = m_algorithms.contains(algorithmId);
    setLastError(QAlgorithmRegistryError::None);
    return available;
}

void QAlgorithmRegistry::clear()
{
    QMutexLocker locker(&m_mutex);
    m_algorithms.clear();
    emit registryCleared();
    emit algorithmsChanged(QStringList());
    setLastError(QAlgorithmRegistryError::None);
}

int QAlgorithmRegistry::getAlgorithmCount()
{
    QMutexLocker locker(&m_mutex);
    setLastError(QAlgorithmRegistryError::None);
    return m_algorithms.size();
}

QString QAlgorithmRegistry::getAlgorithmName(const QString &algorithmId) const
{
    QMutexLocker locker(&m_mutex);
    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmName: Empty algorithm ID provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        return QString();
    }
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmName: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        return QString();
    }
    setLastError(QAlgorithmRegistryError::None);
    return it.value().name;
}

QString QAlgorithmRegistry::getAlgorithmDescription(const QString &algorithmId) const
{
    QMutexLocker locker(&m_mutex);
    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmDescription: Empty algorithm ID provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        return QString();
    }
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmDescription: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        return QString();
    }
    setLastError(QAlgorithmRegistryError::None);
    return it.value().description;
}

AlgorithmCapabilities QAlgorithmRegistry::getAlgorithmCapabilities(const QString &algorithmId) const
{
    QMutexLocker locker(&m_mutex);
    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmCapabilities: Empty algorithm ID provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        return AlgorithmCapabilities();
    }
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmCapabilities: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        return AlgorithmCapabilities();
    }
    setLastError(QAlgorithmRegistryError::None);
    return it.value().capabilities;
}

QMap<QString, QVariant> QAlgorithmRegistry::getAlgorithmConfiguration(const QString &algorithmId) const
{
    QMutexLocker locker(&m_mutex);
    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmConfiguration: Empty algorithm ID provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        return QMap<QString, QVariant>();
    }
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmConfiguration: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        return QMap<QString, QVariant>();
    }
    // Return stored config if available
    if (m_algorithmConfigs.contains(algorithmId)) {
        setLastError(QAlgorithmRegistryError::None);
        return m_algorithmConfigs[algorithmId];
    }
    // Fallback: get from default instance
    if (it.value().factory) {
        std::unique_ptr<QDiffAlgorithm> algo = it.value().factory();
        if (algo) {
            setLastError(QAlgorithmRegistryError::None);
            return algo->getConfiguration();
        }
    }
    setLastError(QAlgorithmRegistryError::None);
    return QMap<QString, QVariant>();
}

bool QAlgorithmRegistry::setAlgorithmConfiguration(const QString &algorithmId, const QMap<QString, QVariant> &config)
{
    QMutexLocker locker(&m_mutex);
    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::setAlgorithmConfiguration: Empty algorithm ID provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        emit errorOccurred(QAlgorithmRegistryError::EmptyAlgorithmId, errorMessage(QAlgorithmRegistryError::EmptyAlgorithmId));
        return false;
    }
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::setAlgorithmConfiguration: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        emit errorOccurred(QAlgorithmRegistryError::AlgorithmNotFound, errorMessage(QAlgorithmRegistryError::AlgorithmNotFound) + ": " + algorithmId);
        return false;
    }
    m_algorithmConfigs[algorithmId] = config;
    emit algorithmConfigurationChanged(algorithmId, config);
    setLastError(QAlgorithmRegistryError::None);
    return true;
}

std::unique_ptr<QDiffAlgorithm> QAlgorithmRegistry::createAlgorithm(const QString &algorithmId)
{
    QMutexLocker locker(&m_mutex);
    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::createAlgorithm: Empty algorithm ID provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        emit errorOccurred(QAlgorithmRegistryError::EmptyAlgorithmId, errorMessage(QAlgorithmRegistryError::EmptyAlgorithmId));
        return nullptr;
    }
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::createAlgorithm: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        emit errorOccurred(QAlgorithmRegistryError::AlgorithmNotFound, errorMessage(QAlgorithmRegistryError::AlgorithmNotFound) + ": " + algorithmId);
        return nullptr;
    }
    if (!it.value().factory) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::createAlgorithm: No factory for algorithm:" << algorithmId;
        setLastError(QAlgorithmRegistryError::InvalidFactory);
        emit errorOccurred(QAlgorithmRegistryError::InvalidFactory, errorMessage(QAlgorithmRegistryError::InvalidFactory) + ": " + algorithmId);
        return nullptr;
    }
    std::unique_ptr<QDiffAlgorithm> algo = it.value().factory();
    if (!algo) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::createAlgorithm: Factory creation failed for algorithm:" << algorithmId;
        setLastError(QAlgorithmRegistryError::FactoryCreationFailed);
        emit errorOccurred(QAlgorithmRegistryError::FactoryCreationFailed, errorMessage(QAlgorithmRegistryError::FactoryCreationFailed) + ": " + algorithmId);
        return nullptr;
    }
    if (m_algorithmConfigs.contains(algorithmId)) {
        algo->setConfiguration(m_algorithmConfigs[algorithmId]);
    }
    setLastError(QAlgorithmRegistryError::None);
    return algo;
}

QStringList QAlgorithmRegistry::getAlgorithmConfigurationKeys(const QString& algorithmId) const
{
    QMutexLocker locker(&m_mutex);
    if (algorithmId.isEmpty()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmConfigurationKeys: Empty algorithm ID provided";
        setLastError(QAlgorithmRegistryError::EmptyAlgorithmId);
        return QStringList();
    }
    auto it = m_algorithms.find(algorithmId);
    if (it == m_algorithms.end()) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmConfigurationKeys: algorithm not found:" << algorithmId;
        setLastError(QAlgorithmRegistryError::AlgorithmNotFound);
        return QStringList();
    }
    if (!it.value().factory) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmConfigurationKeys: No factory for algorithm:" << algorithmId;
        setLastError(QAlgorithmRegistryError::InvalidFactory);
        return QStringList();
    }
    std::unique_ptr<QDiffAlgorithm> algo = it.value().factory();
    if (!algo) {
        if (m_errorOutputEnabled) qWarning() << "QAlgorithmRegistry::getAlgorithmConfigurationKeys: Factory creation failed for algorithm:" << algorithmId;
        setLastError(QAlgorithmRegistryError::FactoryCreationFailed);
        return QStringList();
    }
    setLastError(QAlgorithmRegistryError::None);
    return algo->getConfigurationKeys();
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
