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
    QAlgorithmRegistry() = default;
    ~QAlgorithmRegistry() = default;

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
