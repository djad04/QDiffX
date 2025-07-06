#pragma once

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
