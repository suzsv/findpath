#include "view.h"

#include <QRandomGenerator>
#include <QWheelEvent>

/**
 * @brief Конструктор класса View.
 *
 * Инициализирует объект класса View, устанавливает начальные значения для зума и настраивает параметры отображения.
 *
 * @param parent Указатель на родительский виджет.
 */
View::View(QWidget *parent)
    : QGraphicsView(parent)
{
    m_zoom = 250;   // Начальное значение зума
    m_maxZoom = 500; // Максимальное значение зума
    m_minZoom = 0;  // Минимальное значение зума

    setRenderHint(QPainter::Antialiasing, false);               // Выключение сглаживания краев
    setOptimizationFlags(QGraphicsView::DontSavePainterState);  // Оптимизация: не сохранять состояние QPainter
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);  // Умный режим обновления области просмотра
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);   // Якорь трансформации под курсором мыши
}

/**
 * @brief Сброс значения зума до начального.
 *
 * Обновляет значение зума до 250 и делает область видимой с координатами (0, 0).
 */
void View::resetZoom()
{
    m_zoom = 250;
    ensureVisible(QRectF(0, 0, 0, 0));
    setupMatrix();  // Настройка матрицы преобразования
}

#if QT_CONFIG(wheelevent)
/**
 * @brief Обработчик события колесика мыши.
 *
 * Изменяет масштаб при удержании клавиши Control и движении колесика мыши.
 *
 * @param event Указатель на событие колесика мыши.
 */
void View::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0)
           zoomInBy(6);
        else
           zoomOutBy(6);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}
#endif

/**
 * @brief Увеличение масштаба.
 *
 * Увеличивает значение зума на заданное количество уровней, если не достигнуто максимальное значение.
 *
 * @param level Количество уровней для увеличения масштаба.
 */
void View::zoomInBy(int level)
{
    if (m_zoom + level >= m_maxZoom)
        return;

    m_zoom += level;
    setupMatrix();
}

/**
 * @brief Уменьшение масштаба.
 *
 * Уменьшает значение зума на заданное количество уровней, если не достигнуто минимальное значение.
 *
 * @param level Количество уровней для уменьшения масштаба.
 */
void View::zoomOutBy(int level)
{
    if (m_zoom - level <= m_minZoom)
        return;

    m_zoom -= level;
    setupMatrix();
}

/**
 * @brief Настройка матрицы преобразования.
 *
 * Вычисляет и применяет матрицу преобразования для текущего уровня зума.
 */
void View::setupMatrix()
{
    qreal scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));   // Вычисление коэффициента масштабирования
    QTransform matrix;
    matrix.scale(scale, scale); // Применение масштабирования к матрице
    setTransform(matrix);       // Установка матрицы преобразования
}
