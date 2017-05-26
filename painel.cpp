#include "painel.h"
#include "ui_painel.h"
#include "qcustomplot.h"
#include "signal.h"
#include "math.h"

/** variaveis auxiliares usadas na geração das ondas dente de serra e random */
double tempoDenteSerra = 0;
double tempoRandom = 0;




// tempo do timer ms
//double r;

Painel::Painel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Painel)
{
    ui->setupUi(this);

    this->T = 100;
    offSet = 10;
    this->amplitude = 10;

    this->bombaLigada = 1;

    this->sinal = 5.0 * ui->hs_sinal_bomba->value() /100;

    tipoOnda = 0;

    ui->pbTanque1->setValue(0);
    ui->pbTanque2->setValue(0);

    this->tanque = new Tanque();

    //configura timer do tanque virtual
    timerTanqueVirtual= new QTimer(this);
    threadTanqueVirtual = new QThread(0);

    timerTanqueVirtual->moveToThread(threadTanqueVirtual);
    connect(timerTanqueVirtual, SIGNAL(timeout()), this, SLOT(atualizaTanqueVirtual()));

    this->timerTanqueVirtual->start(100);
    this->threadTanqueVirtual->start();

    this->configEscrita();
    this->configLeitura();
}

Painel::~Painel()
{
    delete ui;
}

void Painel::configLeitura()
{
    // Nivel do tanque1
    ui->wdgPlotNivel->addGraph();
    ui->wdgPlotNivel->graph(0)->setPen(QPen(Qt::black));
    ui->wdgPlotNivel->graph(0)->setAntialiasedFill(false);
    ui->wdgPlotNivel->graph(0)->setName("Nivel do tanque1");

    //Nivel do tanque2
    ui->wdgPlotNivel->addGraph();
    ui->wdgPlotNivel->graph(1)->setPen(QPen(Qt::blue));
    ui->wdgPlotNivel->graph(1)->setAntialiasedFill(false);
    ui->wdgPlotNivel->graph(1)->setName("Nivel do tanque2");




    ui->wdgPlotNivel->legend->setVisible(true);

    ui->wdgPlotNivel->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->wdgPlotNivel->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->wdgPlotNivel->xAxis->setAutoTickStep(false);
    ui->wdgPlotNivel->xAxis->setTickStep(2);


    ui->wdgPlotNivel->yAxis->setRange(0,30);
    ui->wdgPlotNivel->yAxis->setNumberPrecision(2);
    ui->wdgPlotNivel->yAxis->setLabel("Nivel do tanque (Cm) ");
    ui->wdgPlotNivel->xAxis->setLabel("Tempo");

    // make left and bottom axes transfer their ranges to right and top axes:
    // connect(ui->wdgPlotNivel->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->wdgPlotNivel->xAxis2, SLOT(setRange(QCPRange)));
    //connect(ui->wdgPlotNivel->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->wdgPlotNivel->yAxis2, SLOT(setRange(QCPRange)));

     timerLeitura = new QTimer(this);
     thrLeitura = new QThread();

    //configura o timer pra executar a função continuamente
    connect(timerLeitura, SIGNAL(timeout()), this, SLOT(lerDadosPlanta()));

    timerLeitura->moveToThread(thrLeitura);

}

void Painel::lerDadosPlanta()
{
    // calculate two new data points:
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;


    // ler o sinal do servidor aqui

    this->nivelTq1 = this->tanque->getNivelTq1();
    this->nivelTq2 = this->tanque->getNivelTq2();

    double pctTq1 = (this->nivelTq1/30)*100;
    double pctTq2 = (this->nivelTq2/30)*100;

    ui->pbTanque1->setValue(pctTq1);
    ui->pbTanque2->setValue(pctTq2);

    ui->wdgPlotNivel->graph(0)->addData(key/5, this->nivelTq1);
    ui->wdgPlotNivel->graph(1)->addData(key/5,this->nivelTq2);

    // remove data of lines that's outside visible range:
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->wdgPlotNivel->xAxis->setRange((key+0.25)/5, 8, Qt::AlignRight);
    ui->wdgPlotNivel->replot();
}

