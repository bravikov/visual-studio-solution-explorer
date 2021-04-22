#include "SolutionTreeModel.h"
#include <QDebug>


#include "SolutionParser/SolutionParser.h"
#include "SolutionParser/FileSolutionSource.h"
#include "SolutionParser/ISolutionProject.h"
#include <QMessageBox>
#include <QFileInfo>

SolutionTreeModel::SolutionTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant SolutionTreeModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,
                                       int /*role*/) const
{
    return QVariant{};
}

QModelIndex SolutionTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    if (!parent.isValid() && row < m_nodes.size()) {
        return createIndex(row, column, m_nodes[row].get());
    }

    if (parent.isValid()) {
        ISolutionTreeNode* parentNode = static_cast<ISolutionTreeNode*>(parent.internalPointer());

        if (row < parentNode->childCount()) {
            auto childNode = parentNode->child(row);
            return createIndex(row, column, childNode.get());
        }
    }

    return QModelIndex();
}

QModelIndex SolutionTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    auto childNode = static_cast<ISolutionTreeNode*>(index.internalPointer());

    if (childNode->parent()) {
        return createIndex(childNode->parent()->row(), 0, childNode->parent().get());
    }

    return QModelIndex();
}

int SolutionTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        return m_nodes.size();
    }
    else {
        auto parentNode = static_cast<ISolutionTreeNode*>(parent.internalPointer());
        return parentNode->childCount();
    }
}

int SolutionTreeModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant SolutionTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    ISolutionTreeNode* node = static_cast<ISolutionTreeNode*>(index.internalPointer());

    return node->value();
}

void SolutionTreeModel::openSolution(const QString& filename)
{
    std::unique_ptr<ISolution> parsedSolution;
    try {
        FileSolutionSource fileSolutionSource{filename.toStdString()};
        SolutionParser solutionParser;
        parsedSolution = solutionParser.parse(fileSolutionSource);

        const bool hasError = solutionParser.lastError().code != ISolutionParser::ErrorCode::Ok;
        if (!parsedSolution || hasError) {
            throw std::runtime_error{solutionParser.lastError().description};
        }
    }  catch (const std::runtime_error& error) {
        QMessageBox::warning(nullptr, "Solution opening error", error.what());
        return;
    }

    QString soulutionName = QFileInfo{filename}.baseName();

    auto solutionNode = std::make_shared<SolutionNode>(m_nodes.size(), soulutionName, nullptr);

    for (const auto& project: parsedSolution->projects()) {
        auto projectNode = std::make_shared<SolutionNode>(solutionNode->m_childs.size(),
                                                          project->name().c_str(),
                                                          solutionNode);
        solutionNode->m_childs.push_back(projectNode);
    }

    beginInsertRows(QModelIndex(), m_nodes.size(), m_nodes.size());
    m_nodes.push_back(solutionNode);
    endInsertRows();
}
