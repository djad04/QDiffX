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

void DTLAlgorithm::setConfiguration(const QMap<QString, QVariant> &newConfig)
{
    QDiffAlgorithm::setConfiguration(newConfig);

    // DTL-specific configuration handling
    // Configuration is stored in base class and used during diff calculation
}

QStringList DTLAlgorithm::getConfigurationKeys() const
{
    return {
        CONFIG_LARGE_FILE_THRESHOLD,
        CONFIG_ENABLE_OPTIMIZATION,
        CONFIG_MAX_DIFF_SIZE,
        CONFIG_ENABLE_HEURISTICS
    };
}

int DTLAlgorithm::estimateComplexity(const QString &leftText, const QString &rightText) const
{
    // TODO: requires Benchmark
    return 0;
}

bool DTLAlgorithm::isRecommendedFor(const QString &leftText, const QString &rightText) const
{
    // TODO: will be implemented Later
    return 0;
}

DiffOperation DTLAlgorithm::convertDTLOperation(dtl::edit_t dtlOp) const
{
    switch (dtlOp) {
    case dtl::SES_ADD:
        return DiffOperation::Insert;
    case dtl::SES_DELETE:
        return DiffOperation::Delete;
    case dtl::SES_COMMON:
        return DiffOperation::Equal;
    default:
        return DiffOperation::Equal;
    }
}

QStringList DTLAlgorithm::splitIntoLines(const QString &text) const
{
    if (text.isEmpty()) {
        return QStringList();
    }

    QStringList lines = text.split('\n');

    // DTL requires non-empty sequences, so ensure we have at least one element
    if (lines.isEmpty()) {
        lines.append(QString());
    }

    return lines;
}

QString DTLAlgorithm::joinLines(const QStringList &lines) const
{
    return lines.join('\n');
}

QList<DiffChange> DTLAlgorithm::diffLineByLine(const QString &leftFile, const QString &rightFile)
{
    // Split texts into lines for DTL processing
    QStringList leftLines = splitIntoLines(leftFile);
    QStringList rightLines = splitIntoLines(rightFile);

    // Convert to std::vector for DTL
    std::vector<QString> leftVec(leftLines.begin(), leftLines.end());
    std::vector<QString> rightVec(rightLines.begin(), rightLines.end());

    // Create DTL diff object and calculate differences
    dtl::Diff<QString> dtlDiff(leftVec, rightVec);
    dtlDiff.compose();

    // Convert DTL result to QDiffX format
    return convertDTLSequence(dtlDiff);
}

QList<DiffChange> DTLAlgorithm::convertDTLSequence(const dtl::Diff<QString> &dtlDiff) const
{
    QList<DiffChange> changes;
    auto ses = dtlDiff.getSes();
    int position = 0;

    for (const auto &edit : ses.getSequence()) {
        DiffChange change;
        change.operation = convertDTLOperation(edit.second.type);
        change.text = edit.first;
        change.lineNumber = -1; // Will be calculated later
        change.position = position;

        changes.append(change);

        // Update position (don't advance for deletions)
        if (edit.second.type != dtl::SES_DELETE) {
            position += edit.first.length();
        }
    }

    return changes;
}

} // namespace QDiffX
