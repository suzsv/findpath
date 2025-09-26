#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "box.h"
#include "scene.h"
#include "view.h"

#include <random>
#include <vector>
#include <QDir>
#include <QValidator>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFuture>
#include <QPromise>
#include <QFutureWatcher>
#include <QFutureWatcherBase>
#include <QtConcurrent/QtConcurrent>


/**
 * @brief Конструктор класса MainWindow.
 *
 * Инициализирует главное окно, устанавливает интерфейс пользователя, создает и настраивает виджеты и сцену.
 *
 * @param parent Указатель на родительский виджет.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_settingsPath = QDir::toNativeSeparators(QDir::currentPath()+"/settings.ini");
    ui->setupUi(this);

    QHBoxLayout *hLayout = new QHBoxLayout(ui->graphicsView);
    m_view = new View(ui->graphicsView);
    hLayout->addWidget(m_view);

    m_scene = new Scene(m_view);
    m_view->setScene(m_scene);

    setWindowTitle(tr("Задача поиска пути"));
    QValidator *validator = new QIntValidator(0, 999, this);
    // редактирование lineedit допустимо только числами от 0 and 999
    ui->leH->setValidator(validator);
    ui->leW->setValidator(validator);

    ui->pbPathFinding->setEnabled(false);

    connect(m_scene, &Scene::animated,this, &MainWindow::startAnimatedPath);
    connect(&m_watcher, &QFutureWatcher<pathNodes>::finished,this, [this] { finish(); });
    showHint(tr("Введите количество квадратов (поля ввода - \"W\", \"H\")...."));
    readSettings();
}

/**
 * @brief Деструктор класса MainWindow.
 */
MainWindow::~MainWindow()
{
    m_watcher.cancel();
    m_watcher.waitForFinished();
    delete ui;
}

/**
 * @brief Заполнение сцены квадратами.
 *
 * Создает и добавляет на сцену квадраты в соответствии с заданными размерами сетки.
 *
 * @param w Ширина сетки.
 * @param h Высота сетки.
 */
void MainWindow::fillScene(int w, int h)
{
    int ww = m_view->width()/w - 5;
    int hh = m_view->height()/h - 5;
    m_boxSize = (ww > hh)? hh : ww;

    if (m_boxSize < m_minBoxSize)
        m_boxSize = m_minBoxSize;

    // Заполнение сцены квадратами
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            // Создание нового квадрата с заданными параметрами
            QGraphicsItem *item = new Box(m_boxSize);
            item->setData(2,QPoint(i,j));
            item->setPos(i*m_boxSize,j*m_boxSize);
            m_boxes[{i,j}] = qgraphicsitem_cast<Box*>(item);
            m_scene->addItem(item);
        }
    }
    createWall();
    m_view->update();
}

/**
 * @brief Создание препятствий.
 *
 * Генерирует случайные препятствия на сцене с вероятностью 20%.
 */
void MainWindow::createWall()
{
    std::random_device rd; // Создаем объект для получения случайного начального значения
    std::mt19937 gen(rd()); //Инициализируем генератор случайных чисел значением, полученным от std::random_device

    // Создаем распределение Бернулли с вероятностью генерации значения  true = 0.2
    std::bernoulli_distribution dist(0.2);

    QList<QGraphicsItem*> items = m_scene->items();
    for (const auto &item : m_boxes) {
        bool random_value = dist(gen); // Генерируем случайное булево значение
        item.second->setIsBusy(random_value);
    }
}

/**
 * @brief Чтение настроек из файла.
 *
 * Читает и применяет сохраненные настройки окна из файла settings.ini.
 */
void MainWindow::readSettings()
{
    QSettings settings(m_settingsPath,QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    const auto geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty())
        setGeometry(200, 200, 400, 400);
    else
        restoreGeometry(geometry);
    settings.endGroup();

}

/**
 * @brief Запись настроек в файл.
 *
 * Сохраняет текущие настройки окна в файл settings.ini.
 */
void MainWindow::writeSettings()
{
    QSettings settings(m_settingsPath,QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

/**
 * @brief Обработчик события закрытия окна.
 *
 * Сохраняет настройки перед закрытием окна.
 *
 * @param event Указатель на событие закрытия окна.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    writeSettings();
}

/**
 * @brief Удаление предыдущего пути.
 *
 * Удаляет все линии, представляющие путь на сцене.
 */
void MainWindow::deletePath()
{
    // Удаляем предыдущие линии
    for (QGraphicsLineItem* line : m_currentPath) {
        m_scene->removeItem(line);
        delete line;
    }
    m_currentPath.clear();
}

/**
 * @brief Отображение подсказки в строке статуса.
 *
 * Обновляет текст в строке статуса для отображения сообщений пользователю.
 *
 * @param msg Сообщение для отображения.
 */
void MainWindow::showHint(const QString &msg)
{
    ui->statusbar->showMessage(msg);
}

/**
 * @brief Отрисовка пути.
 *
 * Рисует на сцене путь, представленный вектором узлов.
 *
 * @param path Вектор узлов, представляющий путь.
 */
void MainWindow::paintPath(pathNodes &path)
{
    // Удаляем старый путь
    deletePath();
    ui->lbResult->setText(QString("%1").arg(path.size()));

    // Рисуем путь
    QPen pen(Qt::red, 2);
    for (size_t i = 1; i < path.size(); ++i) {
        std::pair xy1 = std::make_pair(path[i-1].x,path[i-1].y);
        std::pair xy2 = std::make_pair(path[i].x,path[i].y);

        QPointF p1(m_boxes[xy1]->scenePos()-QPointF(m_boxSize/2,m_boxSize/2));  // Вычисляем начальную точку линии
        QPointF p2(m_boxes[xy2]->scenePos()-QPointF(m_boxSize/2,m_boxSize/2));  // Вычисляем конечную точку линии

        QGraphicsLineItem *line = m_scene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), pen);
        m_currentPath.push_back(line);
    }
    m_view->update();
}

