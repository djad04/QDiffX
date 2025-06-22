#include "DTLAlgorithm.h"

namespace QDiffX {

// Configuration keys
const QString DTLAlgorithm::CONFIG_LARGE_FILE_THRESHOLD = "large_file_threshold";
const QString DTLAlgorithm::CONFIG_ENABLE_OPTIMIZATION = "enable_optimization";
const QString DTLAlgorithm::CONFIG_MAX_DIFF_SIZE = "max_diff_size";
const QString DTLAlgorithm::CONFIG_ENABLE_HEURISTICS = "enable_heuristics";

DTLAlgorithm::DTLAlgorithm()
{
    // Set default configuration for DTL algorithm
    // DTL is optimized for large files and line-based comparisons
    QMap<QString, QVariant> defaultConfig;
    defaultConfig[CONFIG_LARGE_FILE_THRESHOLD] = 1024 * 1024; // 1MB threshold
    defaultConfig[CONFIG_ENABLE_OPTIMIZATION] = true;
    defaultConfig[CONFIG_MAX_DIFF_SIZE] = 10 * 1024 * 1024; // 10MB max
    defaultConfig[CONFIG_ENABLE_HEURISTICS] = true;
    
    setConfiguration(defaultConfig);
}

AlgorithmCapabilities DTLAlgorithm::getCapabilities() const
{
    AlgorithmCapabilities caps;
    caps.supportsLargeFiles = true;        // DTL excels with large files
    caps.supportsUnicode = true;
    caps.supportsBinary = false;           // Text-based algorithm
    caps.supportsLineByLine = true;        // Primary strength
    caps.supportsCharByChar = true;        // Also supported
    caps.supportsWordByWord = false;       // Not implemented
    caps.maxRecommendedSize = 10 * 1024 * 1024; // 10MB
    caps.description = "High-performance DTL (Diff Template Library) algorithm optimized for large files and line-based comparisons";
    
    return caps;
}



} // namespace QDiffX
