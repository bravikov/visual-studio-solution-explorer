#ifndef SOLUTIONTREEMODEL_H
#define SOLUTIONTREEMODEL_H

#include <QAbstractItemModel>
#include <memory>

namespace SolutionTree {
    class IItem;
}

class SolutionTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SolutionTreeModel(QObject* parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public slots:
    void openSolution(const QString& filename);

private:
    QVector<std::shared_ptr<SolutionTree::IItem>> m_solutions;
};

#endif // SOLUTIONTREEMODEL_H
