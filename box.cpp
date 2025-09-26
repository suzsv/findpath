#include "box.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QRandomGenerator>
#include <QStyleOption>
#include <QDebug>

/**
 * @brief Конструктор класса Box.
 *
 * Инициализирует объект квадрата с заданным размером и родительским элементом.
 *
 * @param size Размер квадрата.
 * @param parent Указатель на родительский графический элемент.
 */
Box::Box(int size, QGraphicsItem *parent):
    QGraphicsItem(parent),
    m_size(size)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

/**
 * @brief Получение состояния занятости квадрата.
 *
 * Возвращает true, если квадрат занят (недоступен), и false в противном случае.
 *
 * @return Состояние занятости квадрата.
 */
bool Box::isBusy() const
{
    return m_isBusy;
}

/**
 * @brief Установка состояния занятости квадрата.
 *
 * Устанавливает, занят ли квадрат (недоступен).
 *
 * @param newIsBusy Новое состояние занятости квадрата.
 */
void Box::setIsBusy(bool newIsBusy)
{
    m_isBusy = newIsBusy;
}

/**
 * @brief Получение состояния начала пути.
 *
 * Возвращает true, если квадрат является начальным элементом пути, и false в противном случае.
 *
 * @return Состояние начала пути.
 */
bool Box::start() const
{
    return m_start;
}

/**
 * @brief Установка состояния начала пути.
 *
 * Устанавливает, является ли квадрат начальным элементом пути.
 *
 * @param newStart Новое состояние начала пути.
 */
void Box::setStart(bool newStart)
{
    m_start = newStart;
}

/**
 * @brief Получение состояния конца пути.
 *
 * Возвращает true, если квадрат является конечным элементом пути, и false в противном случае.
 *
 * @return Состояние конца пути.
 */
bool Box::end() const
{
    return m_end;
}

/**
 * @brief Установка состояния конца пути.
 *
 * Устанавливает, является ли квадрат конечным элементом пути.
 *
 * @param newEnd Новое состояние конца пути.
 */
void Box::setEnd(bool newEnd)
{
    m_end = newEnd;
}

/**
 * @brief Получение ограничивающего прямоугольника квадрата.
 *
 * Возвращает ограничивающий прямоугольник квадрата для отрисовки и взаимодействия.
 *
 * @return Ограничивающий прямоугольник квадрата.
 */
QRectF Box::boundingRect() const
{
    return QRectF(QPointF(-m_size, -m_size), QSize(m_size,m_size));
}

/**
 * @brief Отрисовка квадрата.
 *
 * Отрисовывает квадрат с учетом его состояния (занятость, начало/конец пути).
 *
 * @param painter Указатель на объект QPainter для рисования.
 * @param option Указатель на параметры стиля отрисовки.
 * @param widget Указатель на виджет (не используется в данном контексте).
 */
void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Устанавливаем шрифт с учетом масштаба
    QFont font = painter->font();
    auto psize = boundingRect().height()/2;
    font.setPointSizeF((psize<=0)?2:psize);
    painter->setFont(font);

    painter->setPen(Qt::black);

    // Рисуем прямоугольник
    painter->drawRect(boundingRect());

    QRectF adjustedRect = boundingRect().adjusted(1, 1, -1, -1);

    if (m_isBusy)
        painter->fillRect(adjustedRect, QBrush(m_busyColor));
    else
        painter->fillRect(adjustedRect, QBrush(m_freeColor));

    // Рисуем текст внутри прямоугольника
    if (m_start) {
        painter->drawText(adjustedRect,Qt::AlignCenter,"A");
    }
    if (m_end) {
        painter->drawText(adjustedRect,Qt::AlignCenter,"B");
    }
}

