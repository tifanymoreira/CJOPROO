#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QScreen>
#include <QStyle>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet(R"(
        QMainWindow {
            background-color: #FFEFF3;
            color: #C2185B;
        }
        QLineEdit {
            background-color: #FFFFFF;
            border: 1px solid #FFCDD2;
            padding: 8px;
            color: #C2185B;
            font-size: 14px;
            border-radius: 5px;
        }
        QPushButton {
            background-color: #F8BBD0;
            border: none;
            color: #880E4F;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            font-size: 14px;
            margin: 4px 2px;
            border-radius: 5px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #F48FB1;
        }
        QPushButton#btnUpdate {
            background-color: #EF9A9A;
        }
        QPushButton#btnUpdate:hover {
            background-color: #E57373;
        }
        QPushButton#btnDelete {
            background-color: #FFCCBC;
        }
        QPushButton#btnDelete:hover {
            background-color: #FFAB91;
        }
        QListWidget {
            background-color: #FFFFFF;
            border: 1px solid #FFCDD2;
            color: #C2185B;
            font-size: 15px;
            padding: 5px;
            border-radius: 5px;
            selection-background-color: #FBC02D;
            selection-color: white;
        }
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #FFCDD2;
        }
        QListWidget::item:selected {
            background-color: #FBC02D;
            color: white;
        }
    )"); // Corrigido: ")" antes do ";"

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ToDoList_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
