#include "mainwindow.h"
#include "ui_mainwindow.h"


void stringTohexString(QString& str, QString& hexStr)
{

}

int hexStringToString(QString& hexStr, QString& str)
{
    int ret = 0;
    bool ok;
    QByteArray retByte;
    hexStr = hexStr.trimmed();
    hexStr = hexStr.simplified();
    QStringList sl = hexStr.split(" ");

    foreach (QString s, sl)
    {
        if(!s.isEmpty())
        {
            char c = (s.toInt(&ok,16))&0xFF;
            if (ok)
            {
                retByte.append(c);
            }
            else
            {
                ret = -1;
            }
        }
    }

    str = retByte;

    return ret;
}

int hexStringToHexArray(QString& hexStr, QByteArray& arr)
{
    int ret = 0;
    bool ok;
    hexStr = hexStr.trimmed();
    hexStr = hexStr.simplified();
    QStringList sl = hexStr.split(" ");

    foreach (QString s, sl)
    {
        if(!s.isEmpty())
        {
            char c = (s.toInt(&ok,16))&0xFF;
            if (ok)
            {
                arr.append(c);
            }
            else
            {
                ret = -1;
            }
        }
    }

    return ret;
}

int hexArrayToString(QByteArray& hexArr, QString& str)
{
    int ret = 0;
    str = hexArr.toHex(' ').toLower();
    return ret;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initMainWindow();
    initStatusBar();
    initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_stsExit) // 程序退出
    {
        //判断事件
        if(event->type() == QEvent::MouseButtonPress)
        {
            // TODO：直接退出还是发信号？
            emit sig_exit();
            return true; // 事件处理完毕
        }
        else
        {
            return false;
        }
    }
    else if (watched == m_stsPinned) // 置顶
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            static bool isTop = true;
            Qt::WindowFlags winFlags  = Qt::Dialog;
            winFlags = winFlags | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;
            if (isTop)
            {
                winFlags |= Qt::WindowStaysOnTopHint;
                setWindowFlags(winFlags);
                showNormal();
                QPixmap pinnedIcon(":/images/pinned.bmp");
                m_stsPinned->setPixmap(pinnedIcon);
                isTop = false;
            }
            else
            {
                winFlags  &= ~Qt::WindowStaysOnTopHint;
                setWindowFlags(winFlags);
                showNormal();
                QPixmap pinnedIcon(":/images/unpinned.bmp");
                m_stsPinned->setPixmap(pinnedIcon);
                isTop = true;
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (watched == m_stsResetCnt)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            m_stsRx->setText("RX: 0");
            m_stsTx->setText("TX: 0");
            m_rxCnt = m_txCnt = 0;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return QWidget::eventFilter(watched, event);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //当鼠标左键点击时.
    if (event->button() == Qt::LeftButton)
    {
        //记录鼠标的世界坐标
        m_startPos = event->globalPos();
        //记录窗体的世界坐标
        m_windowPos = this->frameGeometry().topLeft();
        m_pressMouse = 1;
    }
    else if(event->button() == Qt::RightButton)
    {

    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        if (m_pressMouse)
        {
        //移动中的鼠标位置相对于初始位置的相对位置
        QPoint relativePos = event->globalPos() - m_startPos;
        //然后移动窗体即可
        this->move(m_windowPos + relativePos );

        }
    }
    else if(event->button() == Qt::RightButton)
    {

    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressMouse = 0;
}

// 对主窗口的初始化
void MainWindow::initMainWindow()
{
    setWindowTitle(tr("QtTCP demo"));
    setMinimumSize(480, 320);

    Qt::WindowFlags winFlags  = Qt::Dialog;
    winFlags = winFlags | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;

    setWindowFlags(winFlags);
}

void MainWindow::initWindow()
{
    m_pressMouse = 0;
    m_recvHex = 0;
    m_sendHex = 0;
    m_sendTimer = 0;
    m_sendTimerId = 0;

    m_showTimestamp = 0;
    m_sendNewline = 0;

    m_rxCnt = 0;
    m_txCnt = 0;

    /////////////////////////////////////////////////////////////

    ui->btnClearRecv->setFlat(true);
    ui->btnClearSend->setFlat(true);

    ui->txtRecv->setPlaceholderText("Receive Data here");
    ui->txtSend->setPlaceholderText("Send Data here");
    ui->txtSend->setPlainText("hello world");

    ui->btnOpen->setText(tr("监听端口"));
    ui->btnOpen->setIconSize(ui->btnOpen->rect().size());
    ui->btnOpen->setIcon(QIcon(":images/notopened.ico"));

    ui->btnConnect->setText(tr("连接"));
    ui->btnConnect->setIconSize(ui->btnOpen->rect().size());
    ui->btnConnect->setIcon(QIcon(":images/notopened.ico"));

    ui->txtInterval->setText("1000");
    /////////////////////////////////////////////////////////////
    QStringList list;
    list.clear();
    list << "127.0.0.1";
    ui->cbLocalIP->addItems(list);

    list.clear();
    list << "8000";
    ui->cbLocalPort->addItems(list);

    list.clear();
    list << "127.0.0.1";
    ui->cbRemoteIP->addItems(list);

    list.clear();
    list << "8000";
    ui->cbRemotePort->addItems(list);

    ui->txtRecv->setPlaceholderText("Receive data here");
    ui->txtSend->setPlaceholderText("Send data here");
    ui->txtSend->setPlainText("hello world");

    // client
    m_tcpSocket = new QTcpSocket();
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(cli_connected())); // 客户端连接
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(cli_disconnected())); // 客户端断开连接
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(cli_receiveData())); // 客户端接收数据

    // server
    m_tcpServer = new QTcpServer();
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(svr_newConnect()));
}


