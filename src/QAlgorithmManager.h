#pragma once
#include "QDiffAlgorithm.h"
#include "QAlgorithmRegistry.h"
#include "QAlgorithmManagerError.h"
#include <QFuture>
/*
 TODO:
-calculateDiffSync
-calculateDiffWithAlgorithm
-calculateDiff
*/


namespace QDiffX {




enum class QAlgorithmSelectionMode{
    Auto,
    Manual
};

enum class QExecutionMode{
    Asynchronous,
    Synchronous
};

class QAlgorithmManager : QObject
{
    Q_OBJECT
public:
    QAlgorithmManager(QObject *parent = nullptr);
    ~QAlgorithmManager() = default;

    // Diff Functions:
    QFuture<QDiffResult> calculateDiff(const QString &leftText, const QString &rightText,
                                       QExecutionMode executionMode = QExecutionMode::Asynchronous,
                                       QAlgorithmSelectionMode selectionMode = QAlgorithmSelectionMode::Auto,
                                       QString algorithmId = QString());

    QFuture<QDiffResult> calculateDiffAsync(const QString &leftText, const QString &rightText,
                                            QAlgorithmSelectionMode selectionMode = QAlgorithmSelectionMode::Auto,
                                            QString algorithmId = QString());

    QDiffResult calculateDiffSync(const QString &leftText, const QString &rightText,
                                  QAlgorithmSelectionMode selectionMode = QAlgorithmSelectionMode::Auto,
                                  QString algorithmId = QString());

    QDiffResult calculateDiffWithAlgorithm(const QString& algorithmId,
                                           const QString& leftText,
                                           const QString& rightText);


    bool isAlgorithmAvailable(const QString &algorithmId) const;

    QAlgorithmSelectionMode selectionMode() const;
    void setSelectionMode(QAlgorithmSelectionMode newSelectionMode);
    QExecutionMode executionMode() const;
    void setExecutionMode(QExecutionMode newExecutionMode);
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
    void diffCalculated(const QDiffX::QDiffResult &result);

private:
    void setLastError(QAlgorithmManagerError newLastError);
    QDiffResult executeAlgorithm(const QString& algorithmId,
                          const QString& leftText,
                          const QString& rightText);
QString autoSelectAlgorithm (const QString& leftText,
                             const QString& rightText) const;
private:
    QAlgorithmSelectionMode m_selectionMode;
    QExecutionMode m_executionMode;
    QString m_currentAlgorithm;
    QString m_fallBackAlgorithm;
    mutable QMutex m_mutex;

    // Default algorithms
    static const QString DEFAULT_ALGORITHM;
    static const QString DEFAULT_FALLBACK;

    QAlgorithmManagerError m_lastError;
    bool m_errorOutputEnabled = false;
};

}//namespace QDiffX
