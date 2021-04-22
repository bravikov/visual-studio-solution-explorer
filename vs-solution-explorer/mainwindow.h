#ifndef H_413A52C4_63ED_4AE0_8DA2_1CF1E340DF49
#define H_413A52C4_63ED_4AE0_8DA2_1CF1E340DF49

#include "SolutionTreeModel.h"
#include <QMainWindow>
#include <QSettings>

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
    void showAbout();

signals:
    void openedSolution(const QString& filename);

private:
    Ui::MainWindow* m_ui;
    QSettings m_settings;

    SolutionTreeModel* m_model;
};
#endif // H_413A52C4_63ED_4AE0_8DA2_1CF1E340DF49
