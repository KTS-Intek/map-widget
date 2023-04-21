#include "mapwidget.h"
#include "ui_mapwidget.h"
#include <QWindow>
#include <QQmlContext>
#include <QQuickView>
#include <QQmlError>
//#include <QQuickWidget>
#include <QQmlApplicationEngine>


#if QT_CONFIG(ssl)
#include <QSslSocket>
#endif


///[!] qml-geo
#include "qml-geo-v2/qmlgeo2cppmedium.h"
#include "qml-geo-v2/mygeoconverthelper.h"

//---------------------------------------------------------------------------------------------------------
MapWidget::MapWidget(const bool &allow2useDevPosCalcualtion, const bool &isReadOnly, const QString &mapTitle, const bool &verboseMode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapWidget)
{
    ui->setupUi(this);


    this->verboseMode = verboseMode;
    mstate.isReadOnly = isReadOnly;
    mstate.allow2useDevPosCalcualtion = allow2useDevPosCalcualtion;

    QmlGeo2cppMedium::registerQmlTypes();

    if(mapTitle.isEmpty())
        ui->widget->hide();
    else
        ui->label->setText(mapTitle);
}
//---------------------------------------------------------------------------------------------------------
MapWidget::~MapWidget()
{

    delete ui;
}
//---------------------------------------------------------------------------------------------------------
void MapWidget::showMap(QString lastLang)
{
    mstate.lastLang = lastLang;
    emit setThisFontSize(calcFontPixelSize());
    if(!mstate.isQmlReady){

        mstate.isQmlReady = true;
//        qmlRegisterSingletonType(QUrl("qrc:/geo/qml-geo/context/PageIndex.qml"), "Kts.ItmPageIndexSingleton", 1 , 0 , "PageIndexSingleton");

        ThisMapObjectSettings parameters;

//        QVariantMap parameters;
//        parameters.insert("lserver", "lserver");
//        parameters.insert("font-size",  QString::number(calcFontPixelSize()));
//        parameters.insert("font-family",  this->font().family());
        parameters.company_name = "kts-intek";
        parameters.app_name = "matilda";
        parameters.group_name = "mapping";
        parameters.isReadOnly = mstate.isReadOnly;


//        parameters.insert("lang", lastLang);
//        QString p = (tmpDir.isValid()) ? tmpDir.path() : QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/qml-geo-" + QTime::currentTime().msecsSinceStartOfDay();
//        parameters.insert("work-dir",  "");

//        parameters.insert("read-only", isReadOnly);


        QQmlApplicationEngine *view = new QQmlApplicationEngine;


#if QT_CONFIG(ssl)
    view->rootContext()->setContextProperty("supportsSsl", QSslSocket::supportsSsl());
#else
    view->rootContext()->setContextProperty("supportsSsl", false);
#endif

        QmlGeo2cppMedium *qmlGeoCppMedium = new QmlGeo2cppMedium(parameters, verboseMode, this);


        qmlGeoCppMedium->pagesett.allow2useDevPosCalcualtion = mstate.allow2useDevPosCalcualtion;

//        connect(qmlGeoCppMedium, &QmlGeo2cppMedium::moveDevice              , this, &MapWidget::moveDevice);
//        connect(qmlGeoCppMedium, &QmlGeo2cppMedium::addDeviceStreet         , this, &MapWidget::addDeviceStreet);

        connect(this, &MapWidget::addDeviceStreet         , this, &MapWidget::addDeviceStreetSlot);


        connect(this, &MapWidget::setTableData              , qmlGeoCppMedium, &QmlGeo2cppMedium::setTableData );
        connect(this, &MapWidget::setTableDataExt           , qmlGeoCppMedium, &QmlGeo2cppMedium::setTableDataExt );
        connect(this, &MapWidget::setCoordinatorPosition    , qmlGeoCppMedium, &QmlGeo2cppMedium::setCoordinatorPosition );

/*
 * ! connect these signals directly in QML
 *  connect(this, &MapWidget::showThisDeviceNI          , qmlGeoCppMedium, &QmlGeo2cppMedium::showThisDeviceNI);
 *  connect(this, &MapWidget::showThisDeviceNIQuite     , qmlGeoCppMedium, &QmlGeo2cppMedium::showThisDeviceNIQuite);
 *  connect(this, &MapWidget::showThisCoordinate       , qmlGeoCppMedium, &QmlGeo2cppMedium::showThisCoordinate);
 *
 *  send these signals directcly from the QML
 *  connect(qmlGeoCppMedium, &QmlGeo2cppMedium::niActivated             , this, &MapWidget::niActivated); *
 *  connect(qmlGeoCppMedium, &QmlGeo2cppMedium::addDevice               , this, &MapWidget::addDevice);
 *  connect(qmlGeoCppMedium, &QmlGeo2cppMedium::showThisDev             , this, &MapWidget::showThisDev);
 *  connect(qmlGeoCppMedium, &QmlGeo2cppMedium::showContextMenu4thisDev , this, &MapWidget::showContextMenu4thisDev);
 *  connect(qmlGeoCppMedium, &QmlGeo2cppMedium::showThisDevInSource     , this, &MapWidget::showThisDevInSource);
 *  connect(qmlGeoCppMedium, &QmlGeo2cppMedium::removeDevice            , this, &MapWidget::removeDevice);
 *
 */


//        connect(this, SIGNAL(setThisFontSize(int))           , qml2cpp, SLOT(setThisFontSize(int))             );


        view->rootContext()->setContextProperty("qmlGeoCppMedium", qmlGeoCppMedium);
        view->rootContext()->setContextProperty("cppParent", this);

//        view->rootContext()->setContextProperty("geoPointModel", qml2cpp->geoPointModel);
//        view->rootContext()->setContextProperty("modelProviders", qml2cpp->modelProviders);
//        view->rootContext()->setContextProperty("modelProviderParam", qml2cpp->modelProviderParam);
//        view->rootContext()->setContextProperty("modelMapType", qml2cpp->modelMapType);
//        view->rootContext()->setContextProperty("modelProviderProfile", qml2cpp->modelProviderProfile);

//        view->rootContext()->setContextProperty("interProcessDeviceModel", qml2cpp->interProcessDeviceModel);


        view->load(QUrl("qrc:///geo/qml-geo-v2/qml/main4map.qml"));

        connect(this, SIGNAL(destroyed(QObject*)), view, SLOT(deleteLater()) );


        QWindow *qmlWindow = qobject_cast<QWindow*>(view->rootObjects().at(0));
        QWidget *widget = QWidget::createWindowContainer(qmlWindow, this);
        widget->setMinimumSize(this->minimumSize());
        widget->setMaximumSize(this->maximumSize());

        widget->setFocusPolicy(Qt::StrongFocus);
        ui->verticalLayout->addWidget(widget);
        widget->updateGeometry();



    }
}
//---------------------------------------------------------------------------------------------------------
void MapWidget::addDeviceStreetSlot(QVariantHash h)
{
    if(!h.value("simpleText").toString().isEmpty())
        emit addDeviceStreetLine(h.value("simpleText").toString());

    if(verboseMode)
        qDebug() << "MapWidget::addDeviceStreetSlot(QVariantHash h) " << h;
    //    h.insert("pos", QString("%1,%2").arg(QString::number(c.latitude(),'f', 6)).arg(QString::number(c.longitude(),'f', 6)));

}
//---------------------------------------------------------------------------------------------------------
void MapWidget::addDeviceStreetStr(QString simpletxt, QString pos)
{
    QVariantHash h;
    h.insert("simpleText", simpletxt);
    h.insert("pos", pos);//it is already checked
    emit addDeviceStreet(h);
}
//---------------------------------------------------------------------------------------------------------
void MapWidget::moveDeviceStr(QString keyval, QString pos)
{
    QVariantHash h;
    h.insert("ni", keyval);
    h.insert("pos", pos);//it is already checked
    emit moveDevice(h);
}
//---------------------------------------------------------------------------------------------------------
void MapWidget::setNewDeviceModel(QVariantList vl)
{
    //obsolete
    if(verboseMode)
        qDebug() << "MapWidget::setNewDeviceMode " << vl;
}

