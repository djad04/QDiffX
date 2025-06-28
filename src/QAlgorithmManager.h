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
    AlgorithmNotFound,
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
    ~QAlgorithmManager() = default;


    bool isAlgorithmAvailable(const QString &algorithmId) const;

    AlgorithmSelectionMode selectionMode() const;
    void setSelectionMode(AlgorithmSelectionMode newSelectionMode);
    ExecutionMode executionMode() const;
    void setExecutionMode(ExecutionMode newExecutionMode);
    QString currentAlgorithm() const;
    void setCurrentAlgorithm(const QString &newCurrentAlgorithm);
    QString fallBackAlgorithm() const;
    void setFallBackAlgorithm(const QString &newFallBackAlgorithm);

    // Error Handeling
    QAlgorithmManagerError lastError() const { return m_lastError; }
    QString errorMessage(const QAlgorithmManagerError &error) const;
    QString lastErrorMessage() const;

    bool errorOutputEnabled() const;
    void setErrorOutputEnabled(bool newErrorOutputEnabled);

signals:
    void errorOccurred(QAlgorithmManagerError error, const QString& message);
    void currentAlgorithmChanged();
    void fallBackAlgorithmChanged();
    void selectionModeChanged();
    void executionModeChanged();

private:
    void setLastError(QAlgorithmManagerError newLastError);

private:
    AlgorithmSelectionMode m_selectionMode;
    ExecutionMode m_executionMode;
    QString m_currentAlgorithm;
    QString m_fallBackAlgorithm;

    // Default algorithms
    static const QString DEFAULT_ALGORITHM;
    static const QString DEFAULT_FALLBACK;

    QAlgorithmManagerError m_lastError;
    bool m_errorOutputEnabled = false;
};

}//namespace QDiffX
#endif // QALGORITHMMANAGER_H
