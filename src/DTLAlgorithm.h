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

    // Algorithm interface Implementation
    QDiffResult calculateDiff(const QString &leftFile, const QString &rightFile, DiffMode mode = DiffMode::Auto) override;

    // diff methods
    QList<DiffChange> diffLineByLine(const QString &leftFile, const QString &rightFile);
    QList<DiffChange> diffCharByChar(const QString &leftFile, const QString &rightFile);

    QString getName() const override { return "DTL-Diff-Template-Library-Algorithm"; }
    QString getDescription() const override {
        return "High-performance DTL (Diff Template Library) algorithm optimized for large files and line-based comparisons";
    }

    AlgorithmCapabilities getCapabilities() const override;

    // Algorithm Configuration
    void setConfiguration(const QMap<QString, QVariant> &newConfig) override;
    QStringList getConfigurationKeys() const override;

    // Performance
    int estimateComplexity(const QString &leftText, const QString &rightText) const override;
    bool isRecommendedFor(const QString &leftText, const QString &rightText) const override;

    double calculateSimilarity(const QList<DiffChange> &changes, const QString &leftText, const QString &rightText) const;

private:
    // DTL conversion helpers
    QList<DiffChange> convertDTLSequence(const dtl::Diff<QString> &dtlDiff) const;
    QList<DiffChange> convertDTLSequenceChar(const dtl::Diff<QChar> &dtlDiff) const;
    DiffOperation convertDTLOperation(dtl::edit_t dtlOp) const;
    void calculateLineNumbers(QList<DiffChange> &changes, const QString &leftFile, const QString &rightFile) const;

    // DTL utility methods
    QStringList splitIntoLines(const QString &text) const;
    QString joinLines(const QStringList &lines) const;

private:
    // Configuration keys
    static const QString CONFIG_LARGE_FILE_THRESHOLD;
    static const QString CONFIG_ENABLE_OPTIMIZATION;
    static const QString CONFIG_MAX_DIFF_SIZE;
    static const QString CONFIG_ENABLE_HEURISTICS;
};

} // namespace QDiffX

#endif // DTLALGORITHM_H