void Painel::configEscrita()
{

    timerEscrita= new QTimer(this);
    thrEscrita = new QThread(0);

    //configura o timer pra executar a função continuamente
    connect(timerEscrita, SIGNAL(timeout()), this, SLOT(enviaDadosPlanta()));

    timerEscrita->moveToThread(thrEscrita);

}

void Painel::resetaPlanta()
{
    ui->pbTanque1->setValue(0);
    ui->pbTanque2->setValue(0);
}


void Painel::enviaDadosPlanta()
{


    this->sinal = this->getSinalControle();

    if(this->bombaLigada == 0)
        this->sinal = 0;

    this->sinalSaturado = tanque->acionaBomba(this->sinal);

    ui->lnEdtNivelTq1->setText(QString::number(this->nivelTq1));
    ui->lnEdtNivelTq2->setText(QString::number(this->nivelTq2));

    this->t += 0.1;

    tempoDenteSerra += 0.1;
    if(tempoDenteSerra > T)
        tempoDenteSerra = 0;

    tempoRandom += 0.1;
}

double Painel::getSinalControle()
{
    return this->sinal;
}

void Painel::atualizaTanqueVirtual()
{
    this->tanque->escoaTanque1(this->nivelTq1, this->torneira_tq1);
    this->tanque->escoaTanque2(this->nivelTq2, this->torneira_tq2);
}

/**
 * @brief Painel::on_pushButton_2_clicked
 * Inicia a leitura e a esrita na planta
 */
void Painel::on_pushButton_2_clicked()
{
    this->timerTanqueVirtual->start(100);
    this->threadTanqueVirtual->start();

    this->timerEscrita->start(100);
    this->thrEscrita->start();

    this->timerLeitura->start(100);
    this->thrLeitura->start();
}

void Painel::on_chbShowSetPointNivel_clicked(bool checked)
{
    ui->wdgPlotNivel->graph(1)->setVisible(checked);
}

void Painel::on_chbSetPointSinal_clicked(bool checked)
{
    ui->wdgPlotNivel->graph(1)->setVisible(checked);
}

void Painel::on_chbErro_clicked(bool checked)
{
    ui->wdgPlotNivel->graph(2)->setVisible(checked);
}

void Painel::on_chbSetPointNivel_clicked(bool checked)
{
    ui->wdgPlotNivel->graph(1)->setVisible(checked);
}

void Painel::on_chbTq1_clicked(bool checked)
{
    ui->wdgPlotNivel->graph(0)->setVisible(checked);
}

void Painel::on_chbTq1_2_clicked(bool checked)
{
    ui->wdgPlotNivel->graph(3)->setVisible(checked);
}

/**
 * @brief Painel::on_checkBox_2_clicked
 * @param checked
 * Liga/Desliga a bomba do tanque 1
 */
void Painel::on_checkBox_2_clicked(bool checked)
{
    if(checked){
        this->bombaLigada = 1;
        this->sinal = 5.0 * ui->hs_sinal_bomba->value() /100;
    }
    else{
        this->bombaLigada = 0;
    }
}

void Painel::on_hs_sinal_bomba_valueChanged(int value)
{
    this->sinal = 10.0 * value /100;
    ui->lb_tensao_bomba->setText(QString::number(this->sinal) + " V");
}

void Painel::on_hs_torneira_tq1_valueChanged(int value)
{
    double torneira = 1.0*ui->hs_torneira_tq1->value()/100;
    this->torneira_tq1 = torneira;
    ui->lb_torneira_tq1->setText("Torneira: " + QString::number(torneira*100) + " %");
}

void Painel::on_hs_torneira_tq2_valueChanged(int value)
{
    double torneira = 1.0*ui->hs_torneira_tq2->value()/100;
    this->torneira_tq2 = torneira;
    ui->lb_torneira_tq2->setText("Torneira: " + QString::number(torneira*100) + " %");
}



void Painel::on_checkBox_4_clicked(bool checked)
{
    if (checked)
        this->torneira_tq1 = 1.0*ui->hs_torneira_tq1->value()/100;
    else
        this->torneira_tq1 = 0;
}

void Painel::on_checkBox_clicked(bool checked)
{
    if (checked)
        this->torneira_tq2 = 1.0*ui->hs_torneira_tq2->value()/100;
    else
        this->torneira_tq2 = 0;
}
