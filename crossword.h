#ifndef CROSSWORD_H
#define CROSSWORD_H

#include <array>

#include <QObject>
#include <QVector>
#include <QHash>

// #include "cwc/grid.hh"

namespace qqwing {
    class SudokuBoard;
};

class Crossword : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows NOTIFY rowsChanged)
    Q_PROPERTY(int columns READ columns NOTIFY columnsChanged)

public:
    explicit Crossword(QObject *parent = nullptr);

    int rows() const { return m_rows; }
    int columns() const { return m_columns; }

signals:
    void columnsChanged();
    void rowsChanged();

public slots:
    QString hintAt(const int index);
    QString numberAt(const int index);
    bool isFixed(const int index);

    QStringList hintsAcross();
    QStringList hintsDown();

    QString hintTextAt(int index);

private:
    void parseWordlist(const QString &filePath);
    void generateCrossword();

    QHash<QString, QString> m_hints;

    int m_rows = 9;
    int m_columns = 9;

    qqwing::SudokuBoard *m_grid = nullptr;
    // int m_originalSolution[81];
    std::array<int,81> m_initialHints;
};

#endif // CROSSWORD_H
