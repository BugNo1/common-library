#include <QDebug>
#include "gamedata.h"

GameData::GameData(QObject *parent)
    : QObject(parent)
{
}

GameData::GameData(AbstractPlayerTableModel *playerTableModel, GameType gameType, HighscoreType highscoreType, QObject *parent)
    : QObject(parent), m_gameType(gameType), m_highscoreType(highscoreType), m_playerTableModel(playerTableModel)
{
    setup();
}

void GameData::setup()
{
    m_playerTableModel->setData(&m_highscores);
    m_player1 = new Player(1, this);
    m_player2 = new Player(2, this);
    m_playerNamesFilePath = QDir::currentPath() + "/player-names.txt";
    m_highscoresFilePath = QDir::currentPath() + "/highscores.txt";

    initialize();
    loadPlayerNames();
    loadHighscores();
}

void GameData::initialize()
{
    m_player1->initialize();
    m_player2->initialize();
    m_newHighscore = false;
    resetInHighscoreList();
}

void GameData::loadPlayerNames()
{
    QString lines[2] = {"Spieler1", "Spieler2"};
    int linesArrayLength = sizeof(lines) / sizeof(QString);

    QFile file(m_playerNamesFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        int lineIndex = 0;
        QTextStream stream(&file);
        for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine()) {
            lines[lineIndex] = line;
            lineIndex++;
            if (lineIndex >= linesArrayLength) {
                break;
            }
        };
       file.close();
    }
    m_player1->setName(lines[0]);
    m_player2->setName(lines[1]);
}

void GameData::savePlayerNames()
{
    QFile file(m_playerNamesFilePath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        QTextStream stream(&file);
        stream << m_player1->name();
        stream << Qt::endl;
        stream << m_player2->name();
        file.close();
    }
}

void GameData::loadHighscores()
{
    int maxNumberHighscores = 10;
    for (int index = 0; index < maxNumberHighscores; index++) {
        Player *player = new Player();
        player->setName("Spieler" + QString::number(index + 1));
        m_highscores.append(player);
    }

    QFile file(m_highscoresFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        int lineIndex = 0;
        QTextStream stream(&file);
        for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine()) {
            if (m_highscoreType == HighscoreType::Points) {
                loadPointsHighscores(line, lineIndex);
            } else if (m_highscoreType == HighscoreType::TimeLevel) {
                loadTimeLevelHighscores(line, lineIndex);
            }
            lineIndex++;
            if (lineIndex >= maxNumberHighscores) {
                break;
            }
        }
        file.close();
    }
}

void GameData::loadPointsHighscores(QString line, int lineIndex)
{
    QStringList elements = line.split("|");
    if (elements.length() == 2) {
        m_highscores[lineIndex]->setName(elements[0]);
        bool valid;
        int pointsAchieved = elements[1].toInt(&valid);
        if (valid) {
            m_highscores[lineIndex]->setPointsAchieved(pointsAchieved);
        }
    }
}

void GameData::loadTimeLevelHighscores(QString line, int lineIndex)
{
    QStringList elements = line.split("|");
    if (elements.length() == 3) {
        m_highscores[lineIndex]->setName(elements[0]);
        bool valid;
        double timeAchieved = elements[1].toDouble(&valid);
        if (valid) {
            m_highscores[lineIndex]->setTimeAchieved(timeAchieved);
        }
        int levelAchieved = elements[2].toInt(&valid);
        if (valid) {
            m_highscores[lineIndex]->setLevelAchieved(levelAchieved);
        }
    }
}

void GameData::saveHighscores()
{
    QFile file(m_highscoresFilePath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        QTextStream stream(&file);
        for (int index = 0; index < m_highscores.length(); index++) {
            if (m_highscoreType == HighscoreType::Points) {
                savePointsHighscores(stream, index);
            } else if (m_highscoreType == HighscoreType::TimeLevel) {
                saveTimeLevelHighscores(stream, index);
            }
            if (index < m_highscores.length() - 1) {
                stream << Qt::endl;
            }
        }
        file.close();
    }
}

void GameData::savePointsHighscores(QTextStream &stream, int index)
{
    stream << m_highscores[index]->name();
    stream << "|";
    stream << m_highscores[index]->pointsAchieved();
}

