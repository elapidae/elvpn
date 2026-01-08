#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#define vdeb qDebug() << __LINE__

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


static auto elapidae_url = "elapidae.org";

static auto started_label = "=== Started ===";

static auto ovpn_start_label = "=== OVPN START ===";
static auto ovpn_end_label = "=== OVPN END ===";


struct TCol
{
    enum {
        from     = 0,
        key      = 1,
        ip       = 2,
        loc      = 3,
        recv     = 4,
        recv_spd = 5,
        sent     = 6,
        sent_spd = 7,
        count = 8
    };
};
//=======================================================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->table->setColumnCount(TCol::count);
    ui->table->setHorizontalHeaderItem(TCol::from, new QTableWidgetItem("From"));
    ui->table->setHorizontalHeaderItem(TCol::key, new QTableWidgetItem("Key"));
    ui->table->setHorizontalHeaderItem(TCol::ip, new QTableWidgetItem("IP"));
    ui->table->setHorizontalHeaderItem(TCol::loc, new QTableWidgetItem("Location"));
    ui->table->setHorizontalHeaderItem(TCol::recv, new QTableWidgetItem("Recv"));
    ui->table->setHorizontalHeaderItem(TCol::recv_spd, new QTableWidgetItem("R-srd"));
    ui->table->setHorizontalHeaderItem(TCol::sent, new QTableWidgetItem("Sent"));
    ui->table->setHorizontalHeaderItem(TCol::sent_spd, new QTableWidgetItem("S-spd"));
    ui->table->setSortingEnabled(true);
    ui->table->sortItems(TCol::from, Qt::AscendingOrder);

    ui->out->setMaximumBlockCount(200);

    connect( &grab_timer, &QTimer::timeout, this, &MainWindow::slot_grab_timer );

    connect( &elapidae, &Ssh_Process::server_log, [this](const QByteArray& arr) {
        ui->out->appendPlainText( QString::fromLatin1(arr) );
    });

    connect( &elapidae, &Ssh_Process::signal_update, [this]()
    {
        slot_update();
    });

    elapidae.ssh_arguments = QStringList{
        "-p", "28078", "-C",
        QString("root@") + elapidae_url,
    };

    auto elcmd = QByteArray("echo ") + ovpn_start_label +
                      " && cat el-ovpn/el-ovpn-data/status.log && echo " +
                      ovpn_end_label;
    elapidae.grab_commands.append( elcmd );
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
    auto &map = elapidae.ovpn.users;

    auto * table = ui->table;
    table->setRowCount( map.size() );
    ui->table->setSortingEnabled(false);

    auto set_item = [table](int row, int col, QString text) {
        auto item = table->item( row, col );
        if ( !item )
        {
            item = new QTableWidgetItem;
            table->setItem(row, col, item);
        }
        item->setText(text);
    };

    int idx = 0;
    QString html;
    for ( auto && _user: map )
    {
        auto & user = _user.second;
        html += user.text() + hline;
        set_item(idx, TCol::from,     user.get_from());
        set_item(idx, TCol::key,      user.get_key());
        set_item(idx, TCol::ip,       user.get_ip());
        set_item(idx, TCol::loc,      user.get_loc());
        set_item(idx, TCol::recv,     user.get_recv());
        set_item(idx, TCol::recv_spd, user.get_recv_spd());
        set_item(idx, TCol::sent,     user.get_sent());
        set_item(idx, TCol::sent_spd, user.get_sent_spd());
        ++idx;
    }

    table->resizeColumnsToContents();
    ui->table->setSortingEnabled(true);
    ui->browser->setHtml( html );
}
//=======================================================================================
void MainWindow::on_btn_connect_clicked()
{
    on_btn_disconnect_clicked();
    elapidae.start();
}
//=======================================================================================
void MainWindow::on_btn_disconnect_clicked()
{
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

