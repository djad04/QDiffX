#include "QDiffWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

QDiffWidget::QDiffWidget(QWidget *parent, const QString &leftLabelText, const QString &rightLabelText)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *headerLayout = new QHBoxLayout(this);
    QLabel *leftLabel = new QLabel(leftLabelText, this);
    QLabel *rightLabel = new QLabel(rightLabelText, this);
    headerLayout->addWidget(leftLabel);
    headerLayout->addWidget(rightLabel);
    mainLayout->addLayout(headerLayout);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_leftTextBrowser = new QTextBrowser(m_splitter);
    m_rightTextBrowser = new QTextBrowser(m_splitter);

    mainLayout->addWidget(m_splitter);
    setLayout(mainLayout);
}

QDiffWidget::~QDiffWidget() {}

void QDiffWidget::setLeftContent(const QString leftContent)
{
    m_leftContent = leftContent ;
    m_leftTextBrowser ->setPlainText(m_leftContent);
}

void QDiffWidget::setRightContent(const QString rightContent)
{
    m_rightContent = rightContent ;
    m_rightTextBrowser ->setPlainText(m_rightContent);
}

void QDiffWidget::setContent(const QString &leftContent, const QString &rightContent)
{
    m_leftContent = leftContent ;
    m_rightContent = rightContent ;
    m_leftTextBrowser ->setPlainText(m_leftContent);
    m_rightTextBrowser ->setPlainText(m_rightContent);
}


void QDiffWidget::resetLeftContent()
{
    setLeftContent({});
}

void QDiffWidget::resetRightContent()
{
    setRightContent({});
}

void QDiffWidget::resetAll()
{
    resetLeftContent();
    resetRightContent();
}
QString QDiffWidget::rightContent() const
{
    return m_rightContent;
}

QString QDiffWidget::leftContent() const
{
    return m_leftContent;
}
