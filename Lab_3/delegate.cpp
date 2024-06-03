#include "Delegate.h"
#include <QPainter>
#include <QLineEdit>
#include <QKeyEvent>

Delegate::Delegate(QObject *parent)
    : QAbstractItemDelegate(parent) {}

Delegate::~Delegate() {}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid())
        return;

    QString text = index.data(Qt::DisplayRole).toString();
    painter->drawText(option.rect, Qt::AlignCenter, text);
}

QSize Delegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const {
    return QSize(100, 30); // Размер ячейки
}

QWidget *Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->installEventFilter(const_cast<Delegate*>(this));
    return lineEdit;
}

void Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (lineEdit)
        lineEdit->setText(index.data(Qt::DisplayRole).toString());
}

void Delegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (lineEdit)
        model->setData(index, lineEdit->text(), Qt::EditRole);
}

void Delegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
    editor->setGeometry(option.rect);
}

bool Delegate::eventFilter(QObject *object, QEvent *event)  {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            emit commitData(static_cast<QWidget*>(object));
            emit closeEditor(static_cast<QWidget*>(object));
            return true;
        }
    }
    return QObject::eventFilter(object, event);
}
