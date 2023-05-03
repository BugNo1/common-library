#ifndef PLAYER_TABLEMODEL_POINTS_H
#define PLAYER_TABLEMODEL_POINTS_H

#include "abstract_player_tablemodel.h"

class PlayerTableModelPoints : public AbstractPlayerTableModel
{
    Q_OBJECT

public:
    enum PlayerRoles {
        ValueRole = Qt::UserRole + 1,
        SizeRole,
        ColorRole
    };

    PlayerTableModelPoints(QObject *parent=0);

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

protected:
    QHash<int, QByteArray> roleNames() const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

Q_DECLARE_METATYPE(PlayerTableModelPoints *)

#endif // PLAYER_TABLEMODEL_POINTS_H
