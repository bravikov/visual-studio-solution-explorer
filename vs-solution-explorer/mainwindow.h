#ifndef H_413A52C4_63ED_4AE0_8DA2_1CF1E340DF49
#define H_413A52C4_63ED_4AE0_8DA2_1CF1E340DF49

#include "SolutionTreeModel.h"
#include <QMainWindow>
#include <QSettings>

class RecentFiles
{
public:
    void addFilename(const QString& filename);
    QStringList getFilenames();

private:
    class Settings
    {
    public:
        bool isValid(const QString& value);
        QString get(size_t i);
        void set(size_t i, const QString& value);

    private:
        QSettings m_settings;
    };

    Settings m_settings;
    static const int m_max;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void openSolution();
    void openRecentSolution(QAction* action);
    void openSolution(const QString& filename);
    void showAbout();
    void showRecentFiles();

signals:
    void openedSolution(const QString& filename);

private:
    Ui::MainWindow* m_ui;
    QSettings m_settings;
    RecentFiles m_recentFiles;

    SolutionTreeModel* m_model;
};
#endif // H_413A52C4_63ED_4AE0_8DA2_1CF1E340DF49
