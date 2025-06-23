#include "DTLAlgorithm.h"

namespace QDiffX {


const QString DTLAlgorithm::CONFIG_LARGE_FILE_THRESHOLD = "large_file_threshold";
const QString DTLAlgorithm::CONFIG_ENABLE_OPTIMIZATION = "enable_optimization";
const QString DTLAlgorithm::CONFIG_MAX_DIFF_SIZE = "max_diff_size";
const QString DTLAlgorithm::CONFIG_ENABLE_HEURISTICS = "enable_heuristics";


DTLAlgorithm::DTLAlgorithm()
{
    QMap<QString, QVariant> defaultConfig;
    defaultConfig[CONFIG_LARGE_FILE_THRESHOLD] = 1024 * 1024;
    defaultConfig[CONFIG_ENABLE_OPTIMIZATION] = true;
    defaultConfig[CONFIG_MAX_DIFF_SIZE] = 10 * 1024 * 1024; // 15MB max
    defaultConfig[CONFIG_ENABLE_HEURISTICS] = true;

    setConfiguration(defaultConfig);
}
// ----------------------- Diff calculation -------------------------

QDiffResult DTLAlgorithm::calculateDiff(const QString &leftFile, const QString &rightFile, DiffMode mode)
{
    QDiffResult result;
    try {
        QList<DiffChange> changes;

        // Choose diff method based on mode
        switch (mode) {
            case DiffMode::LineByLine:
                changes = diffLineByLine(leftFile, rightFile);
                break;

            case DiffMode::CharByChar:
                changes = diffCharByChar(leftFile, rightFile);
                break;

            case DiffMode::Auto:
            default: {
                // Auto mode: choose based on file size and content
                int totalSize = leftFile.length() + rightFile.length();
                QVariant threshold = getConfiguration().value(CONFIG_LARGE_FILE_THRESHOLD, 1024 * 1024);

                if (totalSize > threshold.toInt()) {
                    // Large files: use line-by-line for better performance
                    changes = diffLineByLine(leftFile, rightFile);
                } else {
                    // Small files: use character-by-character for precision
                    changes = diffCharByChar(leftFile, rightFile);
                }
                break;
            }
        }

        // Calculate line numbers for the changes
        calculateLineNumbers(changes, leftFile, rightFile);

        result.setChanges(changes);
        result.setSuccess(true);

        // Add metadata about the algorithm used
        QMap<QString, QVariant> metadata;
        metadata["algorithm"] = "DTL";
        metadata["algorithm_name"] = getName();
        metadata["mode"] = (mode == DiffMode::LineByLine) ? "line" :
                          (mode == DiffMode::CharByChar) ? "char" : "auto";
        metadata["total_changes"] = changes.size();
        result.setMetaData(metadata);

    } catch (...) {
        result.setSuccess(false);
        result.setErrorMessage("DTL algorithm failed to calculate diff");
    }

    return result;
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

QList<DiffChange> DTLAlgorithm::diffCharByChar(const QString &leftFile, const QString &rightFile)
{
    // Convert strings to character vectors for DTL
    std::vector<QChar> leftChars(leftFile.begin(), leftFile.end());
    std::vector<QChar> rightChars(rightFile.begin(), rightFile.end());

    // Create DTL diff object and calculate differences
    dtl::Diff<QChar> dtlDiff(leftChars, rightChars);
    dtlDiff.compose();

    // Convert DTL result to QDiffX format
    return convertDTLSequenceChar(dtlDiff);
}


// ----------------------- Algorithm Info -------------------------

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

// ----------------------- Algorithm Configuration -------------------------

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

// ----------------------- Performance -------------------------

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

double DTLAlgorithm::calculateSimilarity(const QList<DiffChange> &changes, const QString &leftText, const QString &rightText) const
{
    int totalChars = std::max(leftText.length(), rightText.length());
    if (totalChars == 0) return 1.0;

    int equalChars = 0;
    for (const auto &change : changes) {
        if (change.operation == DiffOperation::Equal) {
            equalChars += change.text.length();
        }
    }

    return static_cast<double>(equalChars) / totalChars;
}

// ----------------------- Helper Functions -------------------------

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

QList<DiffChange> DTLAlgorithm::convertDTLSequenceChar(const dtl::Diff<QChar> &dtlDiff) const
{
    QList<DiffChange> changes;
    auto ses = dtlDiff.getSes();
    int position = 0;

    for (const auto &edit : ses.getSequence()) {
        DiffChange change;
        change.operation = convertDTLOperation(edit.second.type);
        change.text = QString(edit.first); // Convert QChar to QString
        change.lineNumber = -1; // Will be calculated later
        change.position = position;

        changes.append(change);

        // Update position (don't advance for deletions)
        if (edit.second.type != dtl::SES_DELETE) {
            position += change.text.length();
        }
    }

    return changes;
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

void DTLAlgorithm::calculateLineNumbers(QList<DiffChange> &changes, const QString &leftFile, const QString &rightFile) const
{
    int leftLine = 1, rightLine = 1;
    int leftPos = 0, rightPos = 0;

    for (auto &change : changes) {
        switch (change.operation) {
        case DiffOperation::Equal:{
            change.lineNumber = leftLine;
            int lineJumps = change.text.count('\n');
            leftLine += lineJumps;
            rightLine += lineJumps;
            leftPos += change.text.length();
            rightPos += change.text.length();
            break;}

        case DiffOperation::Delete:
            change.lineNumber = leftLine;
            leftLine += change.text.count('\n');
            leftPos += change.text.length();
            break;

        case DiffOperation::Insert:
            change.lineNumber = rightLine;
            rightLine += change.text.count('\n');
            rightPos += change.text.length();
            break;

        case DiffOperation::Replace:{
            change.lineNumber = leftLine;
            int replaceLineJumps = change.text.count('\n');
            leftLine += replaceLineJumps;
            rightLine += replaceLineJumps;
            leftPos += change.text.length();
            rightPos += change.text.length();
            break;}
        }
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

} // namespace QDiffX
