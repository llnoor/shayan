#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <qmainwindow.h>
#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QStyle>
#include <QFont>
#include <QTimer>
#include <QTime>
#include <QDate>
#include <QLabel>
#include <QTextEdit>
#include <QProcess>
 
class QCheckBox;
class QMenu;
class QComboBox;
 
class MainWindow : public QMainWindow
{
	Q_OBJECT
 
public:
    MainWindow();

protected:
	/* Виртуальная функция родительского класса в нашем классе
	 * переопределяется для изменения поведения приложения,
	 *  чтобы оно сворачивалось в трей, когда мы этого хотим
	 */
	void closeEvent(QCloseEvent * event);
 
private slots:
	/* Слот, который будет принимать сигнал от события
	 * нажатия на иконку приложения в трее
	 */
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void slotTimerAlarm();
    void readyReadStandardOutput();
    void processStarted();
    void encodingFinished();
    void toStartProcess(QString input,  QString ss_time,  QString to_time,  QString output);
    void openFile();
    void sendText(QString);

private:
    QTimer *timer;
    QToolBar *toolBar();
    QWidget *hBox;
	/* Объявляем объект будущей иконки приложения для трея */
	QSystemTrayIcon         * trayIcon;
    QLabel *label;
    QLabel *label2;
    QTextEdit *textEdit;
    QTextEdit *textEdit2;
    QProcess *mTranscodingProcess;
    QProcess *mInputPlayProcess;
    QProcess *mOutputPlayProcess;
    QString mOutputString;
    QString mOutputString2;
    QMap<QString, QStringList> programQMap;
};
 
#endif // MAINWINDOW_H
