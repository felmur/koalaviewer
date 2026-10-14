// ********************************************************
// koalaviewer
// c64 KOALA images viewer (.kla or .koa extension)
// (c) 2026 by Felice Murolo - Salerno, Italia
// email: linuxboy@fel.hopto.org
// ********************************************************

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>

#include <iostream>
using namespace std;
#define TC(x) x.toLocal8Bit().data()

#include "c64view.h"
C64View *c64view=nullptr;

QString prgname="koalaviewer";
QString version="1.0a";
QString copyright="(c) 2026 by Felice Murolo - Salerno, Italia";

uint8_t header[2];
uint8_t bitmap[8000];
uint8_t chars[1000];
uint8_t colors[1000];
uint8_t bg=0;

const int C64_WIDTH = 403;
const int C64_HEIGHT = 284;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QString filename;

    ui->setupUi(this);

    c64view = new C64View();
    ui->vlayout->addWidget(c64view);
    setWindowTitle(prgname+" v"+version+" - "+copyright);
    setWindowIcon(QIcon(":/images/logo"));

    filename = "";
    QStringList args = qApp->arguments();
    if (args.length()>1){
        if (args[1] == "-h" || args[1] == "--help") usage();
        else filename = args[1];
    }
    else filename = ":/images/tiger";

    showfile(filename);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::usage()
{
    cout << "Usage: " << TC(prgname) << " <koala_image_file>" << endl;
    QApplication::exit(0);
}

void MainWindow::showfile(QString filename)
{
    uint32_t color;

    if (filename.length()){
        QFile f(filename);
        if (f.open(QFile::ReadOnly)){
            if (f.size() == 10003){
                cout << "Loading file..." << endl;
                f.read(reinterpret_cast<char *>(header),2);
                f.read(reinterpret_cast<char *>(bitmap),8000);
                f.read(reinterpret_cast<char *>(chars),1000);
                f.read(reinterpret_cast<char *>(colors),1000);
                f.read((char *)&bg,1);
            }
            f.close();
        }
        for (uint px=0; px<C64_WIDTH*C64_HEIGHT; px++){
            c64view->pixelBuffer[px]=palette[bg & 0x0F];
        }
        c64view->updateFrame();

        for (uint py=0; py<C64_HEIGHT; py++){
            for (uint px=0; px<C64_WIDTH; px+=2){
                if (py>=42 && py<200+42){
                    if (px>=42 && px<320+42){
                        // qui siamo nell'area visibile
                        // Coordinate relative all'immagine Koala 320x200 (pixel doppi -> 160x200)
                        uint xr = px-42;              // 0..319
                        uint yr = py-42;              // 0..199
                        uint charCol = xr/8;          // 0..39
                        uint charRow = yr/8;          // 0..24
                        uint rowInChar = yr%8;        // riga dentro la cella 8x8
                        // Layout VIC-II: gli 8 byte di ogni cella 8x8 sono contigui:
                        // offset = (charRow*40 + charCol)*8 + rowInChar
                        uint bm = (charRow*40 + charCol)*8 + rowInChar; // 0..7999
                        uint bmb = bitmap[bm];      // byte letto dall'array della bitmap
                        uint pair = (xr%8)/2;       // quale coppia di pixel (0..3 da sinistra a destra)
                        uint shift = (3-pair)*2;    // i bit alti sono a sinistra
                        uint pix = (bmb>>shift) & 0x03; // valore coppia pixel %00..%11
                        uint ch = charRow*40+charCol;   // indice 0..999 in memoria schermo/colori
                        uint8_t col01 = (chars[ch]>>4) & 0x0F; // colore per la coppia pixel %01 (nibble alto)
                        uint8_t col10 = chars[ch] & 0x0F;     // colore per la coppia pixel %10 (nibble basso)
                        uint8_t mcol = colors[ch] & 0x0F;     // colore per la coppia pixel %11
                        if (pix==0) color = palette[bg & 0x0F];
                        else if (pix==1) color = palette[col01];
                        else if (pix==2) color = palette[col10];
                        else color = palette[mcol];
                        c64view->pixelBuffer[py*C64_WIDTH+px]=color;
                        c64view->pixelBuffer[py*C64_WIDTH+px+1]=color;
                    }
                }
            }
        }
        c64view->updateFrame();
    }
}

void MainWindow::exit()
{
    QApplication::exit(0);
}

void MainWindow::loadimage()
{
    QString fn = QFileDialog::getOpenFileName(this,"Select .kla or .koa file",QDir::homePath(),"Koala Images (*.kla *.koa);;All files (*.*)");
    if (fn.length()){
        showfile(fn);
    }
}
