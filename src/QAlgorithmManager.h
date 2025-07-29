#pragma once
#include "QDiffAlgorithm.h"
#include "QAlgorithmRegistry.h"
#include "QAlgorithmManagerError.h"
#include <QFuture>



namespace QDiffX {




enum class QAlgorithmSelectionMode{
    Auto,
    Manual
};

enum class QExecutionMode{
    Asynchronous,
    Synchronous
};

class QAlgorithmManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAlgorithmSelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(QExecutionMode executionMode READ executionMode WRITE setExecutionMode NOTIFY executionModeChanged)
    Q_PROPERTY(QString currentAlgorithm READ currentAlgorithm WRITE setCurrentAlgorithm NOTIFY currentAlgorithmChanged)
    Q_PROPERTY(QString fallBackAlgorithm READ fallBackAlgorithm WRITE setFallBackAlgorithm NOTIFY fallBackAlgorithmChanged)
    Q_PROPERTY(bool errorOutputEnabled READ errorOutputEnabled WRITE setErrorOutputEnabled)
    Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage NOTIFY errorOccurred)
    Q_PROPERTY(QStringList availableAlgorithms READ getAvailableAlgorithms NOTIFY availableAlgorithmsChanged)
    Q_PROPERTY(bool isCalculating READ isCalculating NOTIFY calculationStarted)
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

    // Algorithm Configuration Management (Delegates to QAlgorithmRegistry)
    QMap<QString, QVariant> getAlgorithmConfiguration(const QString& algorithmId) const;
    bool setAlgorithmConfiguration(const QString& algorithmId, const QMap<QString, QVariant>& config);
    QStringList getAlgorithmConfigurationKeys(const QString& algorithmId) const;

    QStringList getAvailableAlgorithms() const;
    
    // Error Handeling
    QAlgorithmManagerError lastError() const { return m_lastError; }
    QString errorMessage(const QAlgorithmManagerError &error) const;
    QString lastErrorMessage() const;

    bool errorOutputEnabled() const;
    void setErrorOutputEnabled(bool newErrorOutputEnabled);

    bool isCalculating() const;

    void resetManager();

signals:
    void errorOccurred(QAlgorithmManagerError error, const QString& message);
    void currentAlgorithmChanged();
    void fallBackAlgorithmChanged();
    void selectionModeChanged();
    void executionModeChanged();
    void diffCalculated(const QDiffX::QDiffResult &result);
    void algorithmAvailabilityChanged(const QString& algorithmId, bool available);
    void availableAlgorithmsChanged(const QStringList& newList);
    void managerReset();
    void configurationChanged(const QString& key, const QVariant& value);
    void aboutToCalculateDiff(const QString& leftText, const QString& rightText, const QString& algorithmId);
    void calculationStarted();
    void calculationFinished(const QDiffX::QDiffResult& result);
    void algorithmConfigurationChanged(const QString& algorithmId, const QMap<QString, QVariant>& config);

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
    bool m_isCalculating = false;
};

}//namespace QDiffX
