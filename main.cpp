#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

/**
 * @brief Основная функция программы.
 *
 * Функция main является точкой входа в приложение. Она создает объект QApplication,
 * который управляет настройками и ресурсами приложения, затем создает главное окно
 * (MainWindow) и отображает его. Затем управление передается методу exec() объекта
 * QApplication, который запускает основной цикл обработки событий.
 *
 * @param argc Количество аргументов командной строки.
 * @param argv Массив строковых аргументов командной строки.
 * @return Возвращает код завершения приложения.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
