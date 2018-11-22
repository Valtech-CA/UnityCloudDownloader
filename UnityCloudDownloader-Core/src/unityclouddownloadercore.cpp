#include "unityclouddownloadercore.h"

#include "profile.h"
#include "project.h"
#include "buildtarget.h"
#include "build.h"
#include "unityapiclient.h"
#include "servicelocator.h"
#include "database.h"
#include "synchronizer.h"

#include <QObject>
#include <QtConcurrent>

namespace ucd
{

void Core::init()
{
    qRegisterMetaType<Profile>("ucd_Profile");
    qRegisterMetaTypeStreamOperators<Profile>("ucd_Profile");
    qRegisterMetaType<Project>("ucd_Project");
    qRegisterMetaTypeStreamOperators<Project>("ucd_Project");
    qRegisterMetaType<BuildTarget>("ucd_BuildTarget");
    qRegisterMetaTypeStreamOperators<BuildTarget>("ucd_BuildTarget");
    qRegisterMetaType<Build>("ucd_Build");
    qRegisterMetaTypeStreamOperators<Build>("ucd_Build");
    QtConcurrent::run(&UnityApiClient::preconnect);
}

void Core::init(const QString &storagePath, QObject *parent)
{
    init();

    auto *database = new Database(storagePath, parent);
    database->init();
    ServiceLocator::setDatabaseProvier(database);

    auto *synchronizer = new Synchronizer(parent);
    ServiceLocator::setSynchronizer(synchronizer);
}

}
