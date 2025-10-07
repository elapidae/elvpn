#include "ssh_process.h"

#include "vlog.h"


static auto started_label = "=== Started ===";

static auto ovpn_start_label = "=== OVPN START ===";
//static auto openvpn_status_file = "/root/elvpn/kylemanna/openvpn-data/openvpn-status.log";
static auto ovpn_end_label = "=== OVPN END ===";

static auto ipsec_start_label = "=== IPSEC START ===";
static auto ipsec_status_script = "/usr/local/sbin/sh-show-ipsec.sh";
static auto ipsec_end_label = "=== IPSEC END ===";



//=======================================================================================
Ssh_Process::Ssh_Process(QObject *parent)
    : QObject{parent}
{
    connect( &ssh_proc, &QProcess::readyReadStandardOutput,
             this, &Ssh_Process::slot_ssh_cout);
    connect( &ssh_proc, &QProcess::readyReadStandardError,
             this, &Ssh_Process::slot_ssh_cerr);
    connect( &ssh_proc, &QProcess::stateChanged,
             this, &Ssh_Process::slot_ssh_changed);
}
//=======================================================================================
Ssh_Process::~Ssh_Process()
{
    send_cmd("exit");
    ssh_proc.waitForFinished();
}
//=======================================================================================
void Ssh_Process::start()
{
    ssh_proc.start( ssh_command, ssh_arguments );
    ssh_proc.waitForStarted();
}
//=======================================================================================
void Ssh_Process::stop()
{
    send_cmd( "exit" );
    ssh_proc.waitForFinished();
    state = State::starting_ssh;
    cout_lines.clear();
    cout_buffer.clear();
}
//=======================================================================================
void Ssh_Process::grab_server()
{
    for ( auto && cmd: grab_commands )
    {
        send_cmd( cmd );
    }
}
//=======================================================================================
void Ssh_Process::send_cmd( const QByteArray& text )
{
    ssh_proc.write( text + "\n" );
}
//=======================================================================================


//=======================================================================================
void Ssh_Process::slot_ssh_cout()
{
    auto msg = ssh_proc.readAllStandardOutput();
    emit server_log( msg );
    cout_buffer += msg;
    process_buffer();
}
//=======================================================================================
void Ssh_Process::slot_ssh_cerr()
{
    emit server_log( ssh_proc.readAllStandardError() );
}
//=======================================================================================
void Ssh_Process::slot_ssh_changed()
{
    emit server_log( QString(">>> stat: %1\n").arg(ssh_proc.state()).toLatin1() );

    if ( ssh_proc.state() != QProcess::Running )
        return;

    send_cmd( "echo", started_label );
}
//=======================================================================================


//=======================================================================================
void Ssh_Process::process_buffer()
{
    // split by lines and save last line, if it is not ready.
    auto lines = cout_buffer.split('\n');
    if ( !cout_buffer.isEmpty() && cout_buffer.back() != '\n')
    {
        cout_buffer = lines.takeLast();
    }
    else
    {
        cout_buffer.clear();
    }

    // scan lines
    for ( auto && line: lines )
    {
        if ( line.isEmpty() ) continue;
        process_one_line(line);
    }
}
//=======================================================================================
void Ssh_Process::process_one_line(QByteArray line)
{
    // ----------------------------------------------------------------------------------
    if ( state == State::wait_start_label )
    {
        if (line == ipsec_start_label) {
            state = State::in_ipsec;
            return;
        }
        if (line == ovpn_start_label) {
            state = State::in_openvpn;
            return;
        }
        vdeb << "look here:" << line;
        emit server_log( "!!! look here: " + line );

        return;
    }
    // ----------------------------------------------------------------------------------
    if ( state == State::starting_ssh )
    {
        if (line == started_label) {
            state = State::wait_start_label;
            grab_server();
        }
        return;
    }
    // ----------------------------------------------------------------------------------
    if ( state == State::in_ipsec )
    {
        if (line == ipsec_end_label) {
            process_ipsec();
            state = State::wait_start_label;
            return;
        }
    }
    if ( state == State::in_openvpn )
    {
        if (line == ovpn_end_label) {
            process_ovpn();
            state = State::wait_start_label;
            return;
        }
    }
    cout_lines << line;
    // ----------------------------------------------------------------------------------
}
//=======================================================================================
void Ssh_Process::process_ipsec()
{
    ipsec.process( cout_lines );
    cout_lines.clear();
    signal_update();
}
//=======================================================================================
void Ssh_Process::process_ovpn()
{
    ovpn.process( cout_lines );
    cout_lines.clear();
    signal_update();
}
//=======================================================================================
//=======================================================================================
//=======================================================================================
