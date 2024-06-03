#ifndef DELEGATE_H
#define DELEGATE_H

#include <QAbstractItemDelegate>
#include <QLineEdit>

class Delegate : public QAbstractItemDelegate {
    Q_OBJECT

public:
    Delegate(QObject *parent = nullptr);
    ~Delegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool eventFilter(QObject *object, QEvent *event) override;
};

#endif // DELEGATE_H
