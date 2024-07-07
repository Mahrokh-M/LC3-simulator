#include "memorytablemodel.h"
MemoryTableModel::MemoryTableModel(QObject *parent)
    : QAbstractTableModel(parent), m_memory(0xFFFF)
{
}

int MemoryTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_memory.size();
}

int MemoryTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2; // Address and Value columns
}

QVariant MemoryTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    int row = index.row();
    int col = index.column();

    if (col == 0)
    {
        // Address column
        return QString("0x%1").arg(row, 4, 16, QChar('0')).toUpper();
    }
    else if (col == 1)
    {
        // Value column
        return QString("0x%1").arg(m_memory[row], 4, 16, QChar('0')).toUpper();
    }

    return QVariant();
}

QVariant MemoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if (section == 0)
            return QString("Address");
        else if (section == 1)
            return QString("Value");
    }

    return QVariant();
}

void MemoryTableModel::setMemory(const std::vector<uint16_t> &memory)
{
    beginResetModel();
    m_memory = memory;
    endResetModel();
}
