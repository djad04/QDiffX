#ifndef DTLALGORITHM_H
#define DTLALGORITHM_H

#include "QDiffAlgorithm.h"
#include "dtl/dtl.hpp"

namespace QDiffX {

class DTLAlgorithm : public QDiffAlgorithm
{
public:
    DTLAlgorithm();
    virtual ~DTLAlgorithm() = default;

    // QDiffAlgorithm interface Implementation
    QDiffResult calculateDiff(const QString &leftFile, const QString &rightFile, DiffMode mode = DiffMode::Auto) override;

    // DTL-specific diff methods
    QList<DiffChange> diffLineByLine(const QString &leftFile, const QString &rightFile);
    QList<DiffChange> diffCharByChar(const QString &leftFile, const QString &rightFile);

    QString getName() const override { return "DTL-Diff-Template-Library-Algorithm"; }
    QString getDescription() const override {
        return "High-performance DTL (Diff Template Library) algorithm optimized for large files and line-based comparisons";
    }

    AlgorithmCapabilities getCapabilities() const override;


private:
    // Configuration keys
    static const QString CONFIG_LARGE_FILE_THRESHOLD;
    static const QString CONFIG_ENABLE_OPTIMIZATION;
    static const QString CONFIG_MAX_DIFF_SIZE;
    static const QString CONFIG_ENABLE_HEURISTICS;
};

} // namespace QDiffX

#endif // DTLALGORITHM_H
