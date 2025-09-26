#pragma once

#include "pathfinding.h"
#include "qfuturewatcher.h"

#include <QMainWindow>
#include <QSettings>
#include <QString>
#include <QGraphicsScene>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Box;
class Scene;
class View;

/**
 * @brief Класс главного окна (MainWindow) приложения.
 *
 * Этот класс управляет интерфейсом пользователя, обработкой событий и логикой поиска пути на графической сцене.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    using pathNodes = std::vector<Node>;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void closeEvent(QCloseEvent *event);
    void on_pbGenerate_clicked();
    void on_pbPathFinding_clicked();
    void on_leW_textChanged(const QString &arg1);
    void startAnimatedPath();
    void on_rdAnimated_clicked(bool checked);
    void on_rbManually_clicked(bool checked);
    void finish();

private:
    Ui::MainWindow *ui; /**< Указатель на интерфейс пользователя. */

    QString m_settingsPath;     /**< Путь к файлу настроек. */
    View *m_view;               /**< Указатель на виджет представления (View). */
    Scene *m_scene;             /**< Указатель на графическую сцену (Scene). */
    int m_boxSize;              /**< Размер квадратов на сцене. */
    const int m_minBoxSize = 6;             /**< Минимальный размер квадрата. */
    std::vector<std::vector<bool>> m_grid;  /**< Сетка, представляющая блокировки ячеек. */
    std::map<std::pair<int,int>, Box*> m_boxes;  /**< Карта квадратов на сцене. */
    std::vector<QGraphicsLineItem *> m_currentPath;             /**< Текущий путь на сцене. */
    QFutureWatcher<pathNodes> m_watcher;    /** < Монитор для отслеживания выполнения поиска пути. */

    void readSettings();
    void writeSettings();
    void fillScene(int w, int h);
    void createWall();
    void createGrid();
    void paintPath(pathNodes &path);
    const Node getEndNode(Box*);
    const Node getStartNode(Box*);
    void deletePath();
    void showHint(const QString &msg);
};
