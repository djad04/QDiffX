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
    QAlgorithmManager();
    ~QAlgorithmManager();


private:

};

}//namespace QDiffX
#endif // QALGORITHMMANAGER_H
