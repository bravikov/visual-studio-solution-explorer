#include "mainwindow.h"
#include "ui_mainwindow.h"
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