void MainWindow::initStatusBar()
{
    // 状态栏分别为：
    // 置顶图标  注：与退出图标似乎无法同时显示，先舍弃
    // 提示信息（可多个）
    // RX、TX
    // 版本信息（或版权声明）
    // 退出图标
    ui->statusbar->setMinimumHeight(22);
    //ui->statusbar->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 不显示边框
    ui->statusbar->setSizeGripEnabled(false);//去掉状态栏右下角的三角

    m_stsPinned = new QLabel();
    m_stsDebugInfo = new QLabel();
    m_stsRx = new QLabel();
    m_stsTx = new QLabel();
    m_stsResetCnt = new QLabel();
    m_stsCopyright = new QLabel();
    m_stsExit = new QLabel();

    // 置顶图标
    m_stsPinned->installEventFilter(this); // 安装事件过滤，以便获取其单击事件
    m_stsPinned->setMinimumWidth(20);
    // 贴图
    QPixmap pinnedIcon(":/images/unpinned.bmp");
    m_stsPinned->setPixmap(pinnedIcon);
    ui->statusbar->addWidget(m_stsPinned);


    m_stsDebugInfo->setMinimumWidth(this->width()/2);
    ui->statusbar->addWidget(m_stsDebugInfo);

    m_stsRx->setMinimumWidth(64);
    ui->statusbar->addWidget(m_stsRx);
    m_stsRx->setText("RX: 0");

    m_stsTx->setMinimumWidth(64);
    ui->statusbar->addWidget(m_stsTx);
    m_stsTx->setText("TX: 0");

    m_stsResetCnt->installEventFilter(this);
    m_stsResetCnt->setFrameStyle(QFrame::Plain);
    m_stsResetCnt->setText("复位计数");
    m_stsResetCnt->setMinimumWidth(32);
    ui->statusbar->addWidget(m_stsResetCnt);

    printDebugInfo("欢迎使用");
    // 版权信息
    m_stsCopyright->setFrameStyle(QFrame::NoFrame);
    m_stsCopyright->setText(tr("  <a href=\"https://www.latelee.org\">技术主页</a>  "));
    m_stsCopyright->setOpenExternalLinks(true);
    ui->statusbar->addPermanentWidget(m_stsCopyright);


    // 退出图标
    m_stsExit->installEventFilter(this); // 安装事件过滤，以便获取其单击事件
    m_stsExit->setToolTip("Exit App");
    m_stsExit->setMinimumWidth(32);
    // 贴图
    QPixmap exitIcon(":/images/exit.png");
    m_stsExit->setPixmap(exitIcon);
    ui->statusbar->addPermanentWidget(m_stsExit);

    connect(this, &MainWindow::sig_exit, qApp, &QApplication::quit); // 直接关联到全局的退出槽

}

