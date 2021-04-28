#include "SolutionTreeModel.h"
#include <QDebug>


#include "SolutionParser/SolutionParser.h"
#include "SolutionParser/FileSolutionSource.h"
#include "SolutionParser/ISolutionProject.h"
#include "SolutionParser/ISolutionGlobal.h"
#include "SolutionParser/SolutionProjectType.h"
#include <QMessageBox>
#include <QFileInfo>
#include <unordered_map>

namespace SolutionTree {
    class IItem
    {
    public:
        virtual ~IItem() {}
        virtual const std::string& name() = 0;
        virtual const std::shared_ptr<IItem>& parent() = 0;

        virtual int childCount() const = 0;
        virtual const std::shared_ptr<IItem>& child(int row) const = 0;
        virtual int row() const = 0;

        virtual void setParent(const std::shared_ptr<IItem>& parent, int row) = 0;
        virtual void addChild(const std::shared_ptr<IItem>& child) = 0;
    };

    class BaseItem: public IItem
    {
    public:
        const std::string& name() override { return m_name; }

        const std::shared_ptr<IItem>& parent() override { return m_parent; }
        int row() const override
        {
            return m_row;
        }

        int childCount() const override { return static_cast<int>(m_childs.size()); }
        const std::shared_ptr<IItem>& child(int row) const override
        {
            return m_childs[static_cast<size_t>(row)];
        }

        void setParent(const std::shared_ptr<IItem>& parent, int row) override
        {
            m_parent = parent;
            m_row = row;
        }
        void addChild(const std::shared_ptr<IItem>& child) override { m_childs.push_back(child); }

        void setName(const std::string& name) { m_name = name; }

    protected:
        std::string m_name;
        std::shared_ptr<IItem> m_parent;
        int m_row = -1;
        std::vector<std::shared_ptr<IItem>> m_childs;
    };

    class Solution: public BaseItem {};
    class SolutionFolder: public BaseItem {};
    class Project: public BaseItem {};
    class FileFilter: public BaseItem {};
    class File: public BaseItem {};
}

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

    if (!parent.isValid() && row < m_solutions.size()) {
        return createIndex(row, column, m_solutions[row].get());
    }

    if (parent.isValid()) {
        SolutionTree::IItem* parentNode = static_cast<SolutionTree::IItem*>(parent.internalPointer());

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

    auto childNode = static_cast<SolutionTree::IItem*>(index.internalPointer());

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
        return m_solutions.size();
    }
    else {
        auto parentNode = static_cast<SolutionTree::IItem*>(parent.internalPointer());
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

    SolutionTree::IItem* node = static_cast<SolutionTree::IItem*>(index.internalPointer());

    return node->name().c_str();
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

    QString soulutionName = QFileInfo{filename}.completeBaseName();

    std::unordered_map<ISolutionProject::Uuid,
                       std::shared_ptr<SolutionTree::IItem>> uuidItemIndex;

    for (const auto& project: parsedSolution->projects()) {
        std::shared_ptr<SolutionTree::BaseItem> projectItem;
        if (project->type() == SolutionProjectType::SolutionFolder) {
            projectItem = std::make_shared<SolutionTree::SolutionFolder>();
        }
        else {
            projectItem = std::make_shared<SolutionTree::Project>();
        }
        projectItem->setName(project->name());
        uuidItemIndex[project->uuid()] = projectItem;
    }

    for (const auto& project: parsedSolution->global()->nestedProjects()) {
        auto itemIt = uuidItemIndex.find(project.projectUuid);
        if (itemIt == uuidItemIndex.end()) {
            QMessageBox::warning(nullptr, "Solution tree error",
                                 tr("Unknown project with UUID %1").arg(
                                     project.parentProjectUuid.c_str()));
            return;
        }

        auto parentItemIt = uuidItemIndex.find(project.parentProjectUuid);
        if (parentItemIt == uuidItemIndex.end()) {
            QMessageBox::warning(nullptr, "Solution tree error",
                                 tr("Unknown solution folder with UUID %1").arg(
                                     project.parentProjectUuid.c_str()));
            return;
        }

        if (itemIt->second->parent() != nullptr) {
            QMessageBox::warning(nullptr, "Solution tree error",
                                 tr("The project already is in a folder.").arg(
                                     project.parentProjectUuid.c_str()));
            return;
        }

        itemIt->second->setParent(parentItemIt->second, parentItemIt->second->childCount());
        parentItemIt->second->addChild(itemIt->second);
    }

    auto solutionItem = std::make_shared<SolutionTree::Solution>();
    solutionItem->setName(soulutionName.toStdString());
    solutionItem->setParent(nullptr, m_solutions.size());

    for (const auto& projectIt: uuidItemIndex) {
        if (!projectIt.second->parent()) {
            projectIt.second->setParent(solutionItem, solutionItem->childCount());
            solutionItem->addChild(projectIt.second);
        }
    }

    beginInsertRows(QModelIndex(), m_solutions.size(), m_solutions.size());
    m_solutions.push_back(solutionItem);
    endInsertRows();
}
