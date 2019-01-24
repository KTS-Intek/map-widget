#include "mapwidget.h"
#include "ui_mapwidget.h"
#include <QWindow>
#include <QQmlContext>
#include <QQuickView>
#include <QQmlError>


///[!] qml-geo
#include "qml-geo/qml2cpp.h"


//---------------------------------------------------------------------------------------------------------
MapWidget::MapWidget(bool isReadOnly, const QString mapTitle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapWidget)
{
    ui->setupUi(this);
    isQmlReady = false;

    this->isReadOnly = isReadOnly;

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
    this->lastLang = lastLang;
    emit setThisFontSize(calcFontPixelSize());
    if(!isQmlReady){

        isQmlReady = true;
        qmlRegisterSingletonType(QUrl("qrc:/geo/qml-geo/context/PageIndex.qml"), "Kts.ItmPageIndexSingleton", 1 , 0 , "PageIndexSingleton");

        QVariantMap parameters;
        parameters.insert("lserver", "lserver");
        parameters.insert("font-size",  QString::number(calcFontPixelSize()));
        parameters.insert("font-family",  this->font().family());
        parameters.insert("company-name", "kts-intek");
        parameters.insert("app-name", "matilda");
        parameters.insert("group-name", "mapping");
        parameters.insert("lang", lastLang);
        QString p = (tmpDir.isValid()) ? tmpDir.path() : QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/qml-geo-" + QTime::currentTime().msecsSinceStartOfDay();
        parameters.insert("work-dir",  p);

            parameters.insert("read-only", isReadOnly);


        QQuickView *view = new QQuickView;
        QWidget *widget = QWidget::createWindowContainer(view, this);
//        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//        widget->setMinimumSize(200);
//        widget->setMaximumSize(200);
        widget->setFocusPolicy(Qt::TabFocus);
        ui->verticalLayout->addWidget(widget);
        widget->updateGeometry();


        Qml2cpp *qml2cpp = new Qml2cpp(parameters, true, this);

        connect(qml2cpp, SIGNAL(addDevice(QString))             , this, SIGNAL(addDevice(QString))              );
        connect(qml2cpp, SIGNAL(showThisDev(QString))           , this, SIGNAL(showThisDev(QString))            );
//        connect(&qml2cpp, SIGNAL(updateModel4ls())            , this, SIGNAL(updateModel4ls())                );
        connect(qml2cpp, SIGNAL(removeDevice(QString))          , this, SIGNAL(removeDevice(QString))           );
        connect(qml2cpp, SIGNAL(moveDevice(QVariantHash))       , this, SIGNAL(moveDevice(QVariantHash))        );
        connect(qml2cpp, SIGNAL(addDeviceStreet(QVariantHash))  , this, SIGNAL(addDeviceStreet(QVariantHash))   );
        connect(qml2cpp, SIGNAL(niActivated(QString))           , this, SIGNAL(niActivated(QString))            );

        connect(this, SIGNAL(setNewDeviceModel(QVariantList)), qml2cpp, SLOT(setNewDeviceModel(QVariantList))  );
        connect(this, SIGNAL(showThisDeviceNI(QString))      , qml2cpp, SLOT(showThisDeviceNI(QString))        );
        connect(this, SIGNAL(showThisDeviceNIQuite(QString)) , qml2cpp, SLOT(showThisDeviceNIQuite(QString))   );

        connect(this, SIGNAL(showThisCoordinates(QString))   , qml2cpp, SLOT(showThisCoordinates(QString))     );
        connect(this, SIGNAL(setThisFontSize(int))           , qml2cpp, SLOT(setThisFontSize(int))             );

        connect(qml2cpp, SIGNAL(addDeviceStreet(QVariantHash))  , this, SLOT(addDeviceStreetSlot(QVariantHash)) );

        view->rootContext()->setContextProperty("manager", qml2cpp);
        view->rootContext()->setContextProperty("geoPointModel", qml2cpp->geoPointModel);
        view->rootContext()->setContextProperty("modelProviders", qml2cpp->modelProviders);
        view->rootContext()->setContextProperty("modelProviderParam", qml2cpp->modelProviderParam);
        view->rootContext()->setContextProperty("modelMapType", qml2cpp->modelMapType);
        view->rootContext()->setContextProperty("modelProviderProfile", qml2cpp->modelProviderProfile);

        view->rootContext()->setContextProperty("interProcessDeviceModel", qml2cpp->interProcessDeviceModel);

        view->setSource(QUrl("qrc:/geo/qml-geo/main.qml"));

        connect(this, SIGNAL(destroyed(QObject*)), view, SLOT(deleteLater()) );

//         connect(this, SIGNAL(killMapSignal()), widget, SLOT(deleteLater()) );

//        connect(this, SIGNAL(killMapSignal()), view, SLOT(deleteLater()) );
//        connect(view, SIGNAL(destroyed(QObject*)), qml2cpp, SLOT(deleteLater()) );
////        connect(qml2cpp, SIGNAL(destroyed(QObject*)), widget, SLOT(deleteLater()) );
//        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()) );


        view->show();


        foreach (QQmlError e, view->errors()) {
            qDebug() << "qmlErr " << e.toString();
        }


    }
    emit updateModel4ls();
}

void MapWidget::addDeviceStreetSlot(QVariantHash h)
{
    if(!h.value("simpleText").toString().isEmpty())
    emit addDeviceStreet(h.value("simpleText").toString());

//    h.insert("pos", QString("%1,%2").arg(QString::number(c.latitude(),'f', 6)).arg(QString::number(c.longitude(),'f', 6)));

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
