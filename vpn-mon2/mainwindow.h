#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "openvpn.h"
#include "ipsec.h"
#include "ip_locator.h"
#include "ssh_process.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    class Cout;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slot_grab_timer();
    void on_btn_start_clicked();
    void on_btn_stop_clicked();
    void on_btn_connect_clicked();
    void on_btn_disconnect_clicked();
    void on_btn_unlog_clicked();

    void on_le_cmd_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTimer grab_timer;

    Ssh_Process vdsina, elapidae;

    void slot_update();
};

#endif // MAINWINDOW_H
