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

QDiffX::QDiffResult QDiffX::DMPAlgorithm::calculateDiff(const QString &leftFile, const QString &rightFile)
{
    QDiffResult result;
    try {
        QList<Diff> diffs = m_dmp.diff_main(leftFile, rightFile, m_checkLines);

        m_dmp.diff_cleanupSemantic(diffs);
        m_dmp.diff_cleanupEfficiency(diffs);



    } catch (...) {
    }

    return result;
}

AlgorithmCapabilities DMPAlgorithm::getCapabilities() const
{
    AlgorithmCapabilities caps;
    caps.supportsLargeFiles = false;  // DMP is memory-intensive, not for large files
    caps.supportsUnicode = true;
    caps.supportsBinary = false;
    caps.supportsLineByLine = true;   // Has diff_lineMode()
    caps.supportsCharByChar = true;   // Default character-level precision
    caps.supportsWordByWord = false;  // No built-in word mode
    caps.maxRecommendedSize = 1024 * 1024;  // 1MB - DMP performance limit
    caps.description = "Google's Diff-Match-Patch algorithm with character-level precision, optimized for small to medium files";
    return caps;
}

}// namespace QDiffX
