#ifndef QALGORITHMMANAGER_H
#define QALGORITHMMANAGER_H
#include "QDiffAlgorithm.h"
#include "QAlgorithmRegistry.h"

namespace QDiffX {

enum class AlgorithmSelectionMode{
    Auto,
    DMP,
    DTL
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
