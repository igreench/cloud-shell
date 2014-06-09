#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_lineEdit_returnPressed();
    void readyRead();
    void connected();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    void send(QString s);
    QTcpSocket *socket;
    bool isConnected;
    int port;
    QString host;
    void print(QString s);
    void connectToHost();
};

#endif // MAINWINDOW_H
