#pragma once
#include "QDiffAlgorithm.h"
#include <QMutex>

namespace QDiffX{

enum class QAlgorithmRegistryError {
    None,
    EmptyAlgorithmId,
    AlgorithmAlreadyRegistered,
    AlgorithmNotFound,
    InvalidFactory,
    FactoryCreationFailed
};
using QAlgorithmFactory = std::function<std::unique_ptr<QDiffAlgorithm>()>;

struct QAlgorithmInfo{
    QString name;
    QString description;
    AlgorithmCapabilities capabilities;
    QAlgorithmFactory factory;

    QAlgorithmInfo(){}
    QAlgorithmInfo(QString name, QString description,
                   AlgorithmCapabilities capabilities,
                   QAlgorithmFactory factory)
        : name(std::move(name)), description(std::move(description)),
        capabilities(std::move(capabilities)), factory(std::move(factory)) {}
};

// follows singleton pattern
class QAlgorithmRegistry : public QObject
{
    Q_OBJECT
public:
    static QAlgorithmRegistry& get_Instance();

    bool registerAlgorithm(const QString &algorithmId, const QAlgorithmInfo &info);
    bool unregisterAlgorithm(const QString &algorithmId);
    std::unique_ptr<QDiffAlgorithm> createAlgorithm(const QString& algorithmId) const;

    QStringList getAvailableAlgorithms() const;
    std::optional<QAlgorithmInfo> getAlgorithmInfo(const QString &algorithmId) const;
    bool isAlgorithmAvailable(const QString &algorithmId) const;

    void clear();
    int getAlgorithmCount();

    template<typename AlgorithmType>
    bool registerAlgorithm(const QString &algorithmId) {
        auto temp = std::make_unique<AlgorithmType>();

        QAlgorithmInfo info;
        info.name = temp.getName();
        info.description = temp.getDescription();
        info.capabilities = temp.getCapabilities();
        info.factory = []() -> std::unique_ptr<QDiffAlgorithm> {
            return std::make_unique<AlgorithmType>();
        };

        return registerAlgorithm(algorithmId, info);
    }

    // Algorithm Functions:
    QString getAlgorithmName(const QString& algorithmId) const;
    QString getAlgorithmDescription(const QString& algorithmId) const;
    AlgorithmCapabilities getAlgorithmCapabilities(const QString& algorithmId) const;

    QMap<QString, QVariant> getAlgorithmConfiguration(const QString& algorithmId) const;
    bool setAlgorithmConfiguration(const QString& algorithmId, const QMap<QString, QVariant>& config);
    QStringList getAlgorithmConfigurationKeys(const QString& algorithmId) const;

    // Error Handeling:
    QAlgorithmRegistryError lastError() const {  QMutexLocker locker(&m_mutex); return m_lastError; }
    QString errorMessage(const QAlgorithmRegistryError &error) const;
    QString lastErrorMessage() const;

    void setErrorOutputEnabled(bool enabled) {
        QMutexLocker locker(&m_mutex);
        m_errorOutputEnabled = enabled;
    }
    bool isErrorOutputEnabled() const {
        QMutexLocker locker(&m_mutex);
        return m_errorOutputEnabled;
    }



signals:
    void algorithmRegistered(const QString& algorithmId);
    void algorithmUnregistered(const QString& algorithmId);

private:
    QAlgorithmRegistry();
    ~QAlgorithmRegistry() = default;

    // Init Function to Register all default algotithms (note: to integrate additional algorithms add them here)
    void initializeDefaultAlgorithms();

    QAlgorithmRegistry(const QAlgorithmRegistry &) = delete;
    QAlgorithmRegistry(const QAlgorithmRegistry &&) = delete;
    QAlgorithmRegistry operator=(const QAlgorithmRegistry &) = delete;
    QAlgorithmRegistry operator=(const QAlgorithmRegistry &&) = delete;

    void setLastError(QAlgorithmRegistryError error) const { m_lastError = error; }

private:
    QMap<QString, QAlgorithmInfo> m_algorithms;
    mutable QMutex m_mutex;
    mutable QAlgorithmRegistryError m_lastError = QAlgorithmRegistryError::None;
    bool m_errorOutputEnabled = false;
};

}// namespace QDiffX
