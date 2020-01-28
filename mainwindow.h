#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QDateTime>

#include <QTcpServer>
#include <QTcpSocket>

#pragma execution_character_set("utf-8")

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:

    void initMainWindow();
    void initWindow();
    void initStatusBar();

    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *event); // 鼠标按下
    void mouseMoveEvent(QMouseEvent *event); // 鼠标移动
    void mouseReleaseEvent(QMouseEvent *event); // 鼠标释放

    void timerEvent(QTimerEvent *event);

    void sendData();
    void sendHexData(QString& tips, uint8_t* ibuf, uint8_t ilen);

    void printDebugInfo(QString str);
    void printDebugInfo(const char* str);

    void showRecvData(QString tips, QByteArray& buffer);

    QTcpServer *m_tcpServer;

    QList<QTcpSocket*> m_clientList;
    QTcpSocket *m_tcpCliSocket;

    int m_pressMouse;
    QPoint m_startPos;
    QPoint m_windowPos;

    int m_recvHex;
    int m_sendHex;
    int m_sendTimer;

    int m_showTimestamp;
    int m_sendNewline;

    int m_sendTimerId;

    int m_rxCnt;
    int m_txCnt;

    int m_cliConnected;
    int m_svrConnected;

    // 状态栏相关
    QLabel* m_stsPinned;
    QLabel* m_stsDebugInfo;
    QLabel* m_stsRx;
    QLabel* m_stsTx;
    QLabel* m_stsResetCnt;
    QLabel* m_stsCopyright;

    QLabel* m_stsExit;

signals:
    void sig_exit();

private slots:
    void readyRead();

    void svr_newConnect(void);
    void svr_disconnect(void);

    void cli_connected(void);
    void cli_disconnected(void);
    void cli_receiveData(void);

    void on_btnOpen_clicked();

    void on_btnSaveRecv_clicked();

    void on_btnClearRecv_clicked();

    void on_ckRecvHex_stateChanged(int arg1);

    void on_ckTimestamp_stateChanged(int arg1);

    void on_btnSend_clicked();

    void on_btnClearSend_clicked();

    void on_chSendHex_stateChanged(int arg1);

    void on_ckSendTimer_stateChanged(int arg1);

    void on_chSendNewline_stateChanged(int arg1);

    void on_btnConnect_clicked();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
