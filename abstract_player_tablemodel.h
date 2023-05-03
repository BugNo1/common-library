#ifndef ABSTRACT_PLAYER_TABLEMODEL_H
#define ABSTRACT_PLAYER_TABLEMODEL_H

#include <QAbstractTableModel>
#include "player.h"

class AbstractPlayerTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    AbstractPlayerTableModel(QObject *parent=0) : QAbstractTableModel(parent) {}

    void setData(QList<Player *> *data) {
        m_playerData = data;
    }

public:
    QList<Player *> *m_playerData;
};

#endif // ABSTRACT_PLAYER_TABLEMODEL_H
