#include "scene.h"
#include "box.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMessageBox>

/**
 * @brief Конструктор класса Scene.
 *
 * Инициализирует объект класса Scene с указанным родительским объектом.
 *
 * @param parent Указатель на родительский объект.
 */
Scene::Scene(QObject *parent)
    : QGraphicsScene(parent)
{

}

/**
 * @brief Получение конечного элемента пути.
 *
 * Возвращает указатель на конечный элемент (Box) пути.
 *
 * @return Указатель на конечный элемент пути.
 */
Box *Scene::endItem() const
{
    return m_endItem;
}


/**
 * @brief Получение начального элемента пути.
 *
 * Возвращает указатель на начальный элемент (Box) пути.
 *
 * @return Указатель на начальный элемент пути.
 */
Box *Scene::startItem() const
{
    return m_startItem;
}


/**
 * @brief Очистка сцены.
 *
 * Удаляет начальный и конечный элементы пути. Если параметр `all` установлен в `true`, очищает все элементы сцены.
 *
 * @param all Флаг, определяющий, нужно ли очистить все элементы сцены (`true`) или только начальный и конечный элементы (`false`).
 */
void Scene::clearScene(bool all)
{
    if (m_startItem != nullptr) {
        m_startItem->setStart(false);
        m_startItem = nullptr;
    }
    if (m_endItem != nullptr) {
        m_endItem->setEnd(false);
        m_endItem = nullptr;
    }
    if (all) clear();
}

/**
 * @brief Управление анимацией.
 *
 * Запускает или останавливает анимацию элементов сцены. При запуске анимации очищается сцена и обновляется состояние.
 *
 * @param start Флаг запуска анимации (true для запуска, false для остановки).
 */
void Scene::startAnimated(bool start)
{
    clearScene(false);
    update();
    m_startAnimated = start;
}

/**
 * @brief Обработчик события перемещения мыши.
 *
 * Обрабатывает событие перемещения мыши на сцене при активированной анимации. Если мышь перемещается над новым элементом, он становится конечным элементом, и испускается сигнал animated().
 *
 * @param event Указатель на событие перемещения мыши.
 */
void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_startAnimated && m_startItem != nullptr) {
        QPointF pos = event->scenePos();
        auto *item = qgraphicsitem_cast<Box*>(itemAt(pos,QTransform()));

        if (item == nullptr)
            return;

        if (item != m_currentItem) {
            m_currentItem = item;
            m_endItem = item;
            emit animated();
        }
    }
}

/**
 * @brief Обработчик события нажатия мыши.
 *
 * Обрабатывает событие нажатия левой кнопки мыши на сцене. Если анимация запущена и начальный элемент уже выбран, событие игнорируется.
 *
 * @param event Указатель на событие нажатия мыши.
 */
void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->button() == Qt::RightButton) && m_startAnimated) {
        startAnimated(true);
        return;
    }

    if ((event->button() != Qt::LeftButton) || (m_startAnimated && m_startItem !=nullptr))
        return;

    QPointF pos = event->scenePos();
    auto *item = qgraphicsitem_cast<Box*>(itemAt(pos,QTransform()));

    if (item == nullptr)
        return;

    auto *widgetParent = (parent()->parent()->isWidgetType())?static_cast<QWidget*>(parent()->parent()):nullptr;

    if (item->isBusy()) {
        QMessageBox::warning(widgetParent, tr("Внимание!"),tr("Эта ячейка не доступна!\n"
                                                               "Выберите другую ячейку."));
        return;
    }

    if (m_startItem != nullptr && m_endItem != nullptr) {
        m_startItem->setStart(false);
        m_endItem->setEnd(false);
        m_startItem = nullptr;
        m_endItem = nullptr;
    }

    if (m_startItem == nullptr) {
        m_startItem = item;
        m_startItem->setStart(true);
    }
    else {
        m_endItem = item;
        if (m_startItem == m_endItem) {
            QMessageBox::warning(widgetParent, tr("Внимание!"),tr("Начало пути не может совпадать с концом пути!\n"
                                                                   "Выберите другую точку."));
            m_endItem = nullptr;
        }
        else
            m_endItem->setEnd(true);
    }
    update();
}

