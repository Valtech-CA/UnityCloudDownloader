#include "unityclouddownloadercore.h"

#include "profile.h"
#include "project.h"
#include "buildtarget.h"
#include "build.h"
#include "unityapiclient.h"

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
    //qRegisterMetaTypeStreamOperators<Build>("ucd_Build");
    QtConcurrent::run(&UnityApiClient::preconnect);
}

}