/**
 * @brief Получение узла конца пути.
 *
 * Находит и возвращает узел, соответствующий конечному элементу пути.
 *
 * @param box Конечный элемент пути.
 * @return Узел, соответствующий конечному элементу пути.
 */
const Node MainWindow::getEndNode(Box *box)
{
    return Node{box->data(2).toPoint().x(),box->data(2).toPoint().y()};
}

/**
 * @brief Получение узла начала пути.
 *
 * Находит и возвращает узел, соответствующий начальному элементу пути.
 *
 * @param box Начальный элемент пути.
 * @return Узел, соответствующий начальному элементу пути.
 */
const Node MainWindow::getStartNode(Box *box)
{
    return Node{box->data(2).toPoint().x(),box->data(2).toPoint().y()};
}

/**
 * @brief Создание сетки для поиска пути.
 *
 * Создает сетку, представляющую состояние блокировки ячеек, на основе текущих квадратов на сцене.
 */
void MainWindow::createGrid()
{
    int row = ui->leW->text().toInt();
    int col = ui->leH->text().toInt();

    m_grid.resize(row, std::vector<bool>(col));

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            m_grid[i][j] = m_boxes[{i,j}]->isBusy();
        }
    }
}

/**
 * @brief Запуск поиска пути для анимированного отображения пути.
 *
 * Использует алгоритм A* для поиска пути.
 */
void MainWindow::startAnimatedPath()
{
    Node start = getStartNode(m_scene->startItem());
    Node end = getEndNode(m_scene->endItem());

    m_watcher.setFuture(QtConcurrent::run(a_star_search,m_grid, start, end));
}

/**
 * @brief Обработчик завершения поиска пути.
 *
 * Отрисовывает найденный путь на сцене.
 */
void MainWindow::finish()
{
    if (m_watcher.isCanceled())
        return;

    //ui->progressBar->setValue(0);
    pathNodes path = m_watcher.result();
    if (path.empty()) {
        return;
    }

    paintPath(path);
}


/**
 * @brief Обработчик нажатия кнопки "Найти путь".
 *
 * Запускает поиск пути с использованием алгоритма A* при выборе ручного режима.
 */
void MainWindow::on_pbPathFinding_clicked()
{
    if (ui->rbManually->isChecked()) {
        Node start = getStartNode(m_scene->startItem());
        Node end = getEndNode(m_scene->endItem());
        QFuture<std::vector<Node>> future = QtConcurrent::run(a_star_search,m_grid, start, end);
        std::vector<Node> path = future.result();

        if (path.empty()) {
            QMessageBox::warning(this, tr("Внимание!"),tr("Невозможно найти путь."));
            return;
        }

        paintPath(path);
        return;
    }
}

/**
 * @brief Обработчик изменения текста в поле ввода ширины.
 *
 * Отключает кнопку "Найти путь" и отображает подсказку пользователю.
 *
 * @param arg1 Текст поля ввода.
 */
void MainWindow::on_leW_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    ui->pbPathFinding->setEnabled(false);
    showHint(tr("Нажмите кнопку \"Генерировать\""));
}

/**
 * @brief Обработчик нажатия кнопки "Генерировать".
 *
 * Генерирует новую сетку квадратов на сцене и включает кнопку "Найти путь".
 */
void MainWindow::on_pbGenerate_clicked()
{
    if (!m_boxes.empty()) m_boxes.clear();
    if (!m_grid.empty()) m_grid.clear();
    m_scene->clearScene();
    m_view->resetZoom();
    m_currentPath.clear();
    ui->lbResult->setText("0");

    auto textW = ui->leW->text();
    auto textH = ui->leH->text();

    if (textW.isEmpty() || textH.isEmpty()) {
        QMessageBox::warning(this, tr("Внимание!"),tr("Введите количество квадратов (поля ввода - \"W\", \"H\")....\n"));
        return;
    }

    fillScene(textW.toInt(), textH.toInt());
    createGrid();
    ui->pbPathFinding->setEnabled(true);
    showHint(tr("Выберете начальную и конечную точку маршрута. Нажмите кнопку \"Найти путь\""));
}

/**
 * @brief Обработчик выбора режима анимации.
 *
 * Включает режим анимации при выборе соответствующей радиокнопки.
 *
 * @param checked Состояние радиокнопки.
 */
void MainWindow::on_rdAnimated_clicked(bool checked)
{
    if (checked) {
        m_scene->startAnimated(true);
        showHint(tr("Выберете начальную точку маршрута. Для отмены выбора нажмите правую кнопку мыши."));
    }
}

/**
 * @brief Обработчик выбора ручного режима.
 *
 * Отключает режим анимации при выборе соответствующей радиокнопки.
 *
 * @param checked Состояние радиокнопки.
 */
void MainWindow::on_rbManually_clicked(bool checked)
{
    if (checked) {
        m_watcher.cancel();
        m_watcher.waitForFinished();
        //ui->progressBar->setValue(0);
        m_scene->startAnimated(false);
        showHint(tr("Выберете начальную и конечную точку маршрута. Нажмите кнопку \"Найти путь\"."));
    }
}

