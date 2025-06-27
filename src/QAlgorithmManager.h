#ifndef QALGORITHMMANAGER_H
#define QALGORITHMMANAGER_H
#include "QDiffAlgorithm.h"
#include "QAlgorithmRegistry.h"

/*
TODO:
- function setCurrentAlgorithm and setFallbackAlgorithm require error
handeling integration once the error system is implemented
*/
enum class QAlgorithmManagerError {
    None,
    AlgorithmNotAvailable,
    AlgorithmCreationFailed,
    InvalidAlgorithmId,
    DiffExecutionFailed,
    ConfigurationError,
    Timeout,
    OperationCancelled,
    Unknown
};

namespace QDiffX {

enum class AlgorithmSelectionMode{
    Auto,
    Manual
};

enum class ExecutionMode{
    Asynchronous,
    Synchronous
};

class QAlgorithmManager : QObject
{
    Q_OBJECT
public:
    QAlgorithmManager(QObject *parent = nullptr);
    ~QAlgorithmManager();


    bool isAlgorithmAvailable(const QString &algorithmId) const;

    AlgorithmSelectionMode selectionMode() const;
    void setSelectionMode(AlgorithmSelectionMode newSelectionMode);
    ExecutionMode executionMode() const;
    void setExecutionMode(ExecutionMode newExecutionMode);
    QString currentAlgorithm() const;
    void setCurrentAlgorithm(const QString &newCurrentAlgorithm);
    QString fallBackAlgorithm() const;
    void setFallBackAlgorithm(const QString &newFallBackAlgorithm);

signals:
    void selectionModeChanged();
    void executionModeChanged();
    void currentAlgorithmChanged();
    void fallBackAlgorithmChanged();

private:
    AlgorithmSelectionMode m_selectionMode;
    ExecutionMode m_executionMode;
    QString m_currentAlgorithm;
    QString m_fallBackAlgorithm;

    // Default algorithms
    static const QString DEFAULT_ALGORITHM;
    static const QString DEFAULT_FALLBACK;
};

}//namespace QDiffX
#endif // QALGORITHMMANAGER_H
