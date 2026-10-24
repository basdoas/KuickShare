#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include "instance.h"
#include "updatewatcher.h"

class InstanceTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void replacesOnlyADifferentVersionOrWhenForced()
    {
        QVERIFY(!Instance::shouldReplace(QString(), QStringLiteral("1.3.0"), true));
        QVERIFY(!Instance::shouldReplace(QStringLiteral("1.3.0"), QStringLiteral("1.3.0"), false));
        QVERIFY(Instance::shouldReplace(QStringLiteral("1.3.0"), QStringLiteral("1.3.0"), true));
        QVERIFY(Instance::shouldReplace(QStringLiteral("1.2.1"), QStringLiteral("1.3.0"), false));
    }

    void fileSignatureChangesWhenTheFileIsReplaced()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath(QStringLiteral("app"));
        const QString other = dir.filePath(QStringLiteral("new"));
        QVERIFY(QFile(path).open(QIODevice::WriteOnly));
        QVERIFY(QFile(other).open(QIODevice::WriteOnly));
        const QString before = UpdateWatcher::fileSignature(path);
        QVERIFY(!before.isEmpty());
        QCOMPARE(UpdateWatcher::fileSignature(path), before);
        QVERIFY(QFile::remove(path));
        QVERIFY(QFile::rename(other, path));
        QVERIFY(UpdateWatcher::fileSignature(path) != before);
        QVERIFY(UpdateWatcher::fileSignature(dir.filePath(QStringLiteral("missing"))).isEmpty());
    }
};

QTEST_GUILESS_MAIN(InstanceTest)
#include "instance_test.moc"
