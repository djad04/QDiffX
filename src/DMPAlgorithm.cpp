#include "DMPAlgorithm.h"

namespace QDiffX{

// Configuration keys
const QString DMPAlgorithm::CONFIG_TIMEOUT = "timeout";
const QString DMPAlgorithm::CONFIG_EDIT_COST = "edit_cost";
const QString DMPAlgorithm::CONFIG_MATCH_THRESHOLD = "match_threshold";
const QString DMPAlgorithm::CONFIG_MATCH_DISTANCE = "match_distance";
const QString DMPAlgorithm::CONFIG_PATCH_DELETE_THRESHOLD = "patch_delete_threshold";
const QString DMPAlgorithm::CONFIG_PATCH_MARGIN = "patch_margin";
const QString DMPAlgorithm::CONFIG_MATCH_MAX_BITS = "match_max_bits";
const QString DMPAlgorithm::CONFIG_CHECK_LINES = "check_lines";


QDiffX::DMPAlgorithm::DMPAlgorithm() : m_checkLines(true) {
    m_dmp.Diff_Timeout = 0.0f;  // No timeout for accuracy
    m_dmp.Diff_EditCost = 4;
    m_dmp.Match_Threshold = 0.5f;
    m_dmp.Match_Distance = 100000;
    m_dmp.Patch_DeleteThreshold = 0.5f;
    m_dmp.Patch_Margin = 4;
    m_dmp.Match_MaxBits = 8192;
}


QDiffX::QDiffResult QDiffX::DMPAlgorithm::calculateDiff(const QString &leftFile, const QString &rightFile, DiffMode mode)
{
    QDiffResult result;
    try {
        QList<Diff> dmpChanges;

        // Choose diff method based on mode
        switch (mode) {
            case DiffMode::LineByLine:
                dmpChanges = diffLineByLine(leftFile, rightFile);
                break;

            case DiffMode::CharByChar:
                dmpChanges = diffCharByChar(leftFile, rightFile);
                break;

            case DiffMode::Auto:
            default:
              dmpChanges = m_dmp.diff_main(leftFile, rightFile);
                m_dmp.diff_cleanupSemantic(dmpChanges);
                m_dmp.diff_cleanupEfficiency(dmpChanges);
                break;
        }


        QList<QDiffX::DiffChange> changes = convertDiffList(dmpChanges);

        // Calculate line numbers for the changes
        calculateLineNumbers(changes, leftFile, rightFile);

        result.setChanges(changes);
        result.setSuccess(true);

        // Add metadata about the algorithm used
        QMap<QString, QVariant> metadata;
        metadata["algorithm"] = "DMP";
        metadata["mode"] = (mode == DiffMode::LineByLine) ? "line" :
                          (mode == DiffMode::CharByChar) ? "char" : "auto";
        metadata["total_changes"] = changes.size();
        result.setMetaData(metadata);

    } catch (...) {
        result.setSuccess(false);
        result.setErrorMessage("DMP algorithm failed to calculate diff");
    }

    return result;
}
QList<Diff> DMPAlgorithm::diffCharByChar(const QString &leftFile, const QString &rightFile)
{
    // checkLines=false for pure character-by-character comparison
    QList<Diff> diffs = m_dmp.diff_main(leftFile, rightFile, false);

    // Step 2: Apply cleanup for better results
    m_dmp.diff_cleanupSemantic(diffs);
    m_dmp.diff_cleanupEfficiency(diffs);

    return diffs;
}

QList<Diff> DMPAlgorithm::diffLineByLine(const QString &leftFile, const QString &rightFile)
{
    // Use DMP's integrated line mode by setting checklines=true
    QList<Diff> diffs = m_dmp.diff_main(leftFile, rightFile, true);

    // Apply cleanup for better results
    m_dmp.diff_cleanupSemantic(diffs);
    m_dmp.diff_cleanupEfficiency(diffs);

    return diffs;
}

AlgorithmCapabilities DMPAlgorithm::getCapabilities() const
{
    AlgorithmCapabilities caps;
    caps.supportsLargeFiles = false;
    caps.supportsUnicode = true;
    caps.supportsBinary = false;
    caps.supportsLineByLine = true;   // Has diff_lineMode()
    caps.supportsCharByChar = true;   // Default character-level precision
    caps.supportsWordByWord = false;
    caps.maxRecommendedSize = 1024 * 1024;
    caps.description = "Reimplemented Google diff-match-patch,deprecated Qt4 components Replaced and updated to modern C++, optimized performance";

    return caps;
}

void DMPAlgorithm::setConfiguration(const QMap<QString, QVariant> &newConfig)
{
    QDiffAlgorithm::setConfiguration(newConfig);

    // Apply DMP-specific configuration
    if (newConfig.contains(CONFIG_TIMEOUT)) {
        m_dmp.Diff_Timeout = newConfig[CONFIG_TIMEOUT].toFloat();
    }
    if (newConfig.contains(CONFIG_EDIT_COST)) {
        m_dmp.Diff_EditCost = newConfig[CONFIG_EDIT_COST].toInt();
    }
    if (newConfig.contains(CONFIG_MATCH_THRESHOLD)) {
        m_dmp.Match_Threshold = newConfig[CONFIG_MATCH_THRESHOLD].toFloat();
    }
    if (newConfig.contains(CONFIG_MATCH_DISTANCE)) {
        m_dmp.Match_Distance = newConfig[CONFIG_MATCH_DISTANCE].toInt();
    }
    if (newConfig.contains(CONFIG_PATCH_DELETE_THRESHOLD)) {
        m_dmp.Patch_DeleteThreshold = newConfig[CONFIG_PATCH_DELETE_THRESHOLD].toFloat();
    }
    if (newConfig.contains(CONFIG_PATCH_MARGIN)) {
        m_dmp.Patch_Margin = newConfig[CONFIG_PATCH_MARGIN].toInt();
    }
    if (newConfig.contains(CONFIG_MATCH_MAX_BITS)) {
        m_dmp.Match_MaxBits = newConfig[CONFIG_MATCH_MAX_BITS].toInt();
    }
    if (newConfig.contains(CONFIG_CHECK_LINES)) {
        m_checkLines = newConfig[CONFIG_CHECK_LINES].toBool();
    }

}

QStringList DMPAlgorithm::getConfigurationKeys() const {
    return {
        CONFIG_TIMEOUT,
        CONFIG_EDIT_COST,
        CONFIG_MATCH_THRESHOLD,
        CONFIG_MATCH_DISTANCE,
        CONFIG_PATCH_DELETE_THRESHOLD,
        CONFIG_PATCH_MARGIN,
        CONFIG_MATCH_MAX_BITS,
        CONFIG_CHECK_LINES
    };
}

int DMPAlgorithm::estimateComplexity(const QString &leftText, const QString &rightText) const
{
    // TODO: requires Benchmark
    return 0;
}

bool DMPAlgorithm::isRecommendedFor(const QString &leftText, const QString &rightText) const
{
    // TODO: will be implemented Later
    return 0;
}

DiffOperation DMPAlgorithm::convertOperation(Operation dmpOp) const
{
    switch (dmpOp) {
    case INSERT:
        return DiffOperation::Insert;
    case DELETE:
        return DiffOperation::Delete;
    case EQUAL:
        return DiffOperation::Equal;
    default:
        return DiffOperation::Equal;
    }
}

QList<DiffChange> DMPAlgorithm::convertDiffList(const QList<Diff> &dmpDiffs) const
{
    QList<QDiffX::DiffChange> changes;
    int position =0;

    for(auto &diff : dmpDiffs){
        DiffChange change;
        change.operation = convertOperation(diff.operation) ;
        change.text = diff.text ;
        change.lineNumber = -1; // will be calculated later using calculateLineNumbers()
        change.position = position;

        changes.append(change);

        if (diff.operation != DELETE) {
            position += diff.text.length();
        }
    }
    return changes;
}

void DMPAlgorithm::calculateLineNumbers(QList<DiffChange> &changes, const QString &leftFile, const QString rightFile) const
{
    int leftLine = 1 , rightLine = 1;
    int leftPos = 0, rightPos = 0 ;

    for(auto &change : changes){
        switch (change.operation) {
        case QDiffX::DiffOperation::Equal: {
            change.lineNumber = leftLine;
            int countLineJump = change.text.count("\n");
            leftLine += countLineJump;
            rightLine += countLineJump;
            leftPos += change.text.length();
            rightPos += change.text.length();
            break;}
        case QDiffX::DiffOperation::Delete:
            change.lineNumber = leftLine;
            leftLine += change.text.count("\n");
            leftPos += change.text.length();
            break;
        case QDiffX::DiffOperation::Insert:
            change.lineNumber = rightLine;
            rightLine += change.text.count('\n');
            rightPos += change.text.length();
            break;
        case QDiffX::DiffOperation::Replace:{
            change.lineNumber = leftLine;
            int countLineJump = change.text.count("\n");
            leftLine += countLineJump;
            rightLine += countLineJump;
            leftPos += change.text.length();
            rightPos += change.text.length();
            break;}
        }
    }
}

}// namespace QDiffX
