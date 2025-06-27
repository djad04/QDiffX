#ifndef DMPALGORITHM_H
#define DMPALGORITHM_H
#include "QDiffAlgorithm.h"
#include "DMP/diff_match_patch.h"
/*
TODO:
- Implement Functions estimateComplexity and is isRecommendedFor
*/
namespace QDiffX{


class DMPAlgorithm : public QDiffAlgorithm
{
public:
    DMPAlgorithm();
    virtual ~DMPAlgorithm() = default;


    // QDiffAlgorithm interface Implementation
    QDiffResult calculateDiff(const QString &leftFile, const QString &rightFile, DiffMode = DiffMode::Auto) override;

    QList<Diff> diffCharByChar(const QString &leftFile, const QString &rightFile);
    QList<Diff> diffLineByLine(const QString &leftFile, const QString &rightFile);

    QString getName() const override { return "Diff-Match-Patch-GoogleAlgorithme-Modernized"; }
    QString getDescription() const override {
        return "Reimplemented Google diff-match-patch,deprecated Qt4 components Replaced and updated to modern C++, optimized performance";
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
    DiffOperation convertOperation(Operation dmpOp) const;
    QList<DiffChange> convertDiffList(const QList<Diff>& dmpDiffs) const;
    void calculateLineNumbers(QList<DiffChange> &changes,const QString &leftFile,const QString rightFile) const;

private:

    // DMP Engine:
    diff_match_patch m_dmp;

    // Configuration keys
    static const QString CONFIG_TIMEOUT;
    static const QString CONFIG_EDIT_COST;
    static const QString CONFIG_MATCH_THRESHOLD;
    static const QString CONFIG_MATCH_DISTANCE;
    static const QString CONFIG_PATCH_DELETE_THRESHOLD;
    static const QString CONFIG_PATCH_MARGIN;
    static const QString CONFIG_MATCH_MAX_BITS;

};


}// namespace QDiffX
#endif // DMPALGORITHM_H
