#include "QDiffWidget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "QDiffX_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    QDiffX::QDiffWidget w;
    w.setContent("sdnk\nlsdk\nf\n", "aknf\nakf\nlkfn\n");
    w.setGeometry(0,0,400,800);
    w.show();
    return a.exec();
}
