#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void usage();
    void showfile(QString filename);

    // PEPTOPAL default palette, vedi /usr/share/vice/C64/pepto-palold.vpl dell'emulatore VICE
    QList<uint32_t> palette = {
        0xff000000, 0xffffffff, 0xff58291d, 0xff91c6d5,
        0xff915ca8, 0xff588d43, 0xff352879, 0xffb8c76f,
        0xff916f43, 0xff433900, 0xff9a6759, 0xff353535,
        0xff747474, 0xff9ad284, 0xff7466be, 0xffb8b8b8
    };

public slots:
    void exit();
    void loadimage();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
