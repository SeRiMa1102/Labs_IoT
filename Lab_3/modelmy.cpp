#include "modelmy.h"


CustomItem::CustomItem(const QVector<QVariant> &data, CustomItem *parentItem)
    : m_itemData(data), m_parentItem(parentItem) {}

CustomItem::~CustomItem() {
    qDeleteAll(m_childItems);
}

void CustomItem::appendChild(CustomItem *child) {
    m_childItems.append(child);
}

CustomItem *CustomItem::child(int row) {
    return m_childItems.value(row);
}

int CustomItem::childCount() const {
    return m_childItems.count();
}

int CustomItem::columnCount() const {
    return m_itemData.count();
}

QVariant CustomItem::data(int column) const {
    return m_itemData.value(column);
}

int CustomItem::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<CustomItem*>(this));

    return 0;
}

CustomItem *CustomItem::parentItem() {
    return m_parentItem;
}

bool CustomItem::setData(int column, const QVariant &value) {
    if (column < 0 || column >= m_itemData.size())
        return false;

    m_itemData[column] = value;
    return true;
}


CustomModel::CustomModel(const QMap<QString, QStringList> &data, QObject *parent)
    : QAbstractItemModel(parent) {
    QVector<QVariant> rootData;
    rootData << "Root";
    rootItem = new CustomItem(rootData);
    setupModelData(rootItem, "Root", data);
}

CustomModel::~CustomModel() {
    delete rootItem;
}

void CustomModel::setupModelData(CustomItem *parent, const QString &parentKey, const QMap<QString, QStringList> &dataMap) {
    if (!dataMap.contains(parentKey))
        return;

    for (const QString &childKey : dataMap[parentKey]) {
        QVector<QVariant> childData;
        childData << childKey;
        CustomItem *childItem = new CustomItem(childData, parent);
        parent->appendChild(childItem);
        setupModelData(childItem, childKey, dataMap); // рекурсивно добавляем потомков
    }
}

QModelIndex CustomModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CustomItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<CustomItem*>(parent.internalPointer());

    CustomItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex CustomModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    CustomItem *childItem = static_cast<CustomItem*>(index.internalPointer());
    CustomItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CustomModel::rowCount(const QModelIndex &parent) const {
    CustomItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<CustomItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int CustomModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<CustomItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant CustomModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    CustomItem *item = static_cast<CustomItem*>(index.internalPointer());

    return item->data(index.column());
}

bool CustomModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == Qt::EditRole) {
        CustomItem *item = static_cast<CustomItem*>(index.internalPointer());
        if (item->setData(index.column(), value)) {
            emit dataChanged(index, index, {role});
            return true;
        }
    }
    return false;
}

Qt::ItemFlags CustomModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void CustomModel::updateData(const QMap<QString, QStringList> &dataMap) {
    beginResetModel();
    delete rootItem;
    QVector<QVariant> rootData;
    rootData << "Root";
    rootItem = new CustomItem(rootData);
    setupModelData(rootItem, "Root", dataMap);
    endResetModel();
}
