#include "gpsitemspreview.h"
#include "ui_gpsitemspreview.h"
#include <QShortcut>
#include <QMessageBox>


///[!] map-widget
#include "map-pgs/mapwidget.h"


///[!] guisett-shared
#include "gui-src/settloader.h"
#include "gui-src/showmesshelpercore.h"
#include "src/widgets/tableheaders.h"


#include "moji_defy.h"
#include "matildalimits.h"

GpsItemsPreview::GpsItemsPreview(LastDevInfo *lDevInfo, GuiHelper *gHelper, GuiSett4all *gSett4all, QWidget *parent) :
    ReferenceWidgetClass(lDevInfo, gHelper, gSett4all, parent),
    ui(new Ui::GpsItemsPreview)
{
    ui->setupUi(this);
}

GpsItemsPreview::~GpsItemsPreview()
{
    delete ui;
}

QStringList GpsItemsPreview::getHeader4meterType()
{
    QString s = TableHeaders::getColNamesItemsCoordinateList() ;

    return s.split(",");
}

void GpsItemsPreview::initPage()
{
    mapIsReady = false;
    setupObjects(ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_ITEMS_GPSPOS);
    connectMessSignal();
    clearPage();

    connect(ui->pbClose, SIGNAL(clicked(bool)), this, SLOT(deleteLater()) );

    QShortcut *s = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(s, SIGNAL(activated()), this, SLOT(deleteLater())) ;
    connectModelSignalWithGuiHelper(CLBRD_ITMSGPS_LIST);

    if(!addLater.meters.isEmpty())
        addMeters2list(addLater.replaceAll, addLater.meters);
}

void GpsItemsPreview::clearPage()
{
    gHelper->hashUndoHistoryTv.remove(ui->tvTable);
//    ui->cbAddMeterModel->clear();
    ui->tbFilter->setEnabled(true);
    ui->leFilter->clear();

    ui->swMeterList->setCurrentIndex(0);
    ui->tbShowList->setChecked(true);
    ui->tbShowMap->setChecked(false);


    StandardItemModelHelper::modelSetHorizontalHeaderItems(model , getHeader4meterType());

}

void GpsItemsPreview::setPageSett(const QVariantHash &h)
{

    StandardItemModelHelper::append2model(h, model);
    setHasDataFromRemoteDevice();
    emit resizeTv2content(ui->tvTable);
}

void GpsItemsPreview::showThisDev(QString ni)
{

}

void GpsItemsPreview::moveDevice(QVariantHash h)
{
    const int row = StandardItemModelHelper::getRowFromNI(2, h.value("ni").toString(), model);

    if(row < 0)
        return;

    model->item(row, 8)->setText(h.value("pos").toString());
    sayModelChanged();
}

void GpsItemsPreview::removeDevice(QString ni)
{
    const int row = StandardItemModelHelper::getRowFromNI(2, ni, model);

    if(row < 0)
        return;
    gHelper->deleteThisRowsFromTv(QList<int>() << row , ui->tvTable, model);
    sayModelChanged();
}

void GpsItemsPreview::sayModelChanged()
{

}

void GpsItemsPreview::sayModelChangedLater()
{

}

void GpsItemsPreview::addMeters2list(bool replaceAll, QVariantHash meters)
{
    if(!getInitStarted()){
        addLater.replaceAll = replaceAll;
        addLater.meters = meters;
        return;
    }
    hasDataFromDevice = true;
    QMenu *menu = new QMenu(ui->tvTable);
    QAction *a = new QAction(menu);
    if(replaceAll || model->rowCount() < 1)
        StandardItemModelHelper::modelSetHorizontalHeaderItems(model, getHeader4meterType());
    gHelper->hashData2table(meters, a);
    menu->deleteLater();
    sayModelChangedLater();

    addLater.meters.clear();
}

void GpsItemsPreview::onActivateMap()
{
    if(!ui->tbShowMap->isChecked())
        ui->tbShowMap->animateClick();
}

void GpsItemsPreview::on_tbShowList_clicked()
{
    ui->tbShowList->setChecked(true);
    ui->swMeterList->setCurrentIndex(0);
    ui->tbShowMap->setChecked(false);
}

