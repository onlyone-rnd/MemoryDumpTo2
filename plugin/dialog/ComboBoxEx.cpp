
#include "ComboBoxEx.h"

#include <QAbstractItemModel>
#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>

ComboBoxEx::ComboBoxEx(QWidget* parent)
    : QComboBox(parent)
{
}

bool ComboBoxEx::isSelectableIndex(int index) const
{
    if (index < 0 || index >= count())
        return false;

    const QModelIndex mi = model()->index(index, modelColumn(), rootModelIndex());
    if (!mi.isValid())
        return false;

    const Qt::ItemFlags flags = model()->flags(mi);
    return (flags & Qt::ItemIsEnabled) && (flags & Qt::ItemIsSelectable);
}

int ComboBoxEx::nextSelectableIndex(int start, int step) const
{
    if (step == 0)
        return -1;

    for (int i = start; i >= 0 && i < count(); i += step)
    {
        if (isSelectableIndex(i))
            return i;
    }
    return -1;
}

int ComboBoxEx::firstSelectableIndex() const
{
    return nextSelectableIndex(0, +1);
}

int ComboBoxEx::lastSelectableIndex() const
{
    return nextSelectableIndex(count() - 1, -1);
}

void ComboBoxEx::wheelEvent(QWheelEvent* event)
{
    if (count() <= 0)
    {
        event->ignore();
        return;
    }
    const int delta = event->angleDelta().y();
    if (delta == 0)
    {
        QComboBox::wheelEvent(event);
        return;
    }
    const int step = (delta < 0) ? +1 : -1;
    int current = currentIndex();
    if (!isSelectableIndex(current))
    {
        int fixed = nextSelectableIndex(current, step);
        if (fixed < 0)
            fixed = (step > 0) ? firstSelectableIndex() : lastSelectableIndex();

        if (fixed >= 0)
            setCurrentIndex(fixed);

        event->accept();
        return;
    }
    const int next = nextSelectableIndex(current + step, step);
    if (next >= 0)
        setCurrentIndex(next);
    event->accept();
}

void ComboBoxEx::keyPressEvent(QKeyEvent* event)
{
    int next = -1;

    switch (event->key())
    {
    case Qt::Key_Up:
        next = nextSelectableIndex(currentIndex() - 1, -1);
        break;

    case Qt::Key_Down:
        next = nextSelectableIndex(currentIndex() + 1, +1);
        break;

    case Qt::Key_Home:
        next = firstSelectableIndex();
        break;

    case Qt::Key_End:
        next = lastSelectableIndex();
        break;

    default:
        QComboBox::keyPressEvent(event);
        return;
    }

    if (next >= 0)
        setCurrentIndex(next);

    event->accept();
}