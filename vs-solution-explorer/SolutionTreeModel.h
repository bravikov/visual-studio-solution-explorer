#ifndef SOLUTIONTREEMODEL_H
#define SOLUTIONTREEMODEL_H

#include <QAbstractItemModel>
#include <memory>

class ISolutionTreeNode
{
public:
    using Ptr = std::shared_ptr<ISolutionTreeNode>;

    virtual ~ISolutionTreeNode() {}

    virtual int row() const = 0;
    virtual Ptr parent() const = 0;

    virtual QString value() const = 0;

    virtual int childCount() const = 0;
    virtual Ptr child(int row) const = 0;
};

class SolutionNode: public ISolutionTreeNode
{
public:
    SolutionNode(int row, const QString& value, std::shared_ptr<SolutionNode> parent)
        : m_row{row}
        , m_value{value}
        , m_parent{parent}
    {}

    QString value() const override
    {
        return m_value;
    }

    Ptr parent() const override
    {
        return m_parent;
    }

    Ptr child(int row) const override
    {
        return m_childs[row];
    }

    int childCount() const override
    {
        return m_childs.count();
    }

    int row() const override
    {
        return m_row;
    }

    int m_row = 0;
    QString m_value;
    Ptr m_parent;
    QVector<std::shared_ptr<SolutionNode>> m_childs;
};


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

    QVector<ISolutionTreeNode::Ptr> m_nodes;
};

#endif // SOLUTIONTREEMODEL_H
