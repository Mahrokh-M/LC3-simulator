// memorytablemodel.h
#include <QAbstractTableModel>
#include <vector>
#include <cstdint>

class MemoryTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    MemoryTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setMemory(const std::vector<uint16_t> &memory);

private:
    std::vector<uint16_t> m_memory;
};