void GpsItemsPreview::on_tbShowMap_clicked()
{
    ui->tbShowList->setChecked(false);
    ui->swMeterList->setCurrentIndex(1);
    ui->tbShowMap->setChecked(true);

    if(!mapIsReady){
        mapIsReady = true;

        QTimer *tmrModelChanged = new QTimer(this);
        tmrModelChanged->setInterval(555);
        tmrModelChanged->setSingleShot(true);
        proxy_model->enableModelChangedSignal();
        connect(proxy_model, SIGNAL(onModelChanged()), tmrModelChanged, SLOT(start()) );
        connect(tmrModelChanged, SIGNAL(timeout()), this, SLOT(onModelChanged()) );

        MapWidget *w = new MapWidget;
        ui->vlQmlMap->addWidget(w);

        connect(this, SIGNAL(showMap(QString))  , w, SLOT(showMap(QString)) );

        connect(this, SIGNAL(setNewDeviceModel(QVariantList)), w, SIGNAL(setNewDeviceModel(QVariantList))   );
        connect(this, SIGNAL(showThisDeviceNI(QString))      , w, SIGNAL(showThisDeviceNI(QString))         );
//        connect(editWdgt, SIGNAL(showThisDev(QString))       , w, SIGNAL(showThisCoordinates(QString))      );

        connect(w, SIGNAL(showThisDev(QString))         , this, SLOT(showThisDev(QString))          );
        connect(w, SIGNAL(updateModel4ls())             , this, SLOT(onModelChanged())              );
        connect(w, SIGNAL(removeDevice(QString))        , this, SLOT(removeDevice(QString))         );
        connect(w, SIGNAL(moveDevice(QVariantHash))     , this, SLOT(moveDevice(QVariantHash)) );
//        connect(w, SIGNAL(addDeviceStreet(QVariantHash)), editWdgt, SLOT(addDeviceStreet(QVariantHash)) );
//        connect(w, SIGNAL(moveDevice(QVariantHash))     , editWdgt, SLOT(moveDevice(QVariantHash)) );
//        connect(w, SIGNAL(addDevice(QString))           , editWdgt, SLOT(addDevice(QString))            );

//        connect(ui->tbShowMap, SIGNAL(toggled(bool)), w, SLOT(isParentWidgetVisible(bool)) );
    }

    emit showMap(gHelper->getLastLang());
}

void GpsItemsPreview::onModelChanged()
{
    QVariantList vl;

    //    QStringList l = GuiHelper::getColNamesLedLamp().split(",");
    //tr("Model,NI,Group,Last Exchange,Power [%],Start Power [%],NA Power [%],Tna [sec],Coordinate,Poll On/Off,Street,Memo") ;

    //return tr("Model,Serial Number,NI,Memo,Password,On/Off,Energy,Tariff                  ,Coordinate,Meter Version")
//    return tr("Model,Serial Number,NI,Memo, Profile,On/Off,Input , Disable Time Adjustment,Coordinate,Meter Version");

    const QStringList l4app = QString("ni pos pll img").split(" ");
    const QList<int> l4appIndx = QList<int>() << 2 << 8 << 5 ; //-1


    for(int i = 0, iMax = proxy_model->rowCount(), lMax = l4appIndx.size(); i < iMax; i++){
        int row = proxy_model->mapToSource(proxy_model->index(i, 0)).row();
        QVariantHash h;
        for(int l = 0; l < lMax; l++)//службова інформація
            h.insert(l4app.at(l), model->item(row, l4appIndx.at(l))->text());

        QString tltp = QString("ID: <b>%1</b>, Street: %2").arg(model->item(row, 2)->text()).arg(model->item(row, 1)->text());

        if(!model->item(row, 3)->text().isEmpty())
            tltp.append(tr("<br>%1").arg(model->item(row, 3)->text()));

        if(!model->item(row, 9)->text().isEmpty())
            tltp.append(tr("<br>Version: %1").arg(model->item(row, 9)->text()));
        h.insert("grp", model->item(row, 0)->text() + "\t" + model->item(row, 9)->text());
        tltp.append("<br>");
        h.insert("tltp", tltp);
        h.insert("rowid", row + 1);
        if(h.value("pos").toString().isEmpty())//якщо нема координат, то і нема чого показувать
            continue;
        vl.append(h);
    }


    if(!lDevInfo->matildaDev.coordinatesIsDefault){
        QVariantHash h;
        h.insert("pos", QString("%1,%2").arg(QString::number(lDevInfo->matildaDev.coordinates.x(), 'f', 6)).arg(QString::number(lDevInfo->matildaDev.coordinates.y(), 'f', 6)) );
        h.insert("isMatilda", true);
        h.insert("rowid", "Z");
        h.insert("ni", tr("Universal Communicator"));
        h.insert("tltp", h.value("ni").toString() + tr("<br>S/N: %1<br>").arg(lDevInfo->matildaDev.lastSerialNumber));
        vl.prepend(h);
    }


    emit setNewDeviceModel(vl);

    const int row = proxy_model->mapToSource(ui->tvTable->currentIndex()).row();
    if(row < 0)
        return;
    emit showThisDeviceNI(model->item(row, 2)->text());
}

void GpsItemsPreview::on_tvTable_customContextMenuRequested(const QPoint &pos)
{
    gHelper->createCustomMenu(pos, ui->tvTable, (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy), CLBRD_SMPL_PRXTBL, ShowMessHelperCore::matildaFileName(windowTitle()));

}
