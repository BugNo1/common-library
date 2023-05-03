#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include "player.h"
#include "abstract_player_tablemodel.h"

class GameData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Player* player1 READ player1 CONSTANT)
    Q_PROPERTY(Player* player2 READ player2 CONSTANT)
    Q_PROPERTY(Player* winner READ winner CONSTANT)
    Q_PROPERTY(bool newHighscore READ newHighscore CONSTANT)

public:
    enum GameType {Coop, PvP};
    enum HighscoreType {Points, TimeLevel};

    GameData(QObject *parent=0);
    GameData(AbstractPlayerTableModel *playerTableModel, GameType gameType, HighscoreType highscoreType, QObject *parent=0);

    Q_INVOKABLE void initialize();

    void loadPlayerNames();
    Q_INVOKABLE void savePlayerNames();

    void loadHighscores();
    Q_INVOKABLE void saveHighscores();
    Q_INVOKABLE void updateHighscores();
    void updateHighscoresWithPlayer(Player *player);
    void updateHighscoresWithCoopPlayer(Player *coopPlayer);
    void resetInHighscoreList();

    Player* player1();
    Player* player2();
    Player* winner();
    bool newHighscore();

private:
    void setup();
    void loadPointsHighscores(QString line, int lineIndex);
    void savePointsHighscores(QTextStream &stream, int index);
    void loadTimeLevelHighscores(QString line, int lineIndex);
    void saveTimeLevelHighscores(QTextStream &stream, int index);
    Player* getCoopPlayer();

    Player *m_player1;
    Player *m_player2;
    bool m_newHighscore;
    QString m_playerNamesFilePath;
    QString m_highscoresFilePath;
    GameType m_gameType;
    HighscoreType m_highscoreType;
    AbstractPlayerTableModel *m_playerTableModel;
    QList<Player *> m_highscores;
};

#endif // GAMEDATA_H
