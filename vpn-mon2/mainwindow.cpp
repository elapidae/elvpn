#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "vlog.h"
#include "vcat.h"

static auto vdsina_url = "v37503.hosted-by-vdsina.com";
static auto elapidae_url = "elapidae.org";

static auto started_label = "=== Started ===";

static auto ovpn_start_label = "=== OVPN START ===";
//static auto openvpn_status_file = "/root/elvpn/kylemanna/openvpn-data/openvpn-status.log";
static auto ovpn_end_label = "=== OVPN END ===";

static auto ipsec_start_label = "=== IPSEC START ===";
static auto ipsec_status_script = "/usr/local/sbin/sh-show-ipsec.sh";
static auto ipsec_end_label = "=== IPSEC END ===";

//=======================================================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->out->setMaximumBlockCount(200);

    connect( &grab_timer, &QTimer::timeout, this, &MainWindow::slot_grab_timer );

    connect( &vdsina, &Ssh_Process::server_log, [this](const QByteArray& arr) {
        ui->out->appendPlainText( QString::fromLatin1(arr) );
    });
    connect( &elapidae, &Ssh_Process::server_log, [this](const QByteArray& arr) {
        ui->out->appendPlainText( QString::fromLatin1(arr) );
    });

    connect( &vdsina, &Ssh_Process::signal_update, this, &MainWindow::slot_update );
    connect( &elapidae, &Ssh_Process::signal_update, this, &MainWindow::slot_update );

    vdsina.ssh_arguments = QStringList{
        "-p", "28078", "-C",
        QString("monitor2@") + vdsina_url,
    };
    auto vdcmd1 = vcat("echo ", ovpn_start_label,
                       " && show_openvpn && echo ", ovpn_end_label).str();
    auto vdcmd2 = vcat("echo ", ipsec_start_label, " && sudo ", ipsec_status_script,
                       " && echo ", ipsec_end_label).str();

    vdsina.grab_commands.append(vdcmd1.c_str());
    vdsina.grab_commands.append(vdcmd2.c_str());

    elapidae.ssh_arguments = QStringList{
        "-p", "28078", "-C",
        QString("root@") + elapidae_url,
    };

    auto elcmd = vcat("echo ", ovpn_start_label,
                      " && cat el-ovpn/el-ovpn-data/status.log && echo ",
                      ovpn_end_label).str();
    elapidae.grab_commands.append( elcmd.c_str() );

    vdsina.start();
    elapidae.start();

    connect( &grab_timer, &QTimer::timeout, &vdsina, &Ssh_Process::grab_server );
    connect( &grab_timer, &QTimer::timeout, &elapidae, &Ssh_Process::grab_server );
    grab_timer.start(1000);
}
//=======================================================================================
MainWindow::~MainWindow()
{
    delete ui;
}
//=======================================================================================

//=======================================================================================
void MainWindow::slot_grab_timer()
{
    vdsina.grab_server();
    elapidae.grab_server();
}
//=======================================================================================
void MainWindow::on_btn_start_clicked()
{
    grab_timer.start(1000);
}
//=======================================================================================
void MainWindow::on_btn_stop_clicked()
{
    grab_timer.stop();
    vdsina.stop();
    elapidae.stop();
}
//=======================================================================================
auto constexpr hline = R"(<hr style="border: none; border-top: 1px solid #666; margin: 1px 0;">)";
void MainWindow::slot_update()
{
    QMultiMap<QByteArray, UserLog> map = vdsina.ovpn.users;
    map.unite(vdsina.ipsec.users);
    map.unite(elapidae.ovpn.users);
    map.unite(elapidae.ipsec.users);

    QString html;
    for ( auto && user: map )
    {
        html += user.text() + hline;
    }

    ui->browser->setHtml( html );
}
//=======================================================================================
void MainWindow::on_btn_connect_clicked()
{
    on_btn_disconnect_clicked();
    vdsina.start();
    elapidae.start();
}
//=======================================================================================
void MainWindow::on_btn_disconnect_clicked()
{
}
//=======================================================================================
void MainWindow::on_btn_unlog_clicked()
{
    auto new_count = ui->out->maximumBlockCount() == 1
            ? 100 : 1;

    ui->out->setMaximumBlockCount(new_count);
}
//=======================================================================================