void MainWindow::printDebugInfo(QString str)
{
    m_stsDebugInfo->setText(str);
}

void MainWindow::printDebugInfo(const char* str)
{
    QString tmp = str;
    m_stsDebugInfo->setText(tmp);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    //qDebug() << "Timer ID:" << event->timerId();
    sendData();
}

void MainWindow::sendData()
{
    if (!m_tcpSocket->isOpen())
    {
        printDebugInfo("not connected");
        return;
    }
    QString sendStr = ui->txtSend->toPlainText().toLatin1().toLower();
    QByteArray sendData;
    QString showStr;

    if (m_sendHex == 1)
    {
        hexStringToHexArray(sendStr, sendData);
    }
    else if (m_sendHex == 0)
    {
        sendData = sendStr.toLatin1();
    }
    if (m_sendNewline == 1)
    {
        sendData.append(0x0d);
        sendData.append(0x0a);
    }
    //qDebug() << sendData;
//    char*  ch;
//    //QByteArray ba = ui->cliText->text().toLatin1(); // must
//    ch=ba.data();


    m_tcpSocket->write(sendData, sendData.size());

    m_txCnt += sendData.size();
    m_stsTx->setText("TX: " + QString::number(m_txCnt));
}

void MainWindow::sendHexData(QString& tips, uint8_t* ibuf, uint8_t ilen)
{
    QByteArray sendData;

    for (int i = 0; i < ilen; i++)
    {
        sendData[i] = ibuf[i];
    }

    QString tmp = sendData.toHex(' ').toLower();

    //ui->txtSend->appendPlainText(tips + tmp);
}

void MainWindow::readyRead()
{
    QTcpSocket *tcpSocket = static_cast<QTcpSocket *>(QObject::sender());
    QByteArray buffer = tcpSocket->readAll();
    QString info;
    QString tmpStr;
    QString timeStr = "";

    m_rxCnt += buffer.size();
    m_stsRx->setText("RX: " + QString::number(m_rxCnt));

    if (m_showTimestamp)
    {
        QDateTime dateTime(QDateTime::currentDateTime());
        timeStr = "[" + dateTime.toString("yyyy-MM-dd HH:mm::ss.zzz") + "] ";
    }

    if (m_recvHex == 1)
    {
        info = buffer.toHex(' ').data();
    }
    else
    {
        info = QString(buffer);
    }

    ui->txtRecv->appendPlainText(timeStr + info);
    
    // TODO
    // 似乎直接用QByteArray无法直接取真正的值
    // 这里先转为数组，再判断，需要优化
    uint8_t *data = (uint8_t*)buffer.data();
    uint8_t buf[255] = {0};
    for (int i = 0; i < buffer.size(); i++)
    {
        buf[i] = data[i];
        //qDebug("%x ", buf[i]);
    }
    
    // 根据值判断做逻辑处理，可做成函数
    if (buf[0] == 0xaa && buf[1] == 0x55)
    {

    }
}


void MainWindow::svr_newConnect()
{
    printDebugInfo("get new connect");
    QTcpSocket *tcpSocket = m_tcpServer->nextPendingConnection();//新的客户端发起的连接
    QHostAddress clientIp = tcpSocket->peerAddress();//客户端的IP
    quint16 port = tcpSocket->peerPort();//客户端的端口
    if(m_clientList.contains(tcpSocket))
    {
        printDebugInfo(QString("%1:%2 already connected").arg(clientIp.toString()).arg(port));
    }
    else
    {
        printDebugInfo("new connect");
        m_clientList.append(tcpSocket);//记录下客户端发起的连接
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(svr_disconnect()));
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead())); // 数据接收

    }
}

void MainWindow::svr_disconnect(void)
{
    printDebugInfo(tr("disconnect"));
    QTcpSocket *tcpSocket = static_cast<QTcpSocket *>( QObject::sender() );
    m_clientList.removeOne(tcpSocket);
}

