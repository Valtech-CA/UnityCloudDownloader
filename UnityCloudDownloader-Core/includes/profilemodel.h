#ifndef UCD_PROFILEMODEL_H
#define UCD_PROFILEMODEL_H

#pragma once

#include <QAbstractListModel>

namespace ucd
{

class ProfilesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ProfilesModel(QObject *parent = nullptr);
    ~ProfilesModel();

};
}

#endif // UCD_PROFILEMODEL_H
