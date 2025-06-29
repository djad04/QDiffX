#ifndef QALGORITHMMANAGERERROR_H
#define QALGORITHMMANAGERERROR_H

namespace QDiffX{


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

}//namespace QDiffX
#endif // QALGORITHMMANAGERERROR_H
