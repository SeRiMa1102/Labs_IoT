#ifndef MODELMY_H
#define MODELMY_H

#include <QAbstractItemModel>
#include <QVector>
#include <QVariant>
#include <QMap>

class CustomItem {
public:
    CustomItem(const QVector<QVariant> &data, CustomItem *parentItem = nullptr);
    ~CustomItem();

    void appendChild(CustomItem *child);

    CustomItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    CustomItem *parentItem();

    bool setData(int column, const QVariant &value);

private:
    QVector<CustomItem*> m_childItems;
    QVector<QVariant> m_itemData;
    CustomItem *m_parentItem;
};

class CustomModel : public QAbstractItemModel {
    Q_OBJECT

public:
    CustomModel(const QMap<QString, QStringList> &data, QObject *parent = nullptr);
    ~CustomModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void updateData(const QMap<QString, QStringList> &dataMap);

private:
    void setupModelData(CustomItem *parent, const QString &parentKey, const QMap<QString, QStringList> &dataMap);

    CustomItem *rootItem;
};

#endif // MODELMY_H
