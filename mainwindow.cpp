
#include "mainwindow.h"
#include <qtoolbar.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

QT_USE_NAMESPACE

class MyToolBar: public QToolBar
{
public:
    MyToolBar( MainWindow *parent ):
        QToolBar( parent )
    {
    }
    void addSpacing( int spacing )
    {
        QLabel *label = new QLabel( this );
        addWidget( label );
        label->setFixedWidth( spacing );
    }
};
 
MainWindow::MainWindow()
{
    addToolBar(Qt::TopToolBarArea, toolBar());
    //setCentralWidget( toolBar() );
    this->setWindowTitle(" ");
 
	/* Инициализируем иконку трея, устанавливаем иконку из набора системных иконок,
	 * а также задаем всплывающую подсказку
	 * */
	trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_VistaShield));

    //SP_MediaVolume //SP_ComputerIcon
	trayIcon->setToolTip("Tray Program" "\n"
						 "Работа со сворачиванием программы трей");
	/* После чего создаем контекстное меню из двух пунктов*/
	QMenu * menu = new QMenu(this);
	QAction * viewWindow = new QAction(trUtf8("Развернуть окно"), this);
    QAction * quitAction = new QAction(trUtf8("Выход"), this);\
    //QAction * quitAction = new QAction(trUtf8("Свернуть"), this);
 
	/* подключаем сигналы нажатий на пункты меню к соответсвующим слотам.
	 * Первый пункт меню разворачивает приложение из трея,
	 * а второй пункт меню завершает приложение
	 * */
	connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    //connect(quitAction, SIGNAL(triggered()), this, SLOT(hide()));
 
	menu->addAction(viewWindow);
	menu->addAction(quitAction);
 
	/* Устанавливаем контекстное меню на иконку
	 * и показываем иконку приложения в трее
	 * */
	trayIcon->setContextMenu(menu);
	trayIcon->show();
 
	/* Также подключаем сигнал нажатия на иконку к обработчику
	 * данного нажатия
	 * */
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    QFont font("Times", 98, QFont::Bold);
    label->setFont(font);
    label->setText(QTime::currentTime().toString("hh:mm:ss.zzz"));
    label2->setFont(font);
    //label2->setText(QDate::currentDate().toString("ddd"));
    label2->setText(QString::number(QDate::currentDate().dayOfWeek()));

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    timer->start(1000);

    //this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    mTranscodingProcess = new QProcess(this);
    mInputPlayProcess = new QProcess(this);
    mOutputPlayProcess = new QProcess(this);
    connect(mTranscodingProcess, SIGNAL(started()), this, SLOT(processStarted()));
    connect(mTranscodingProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
    connect(mTranscodingProcess, SIGNAL(finished(int)), this, SLOT(encodingFinished()));

    openFile();

    //toStartProcess(input, ss_time, to_time, output);

}

void MainWindow::openFile(){
    QString str;
    str = "H:/W10/git/shayan/program/";
    switch (QDate::currentDate().dayOfWeek()) {
        case 1: str.append("1"); break;
        case 2: str.append("2"); break;
        case 3: str.append("3"); break;
        case 4: str.append("4"); break;
        case 5: str.append("5"); break;
        case 6: str.append("6"); break;
        case 7: str.append("7"); break;
    }
    str.append(".txt");
    //qDebug()<< "str" << str;
    sendText("str: ");
    sendText(str);
    sendText("\n");

    QFile file (str);
    if(QFileInfo(str).exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QStringList list_temp;
        QString timeQTime;
        QStringList argumentsQStringList;
        QString line;

        QTextStream in(&file);
        while (!in.atEnd()) {
            line = in.readLine();
            list_temp = line.split("\t");
            timeQTime = list_temp[0];
            for (int i=1; i<=4; i++){
                argumentsQStringList.append(list_temp[i]);
            }
            programQMap[timeQTime]=argumentsQStringList;
            argumentsQStringList.clear();
            sendText("line: ");
            sendText(line);
            sendText("\n");
            //qDebug()<< "line" << line;
        }
        file.flush();
        file.close();
    }
    //qDebug() << "programQMap" <<  programQMap;

}


