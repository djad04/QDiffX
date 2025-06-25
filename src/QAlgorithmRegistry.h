#ifndef QALGORITHMREGISTRY_H
#define QALGORITHMREGISTRY_H
#include "QDiffAlgorithm.h"

namespace QDiffX{

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
class QAlgorithmRegistry
{
public:
    static QAlgorithmRegistry& get_Instance();

    bool registerAlgorithm(const QString &algorithmId, const QAlgorithmInfo &info);
    bool unregisterAlgorithm(const QString &algorithmId);

    QStringList getAvailableAlgorithms() const;
    const QAlgorithmInfo* getAlgorithmInfo(const QString &algorithmId) const;
    bool isAlgorithmAvailable(const QString &algorithmId) const;

    void clear();
    int getAlgorithmCount();

    std::unique_ptr<QDiffAlgorithm> createAlgorithm(const QString& algorithmId) const;

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


private:
    QAlgorithmRegistry() = default;
    ~QAlgorithmRegistry() = default;

    QAlgorithmRegistry(const QAlgorithmRegistry &) = delete;
    QAlgorithmRegistry(const QAlgorithmRegistry &&) = delete;
    QAlgorithmRegistry operator=(const QAlgorithmRegistry &) = delete;
    QAlgorithmRegistry operator=(const QAlgorithmRegistry &&) = delete;

private:
    QMap<QString, QAlgorithmInfo> m_algorithms;
};

}// namespace QDiffX
#endif // QALGORITHMREGISTRY_H
