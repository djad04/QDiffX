#ifndef QDIFFALGORITHM_H
#define QDIFFALGORITHM_H

#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>


namespace QDiffX{

enum class DiffOperation{
    Equal,
    Insert,
    Delete,
    Replace
};

struct DiffChange{
    DiffOperation operation;
    QString text;
    int lineNumber;
    int position;

    DiffChange(DiffOperation op = DiffOperation::Equal,
               QString txt = QString(),
               int line = -1,
               int pos = -1)
        :operation(op), text(txt), lineNumber(line), position(pos) {}
};

class QDiffResult{
public:
    QDiffResult() : m_success(false) {}

    QList<DiffChange> changes() const { return m_changes; }
    void addChange(const DiffChange &change) { m_changes.append(change); }
    void setChanges(const QList<DiffChange> &newChanges) { m_changes = newChanges;}

    bool success() const { return m_success; }
    void setSuccess(bool newSuccess) { m_success = newSuccess; }

    QString errorMessage() const { return m_errorMessage; }
    void setErrorMessage(const QString &newErrorMessage) { m_errorMessage = newErrorMessage; }

    QMap<QString, QVariant> allMetaData() const { return m_metaData; }
    QVariant metaData(QString &Key) const { return m_metaData.value(Key); }
    void setMetaData(const QMap<QString, QVariant> &newMetaData) { m_metaData = newMetaData; }

private:
    QList<DiffChange> m_changes;
    bool m_success;
    QString m_errorMessage;
    QMap<QString, QVariant> m_metaData;
};
struct AlgorithmCapabilities {
    bool supportsLargeFiles;
    bool supportsUnicode;
    bool supportsBinary;
    bool supportsLineByLine;
    bool supportsCharByChar;
    bool supportsWordByWord;
    int maxRecommendedSize;
    QString description;

    AlgorithmCapabilities()
        : supportsLargeFiles(false), supportsUnicode(true), supportsBinary(false),
        supportsLineByLine(true), supportsCharByChar(false), supportsWordByWord(false),
        maxRecommendedSize(1024*1024) {} // 1MB
};

class DiffAlgorithm{
    virtual ~DiffAlgorithm() = default;

    virtual QDiffResult calculateDiff(QString &leftFile, QString &rightFile) = 0;

    // Algorithm Info:
    virtual QString getName() = 0;
    virtual QString getDescription() const = 0;


private:
    QMap<QString, QVariant> m_config;

};
















}//namespace QDiffX

#endif // QDIFFALGORITHM_H