/////////////////////////////////////
void MainWindow::cli_connected()
{
    printDebugInfo("connected");
}

void MainWindow::cli_disconnected()
{
    printDebugInfo("disconnect");
}

void MainWindow::cli_receiveData()
{
    QByteArray ba;
    ba = m_tcpSocket->readAll();
    printDebugInfo(QString(ba));
}

void MainWindow::on_btnOpen_clicked()
{
    if(ui->btnOpen->text()==QString("监听端口"))
    {
        QString str=ui->cbLocalIP->currentText();
        uint16_t port=ui->cbLocalPort->currentText().toUShort();
        m_tcpServer->close();

        m_tcpServer->listen(QHostAddress::Any, port);
        printDebugInfo(QString("listening on %1").arg(port));

        ui->btnOpen->setText(tr("关闭监听"));
        ui->btnOpen->setIcon(QIcon(":images/opened.ico"));
    }
    else
    {
        m_tcpServer->close();
        uint16_t port=ui->cbLocalPort->currentText().toUShort();
        printDebugInfo(QString("close port %1").arg(port));
        ui->btnOpen->setText(tr("监听端口"));
        ui->btnOpen->setIcon(QIcon(":images/notopened.ico"));
    }
}

void MainWindow::on_btnSaveRecv_clicked()
{

}

void MainWindow::on_btnClearRecv_clicked()
{
    ui->txtRecv->clear();
}

void MainWindow::on_ckRecvHex_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_recvHex = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_recvHex = 0;
    }
}

void MainWindow::on_ckTimestamp_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_showTimestamp = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_showTimestamp = 0;
    }
}

void MainWindow::on_btnSend_clicked()
{
    if (m_sendTimer)
    {
        if(ui->btnSend->text()==QString("发送"))
        {
            if (m_sendTimerId == 0)
                m_sendTimerId = startTimer(ui->txtInterval->text().toInt());
            ui->btnSend->setText(tr("停止发送"));
        }
        else
        {
            if (m_sendTimerId)
            {
                killTimer(m_sendTimerId);
                m_sendTimerId = 0;
            }
            ui->btnSend->setText(tr("发送"));
        }
    }
    else
    {
        sendData();
    }
}

void MainWindow::on_btnClearSend_clicked()
{
    ui->txtSend->clear();
}

void MainWindow::on_chSendHex_stateChanged(int arg1)
{
    QString sendStr = ui->txtSend->toPlainText().toLatin1().toLower();
    QByteArray sendData = sendStr.toLatin1();//ui->txtSend->toPlainText().toLatin1();
    QString tmpStr;
    QString showStr;
    if (arg1 == Qt::Checked)
    {
        m_sendHex = 1;
        showStr = sendData.toHex(' ').data();
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_sendHex = 0;
        hexStringToString(sendStr, showStr); // 用string来转
    }
    ui->txtSend->setPlainText(showStr);
}

void MainWindow::on_ckSendTimer_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_sendTimer = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_sendTimer = 0;
        if (m_sendTimerId)
        {
            killTimer(m_sendTimerId);
            m_sendTimerId = 0;
            ui->btnSend->setText("发送");
        }
    }
}

void MainWindow::on_chSendNewline_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_sendNewline = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_sendNewline = 0;
    }
}

void MainWindow::on_btnConnect_clicked()
{
    if(ui->btnConnect->text()==QString("连接"))
    {
        QHostAddress serverIp;
        if(!serverIp.setAddress(ui->cbRemoteIP->currentText()))
        {
           printDebugInfo("ip error");
           return;
        }

        if (m_tcpSocket->isOpen())
            m_tcpSocket->close();
        uint16_t port=ui->cbRemotePort->currentText().toUShort();
        m_tcpSocket->connectToHost(serverIp, port);
        if (!m_tcpSocket->isOpen())
        {
            printDebugInfo("connect failed");
            return;
        }

        ui->btnConnect->setText(tr("关闭连接"));
        ui->btnConnect->setIcon(QIcon(":images/opened.ico"));
    }
    else
    {
        m_tcpSocket->close();
        ui->btnConnect->setText(tr("连接"));
        ui->btnConnect->setIcon(QIcon(":images/notopened.ico"));
    }
}
