#include "mainwindow.h"
#include "./ui_mainwindow.h"

//#include "vlog.h"
#include <QDebug>
#define vdeb qDebug() << __LINE__

//#include "vcat.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


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

    connect( &vdsina, &Ssh_Process::signal_update, [this]()
    {
        elapidae.shift(QDateTime::currentDateTimeUtc());
        slot_update();
    } );
    connect( &elapidae, &Ssh_Process::signal_update, [this]()
    {
        vdsina.shift(QDateTime::currentDateTimeUtc());
        slot_update();
    });

    vdsina.ssh_arguments = QStringList{
        "-p", "28078", "-C",
        QString("monitor2@") + vdsina_url,
    };
    auto vdcmd1 = QByteArray("echo ") + ovpn_start_label +
                       " && show_openvpn && echo " + ovpn_end_label;
    auto vdcmd2 = QByteArray("echo ") + ipsec_start_label + " && sudo " + ipsec_status_script +
                       " && echo " + ipsec_end_label;

    vdsina.grab_commands.append(vdcmd1);
    vdsina.grab_commands.append(vdcmd2);

    elapidae.ssh_arguments = QStringList{
        "-p", "28078", "-C",
        QString("root@") + elapidae_url,
    };

    auto elcmd = QByteArray("echo ") + ovpn_start_label +
                      " && cat el-ovpn/el-ovpn-data/status.log && echo " +
                      ovpn_end_label;
    elapidae.grab_commands.append( elcmd );

    //vdsina.start();
    //elapidae.start();

    connect( &grab_timer, &QTimer::timeout, &vdsina, &Ssh_Process::grab_server );
    connect( &grab_timer, &QTimer::timeout, &elapidae, &Ssh_Process::grab_server );
    //grab_timer.start(1000);
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
}
//=======================================================================================
auto constexpr hline = R"(<hr style="border: none; border-top: 0px solid #666; margin: 0px 0;">)";
void MainWindow::slot_update()
{
    auto map = vdsina.ovpn.users;
    map.insert(vdsina.ipsec.users.begin(), vdsina.ipsec.users.end());
    map.insert(elapidae.ovpn.users.begin(), elapidae.ovpn.users.end());
    map.insert(elapidae.ipsec.users.begin(), elapidae.ipsec.users.end());

    QString html;
    for ( auto && user: map )
    {
        html += user.second.text() + hline;
        //vdeb << user.text();
//        throw 42;
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
    vdsina.stop();
    elapidae.stop();
}
//=======================================================================================
void MainWindow::on_btn_unlog_clicked()
{
    auto new_count = ui->out->maximumBlockCount() == 1
            ? 100 : 1;

    ui->out->setMaximumBlockCount(new_count);
}
//=======================================================================================
static QString system_exec( QString line, int *rcode = nullptr )
{
    auto list = line.split(' ');
    if ( list.isEmpty() )
    {
        vdeb << "empty";
        return "err -- <empty>";
    }

    auto cmd = list.takeFirst();

    QProcess p;
    p.start(cmd, list);
    p.waitForFinished(2000);

    QString out = p.readAllStandardOutput();
    QString err = p.readAllStandardError();    
    if ( rcode ) *rcode = p.exitCode();

    if ( !err.isEmpty() )
    {
        out += "\nERR: " + err;
    }
    return out;
}

void MainWindow::on_le_cmd_textChanged(const QString &arg1)
{
    if ( !arg1.endsWith(',') )
    {
        vdeb << "wait" << arg1;
        return;
    }
    auto cmd = arg1.chopped(1);

    ui->out->appendPlainText(arg1);

    ui->le_cmd->clear();
    int rcode = -1;
    auto res = system_exec( cmd, &rcode );
    auto o = QString("[%1] %2").arg(rcode).arg(res);
    ui->out->appendPlainText( o );
}