//---------------------------------------------------------------------------------------------------------

void MapWidget::gimmeDefaultDataImageFilteringSettings()
{
    emit setPredefinedDataImageFilteringSettings(mstate.predefinedgroupmethod);
    emit setDefaultDataImageFilteringSettings(mstate.mapdefaultgroupmethod, mstate.dataFilterSettingsName);
}

//---------------------------------------------------------------------------------------------------------

void MapWidget::setPredefinedDataFilterSettings(QVariantMap predefinedfiltersettings)
{

    mstate.predefinedgroupmethod = predefinedfiltersettings;
    emit setPredefinedDataImageFilteringSettings(mstate.predefinedgroupmethod);

}

void MapWidget::realoadFromTheDataSourceForced()
{
    emit mapIsReady();

}

void MapWidget::showThisCoordinate(QString c)
{
    const QVariant pos = MyGeoConvertHelper::coordianeFromStrVar(c);
    if(!pos.isValid())
        return;
    emit  centerMapHere(pos);
}



//---------------------------------------------------------------------------------------------------------

void MapWidget::setDefaultDataFilterSettings(QVariantMap maponeprofile, QString profilename)
{
    mstate.dataFilterSettingsName = profilename;
    mstate.mapdefaultgroupmethod = maponeprofile;

    emit setDefaultDataImageFilteringSettings(mstate.mapdefaultgroupmethod, mstate.dataFilterSettingsName);


}


//---------------------------------------------------------------------------------------------------------
int MapWidget::calcFontPixelSize()
{
    QFont f = this->font();
    int pixeS = f.pixelSize();
    if(pixeS < 0){
        QFontInfo fi(f);
        //        pixeS = fi.pointSize();
        pixeS = fi.pixelSize();
        if(pixeS < 0){
            int pointS = f.pointSize();
            if(pointS < 0){
                pointS = f.pointSizeF();
            }
            if(pointS < 0)
                pointS = 12;

            pixeS = pointS * 1.5;
        }

    }
    return pixeS;
}
//---------------------------------------------------------------------------------------------------------

void MapWidget::on_pushButton_clicked()
{
    deleteLater();
}
