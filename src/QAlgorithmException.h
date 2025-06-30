#pragma once
#include <QAlgorithmManagerError.h>
#include <QException>

namespace QDiffX {


class QAlgorithmException : public QException
{
public:
    QAlgorithmException(QAlgorithmManagerError error, const QString& message = QString())
        : m_error(error), m_message(message) {}

    QAlgorithmManagerError error() const { return m_error; }
    QString message() const { return m_message; }

    void raise() const override { throw *this; }
    QAlgorithmException* clone() const override { return new QAlgorithmException(*this); }

private:
    QAlgorithmManagerError m_error;
    QString m_message;
};

}//namespace QDiffX
