#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QHostAddress>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isConnected = false;
    ui->textEdit->append("Hello, User!");
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_returnPressed()
{
    connectToHost();
}

void MainWindow::connectToHost() {
    if (ui->lineEdit->text() != "clear") {
        if (!ui->lineEdit->text().isEmpty()) {
            host = ui->lineEdit_3->text();
            port = ui->lineEdit_2->text().toInt();
            socket->connectToHost(host, port);
        }
    } else {
        ui->textEdit->clear();
        ui->lineEdit->clear();
    }
}

void MainWindow::send(QString s) {
    print("shell: " + s);
    if (isConnected) {
        socket->write(QString(s).toUtf8());
    }
}

void MainWindow::readyRead()
{
    while(socket->canReadLine()) {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        print(line);
    }
}

void MainWindow::connected()
{
    isConnected = true;
    send(ui->lineEdit->text());
    ui->lineEdit->clear();
}

void MainWindow::print(QString s) {
    ui->textEdit->append(s);
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_3_clicked()
{
    connectToHost();
}