void GameData::saveTimeLevelHighscores(QTextStream &stream, int index)
{
    stream << m_highscores[index]->name();
    stream << "|";
    stream << m_highscores[index]->timeAchieved();
    stream << "|";
    stream << m_highscores[index]->levelAchieved();
}

void GameData::updateHighscores()
{
    if (m_gameType == GameType::Coop) {
        updateHighscoresWithCoopPlayer(getCoopPlayer());
    } else if (m_gameType == GameType::PvP) {
        updateHighscoresWithPlayer(m_player1);
        updateHighscoresWithPlayer(m_player2);
    }
}

void GameData::updateHighscoresWithPlayer(Player *player)
{
    if (m_highscoreType == HighscoreType::TimeLevel) {
        int index = 0;
        for (index = 0; index < m_highscores.length(); index++) {
            if (player->timeAchieved() > m_highscores[index]->timeAchieved()) {
                break;
            }
        }

        if (index < m_highscores.length()) {
            Player *newPlayer = new Player();
            newPlayer->setName(player->name());
            newPlayer->setTimeAchieved(player->timeAchieved());
            newPlayer->setLevelAchieved(player->levelAchieved());
            newPlayer->setInHighscoreList(true);

            m_highscores.insert(index, newPlayer);
            m_highscores.removeAt(m_highscores.length() - 1);
        }

        if (index == 0) {
            m_newHighscore = true;
        }
    } else if (m_highscoreType == HighscoreType::Points) {
        qWarning() << "HighscoreType 'Points' is not implemented for GameType 'PvP'";
    }
}

void GameData::updateHighscoresWithCoopPlayer(Player *coopPlayer)
{
    if (m_highscoreType == HighscoreType::Points) {
        int index = 0;
        for (index = 0; index < m_highscores.length(); index++) {
            if (coopPlayer->pointsAchieved() > m_highscores[index]->pointsAchieved()) {
                break;
            }
        }

        if (index < m_highscores.length()) {
            Player *newPlayer = new Player();
            newPlayer->setName(coopPlayer->name());
            newPlayer->setPointsAchieved(coopPlayer->pointsAchieved());
            newPlayer->setInHighscoreList(true);

            m_highscores.insert(index, newPlayer);
            m_highscores.removeAt(m_highscores.length() - 1);
        }

        if (index == 0) {
            m_newHighscore = true;
        }
    } else if (m_highscoreType == HighscoreType::TimeLevel) {
        qWarning() << "HighscoreType 'TimeLevel' is not implemented for GameType 'Coop'";
    }
}

void GameData::resetInHighscoreList()
{
    for (int index = 0; index < m_highscores.length(); index++) {
        m_highscores[index]->setInHighscoreList(false);
    }
}

Player* GameData::player1()
{
    return m_player1;
}

Player* GameData::player2()
{
    return m_player2;
}

Player* GameData::winner()
{
    if (m_gameType == GameType::Coop) {
        if (m_highscoreType == HighscoreType::Points) {
            return getCoopPlayer();
        } else if (m_highscoreType == HighscoreType::TimeLevel) {
            qWarning() << "HighscoreType 'TimeLevel' is not implemented for GameType 'Coop'";
        }
    } else if (m_gameType == GameType::PvP) {
        if (m_highscoreType == HighscoreType::TimeLevel) {
            if (m_player1->timeAchieved() >= m_player2->timeAchieved()) {
                return m_player1;
            }
            return m_player2;
        } else if (m_highscoreType == HighscoreType::Points) {
            qWarning() << "HighscoreType 'Points' is not implemented for GameType 'PvP'";
        }
    }
    return new Player();
}

Player* GameData::getCoopPlayer() {
    Player *coopPlayer = new Player();
    coopPlayer->setName(m_player1->name() + " & " + m_player2->name());
    coopPlayer->setTimeAchieved(m_player1->timeAchieved());
    coopPlayer->setLevelAchieved(m_player1->levelAchieved());
    coopPlayer->setPointsAchieved(m_player1->pointsAchieved() + m_player2->pointsAchieved());
    return coopPlayer;
}

bool GameData::newHighscore()
{
    return m_newHighscore;
}
