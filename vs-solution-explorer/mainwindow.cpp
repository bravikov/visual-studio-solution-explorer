#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Utils/RecentValues.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_model = new SolutionTreeModel();
    m_ui->treeView->setModel(m_model);

    connect(this, &MainWindow::openedSolution, m_model, &SolutionTreeModel::openSolution);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::openSolution()
{
    const auto filename =
        QFileDialog::getOpenFileName(this, tr("Open Visual Studio Solution"),
                                     m_settings.value("lastFolder").toString(),
                                     tr("Solution (*.sln)"));

    m_settings.setValue("lastFolder", QFileInfo(filename).absoluteDir().absolutePath());

    openSolution(filename);
}

void MainWindow::openRecentSolution(QAction* action)
{
    openSolution(action->data().toString());
}

void MainWindow::openSolution(const QString& filename)
{
    if (filename.isEmpty()) {
        return;
    }

    m_recentFiles.addFilename(filename);

    emit openedSolution(filename);
}

void MainWindow::showAbout()
{
    const QString name{QApplication::applicationName()};
    const QString version{QApplication::applicationVersion()};
    const QString url{"https://github.com/bravikov/vs-solution-explorer"};
    const QString email{"dmitry@bravikov.pro"};
    const QString message{
        QObject::tr(R"(
            <h1>%1</h1>
            Version: %2.
            <p>Open source <a href='%3'>project</a>.</p>
            <p>© 2021 Dmitry Bravikov (<a href='mailto:%4'>%4</a>)</p>
        )").arg(name).arg(version).arg(url).arg(email)
    };
    QMessageBox::about(nullptr, name, message);
}

void MainWindow::showRecentFiles()
{
    m_ui->menuRecentFiles->clear();
    const auto filenames = m_recentFiles.getFilenames();
    for (const auto& filename: filenames) {
        QFileInfo fileInfo{filename};
        const auto text = QString{"%1 (from %2)"}.arg(fileInfo.fileName())
                                                 .arg(fileInfo.dir().dirName());
        auto action = m_ui->menuRecentFiles->addAction(text);
        action->setData(filename);
    }
}

const int RecentFiles::m_max = 10;


void RecentFiles::addFilename(const QString& filename)
{
    RecentValues<Settings, QString> recentValues{m_settings, m_max};
    recentValues.addValue(filename);
}

QStringList RecentFiles::getFilenames()
{
    QStringList list;
    for (size_t i = 0; i < m_max; i++) {
        const auto value = m_settings.get(i);
        if (!m_settings.isValid(value)) {
            break;
        }
        else {
            list.append(value);
        }
    }
    return list;
}


bool RecentFiles::Settings::isValid(const QString& value)
{
    return !value.isEmpty();
}

QString RecentFiles::Settings::get(size_t i)
{
    return m_settings.value(QString{"recentFiles/%1"}.arg(i)).toString();
}

void RecentFiles::Settings::set(size_t i, const QString& value)
{
    m_settings.setValue(QString{"recentFiles/%1"}.arg(i), value);
}
