#ifndef QALGORITHMREGISTRY_H
#define QALGORITHMREGISTRY_H
#include "QDiffAlgorithm.h"

// follows singleton pattern
class QAlgorithmRegistry
{
public:
 static QAlgorithmRegistry& get_Instance();

private:
    QAlgorithmRegistry() = default;
    ~QAlgorithmRegistry() = default;

    QAlgorithmRegistry(const QAlgorithmRegistry &) = delete;
    QAlgorithmRegistry(const QAlgorithmRegistry &&) = delete;
    QAlgorithmRegistry operator=(const QAlgorithmRegistry &) = delete;
    QAlgorithmRegistry operator=(const QAlgorithmRegistry &&) = delete;

};

#endif // QALGORITHMREGISTRY_H