void MainWindow::toStartProcess(QString input,  QString ss_time,  QString to_time,  QString output)
{
    QString program = "H:/W10/git/shayan/ffmpeg/bin/ffmpeg";
    QStringList arguments;
    QString codec1 = "copy";
    QString codec2 = "copy";
    arguments << "-i" << input << "-ss" << ss_time << "-to" << to_time << "-acodec" << codec1 << "-vcodec" << codec2 << output;
    mTranscodingProcess->setProcessChannelMode(QProcess::MergedChannels);
    mTranscodingProcess->start(program, arguments);
    //qDebug() << arguments;
    sendText(arguments.join(";"));
    sendText("\n");
}

void MainWindow::processStarted()
{
    //qDebug() << "processStarted()";
    sendText("processStarted(): ");
}

void MainWindow::sendText(QString textQString)
{
    mOutputString2.append(textQString);
    textEdit2->setText(mOutputString2);
    textEdit2->moveCursor(QTextCursor::End);
}


void MainWindow::readyReadStandardOutput()
{
    mOutputString.append(mTranscodingProcess->readAllStandardOutput());
    textEdit->setText(mOutputString);
    textEdit->moveCursor(QTextCursor::End);
}

void MainWindow::encodingFinished()
{
    sendText("processFinished() \n");
}



void MainWindow::slotTimerAlarm()
{
    /* Ежесекундно обновляем данные по текущему времени
     * Перезапускать таймер не требуется
     * */
    QString timeQString =QTime::currentTime().toString("hh:mm:ss");

    label->setText(timeQString);
    if (programQMap.contains(timeQString) ) {


        QString input = programQMap.value(timeQString).at(2);
        QString output = programQMap.value(timeQString).at(3);

        QDir dirconf;
        dirconf.mkpath(output);

        QDateTime date=QDateTime::currentDateTime();
        QString dir = date.toString("yyyy.MM.dd_hh.mm.ss");
        output.append(dir);
        output.append(".mp4");

        QString ss_time = "00:00:00";
        QString to_time = programQMap.value(timeQString).at(1);
        //qDebug() << "input:" << input <<  " ss_time:" << ss_time <<  " to_time:" << to_time <<  " output:" << output;
        toStartProcess(input, ss_time, to_time, output);
    }
    if (timeQString == "00:03:00"){
        programQMap.clear();

        //qDebug() << "clear programQMap at " << QDateTime::currentDateTime().toString("yyyy.MM.dd_hh.mm.ss");
        sendText("clear programQMap at ");
        sendText(QDateTime::currentDateTime().toString("yyyy.MM.dd_hh.mm.ss"));
        sendText("\n");

        openFile();
    }
}



/* Метод, который обрабатывает событие закрытия окна приложения
 * */
void MainWindow::closeEvent(QCloseEvent * event)
{
	/* Если окно видимо и чекбокс отмечен, то завершение приложения
	 * игнорируется, а окно просто скрывается, что сопровождается
	 * соответствующим всплывающим сообщением
	 */
    if(this->isVisible() && 1){
		event->ignore();
		this->hide();
		QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
 
        /*trayIcon->showMessage("Tray Program",
							  trUtf8("Приложение свернуто в трей. Для того чтобы, "
									 "развернуть окно приложения, щелкните по иконке приложения в трее"),
							  icon,
                              2000);*/
	}
}
 
/* Метод, который обрабатывает нажатие на иконку приложения в трее
 * */
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason){
	case QSystemTrayIcon::Trigger:
		/* Событие игнорируется в том случае, если чекбокс не отмечен
		 * */
        if(1){
			/* иначе, если окно видимо, то оно скрывается,
			 * и наоборот, если скрыто, то разворачивается на экран
			 * */
			if(!this->isVisible()){
				this->show();
			} else {
				this->hide();
			}
		}
		break;
	default:
		break;
	}
}

QToolBar *MainWindow::toolBar()
{

    MyToolBar *toolBar = new MyToolBar( this );
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    hBox = new QWidget( toolBar );
    hBox->isActiveWindow();

    //d_OpenWindow_S = toolBar->toggleViewAction();

    label  = new QLabel(tr(""));
    label2  = new QLabel(tr(""));
    textEdit = new QTextEdit(tr(""));
    textEdit2 = new QTextEdit(tr(""));

    QGridLayout *mainLayout = new QGridLayout( hBox );

    mainLayout->addWidget(label, 0, 0);
    //mainLayout->addWidget(label2, 1,0);
    mainLayout->addWidget(textEdit, 2,0);
    mainLayout->addWidget(textEdit2, 3,0);


    toolBar->addWidget( hBox );

    return toolBar;
}
