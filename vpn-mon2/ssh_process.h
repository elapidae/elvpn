#ifndef SSH_PROCESS_H
#define SSH_PROCESS_H

#include <QObject>
#include <QProcess>

#include "openvpn.h"
#include "ipsec.h"


class Ssh_Process : public QObject
{
    Q_OBJECT
public:
    explicit Ssh_Process(QObject *parent = nullptr);
    virtual ~Ssh_Process();

    QString ssh_command = "ssh";
    QStringList ssh_arguments;

    QList<QByteArray> grab_commands;

    void start();
    void stop();

    OpenVPN  ovpn;
    IPSec    ipsec;

signals:
    void server_log( const QByteArray& );
    void signal_update();

public slots:
    void grab_server();

private slots:
    void slot_ssh_cout();
    void slot_ssh_cerr();
    void slot_ssh_changed();

private:
    QProcess ssh_proc;

    QList<QByteArray> cout_lines;
    QByteArray cout_buffer;
    enum class State
    {
        starting_ssh,
        wait_start_label,
        in_openvpn,
        in_ipsec,
    } state = State::starting_ssh;
    void process_buffer();
    void process_one_line(QByteArray line);
    void process_ipsec();
    void process_ovpn();


    void send_cmd( const QByteArray& cmd );

    template<typename T>
    void send_cmd(QByteArray first, T second)
    {
        first = first + ' ' + second;
        send_cmd(first);
    }

    template<typename T, typename ... Args>
    void send_cmd(QByteArray first, T second, Args ... args)
    {
        first = first + ' ' + second;
        send_cmd(first, args...);
    }

};

#endif // SSH_PROCESS_H
