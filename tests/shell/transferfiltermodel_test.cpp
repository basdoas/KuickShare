#include <QtTest>

#include "transferfiltermodel.h"
#include "transfermodel.h"

class TransferFilterModelTest : public QObject
{
    Q_OBJECT

private:
    static void add(TransferModel &model, const QString &id, const QString &kind, const QString &state, const QString &device, const QString &files = {}, const QString &text = {})
    {
        model.update(id,
                     {{QStringLiteral("kind"), kind},
                      {QStringLiteral("state"), state},
                      {QStringLiteral("device"), device},
                      {QStringLiteral("files"), files},
                      {QStringLiteral("text"), text}});
    }
    static QStringList ids(const TransferFilterModel &filter)
    {
        QStringList out;
        for (int row = 0; row < filter.rowCount(); ++row) {
            out << filter.data(filter.index(row, 0), TransferModel::IdRole).toString();
        }
        out.sort();
        return out;
    }

private Q_SLOTS:
    void showsEverythingByDefault()
    {
        TransferModel model;
        add(model, QStringLiteral("a"), QStringLiteral("inbound"), QStringLiteral("Finished"), QStringLiteral("Pixel"));
        add(model, QStringLiteral("b"), QStringLiteral("outbound"), QStringLiteral("SendingFiles"), QStringLiteral("Galaxy"));
        TransferFilterModel filter(&model);
        QCOMPARE(filter.rowCount(), 2);
        QCOMPARE(filter.sourceCount(), 2);
        QVERIFY(!filter.filtered());
    }

    void filtersByDirection()
    {
        TransferModel model;
        add(model, QStringLiteral("a"), QStringLiteral("inbound"), QStringLiteral("Finished"), QStringLiteral("Pixel"));
        add(model, QStringLiteral("b"), QStringLiteral("outbound"), QStringLiteral("Finished"), QStringLiteral("Galaxy"));
        TransferFilterModel filter(&model);
        filter.setDirection(QStringLiteral("outbound"));
        QCOMPARE(ids(filter), QStringList{QStringLiteral("b")});
        QVERIFY(filter.filtered());
        QCOMPARE(filter.sourceCount(), 2);
    }

    void groupsTheStates()
    {
        TransferModel model;
        add(model, QStringLiteral("run"), QStringLiteral("outbound"), QStringLiteral("SendingFiles"), QStringLiteral("A"));
        add(model, QStringLiteral("ask"), QStringLiteral("inbound"), QStringLiteral("WaitingForUserConsent"), QStringLiteral("B"));
        add(model, QStringLiteral("ok"), QStringLiteral("inbound"), QStringLiteral("Finished"), QStringLiteral("C"));
        add(model, QStringLiteral("no"), QStringLiteral("inbound"), QStringLiteral("Rejected"), QStringLiteral("D"));
        add(model, QStringLiteral("cut"), QStringLiteral("outbound"), QStringLiteral("Disconnected"), QStringLiteral("E"));
        add(model, QStringLiteral("off"), QStringLiteral("outbound"), QStringLiteral("Cancelled"), QStringLiteral("F"));
        TransferFilterModel filter(&model);

        filter.setStatus(QStringLiteral("active"));
        QCOMPARE(ids(filter), (QStringList{QStringLiteral("ask"), QStringLiteral("run")}));
        filter.setStatus(QStringLiteral("done"));
        QCOMPARE(ids(filter), QStringList{QStringLiteral("ok")});
        filter.setStatus(QStringLiteral("failed"));
        QCOMPARE(ids(filter), (QStringList{QStringLiteral("cut"), QStringLiteral("no"), QStringLiteral("off")}));
    }

    void searchesDeviceTextAndFileNamesIgnoringCase()
    {
        TransferModel model;
        add(model, QStringLiteral("a"), QStringLiteral("outbound"), QStringLiteral("Finished"), QStringLiteral("Pixel 7"), QStringLiteral("/tmp/Holiday.JPG\n/tmp/notes.txt"));
        add(model, QStringLiteral("b"), QStringLiteral("inbound"), QStringLiteral("Finished"), QStringLiteral("Galaxy"), {}, QStringLiteral("https://kde.org"));
        TransferFilterModel filter(&model);

        filter.setText(QStringLiteral("pixel"));
        QCOMPARE(ids(filter), QStringList{QStringLiteral("a")});
        filter.setText(QStringLiteral("holiday"));
        QCOMPARE(ids(filter), QStringList{QStringLiteral("a")});
        filter.setText(QStringLiteral(" KDE.org "));
        QCOMPARE(ids(filter), QStringList{QStringLiteral("b")});
        filter.setText(QStringLiteral("nothing like this"));
        QCOMPARE(filter.rowCount(), 0);
    }

    void filtersCombineAndClear()
    {
        TransferModel model;
        add(model, QStringLiteral("a"), QStringLiteral("inbound"), QStringLiteral("Finished"), QStringLiteral("Pixel"));
        add(model, QStringLiteral("b"), QStringLiteral("outbound"), QStringLiteral("Finished"), QStringLiteral("Pixel"));
        add(model, QStringLiteral("c"), QStringLiteral("inbound"), QStringLiteral("Rejected"), QStringLiteral("Pixel"));
        TransferFilterModel filter(&model);
        QSignalSpy filtered(&filter, &TransferFilterModel::filteredChanged);

        filter.setDirection(QStringLiteral("inbound"));
        filter.setStatus(QStringLiteral("done"));
        filter.setText(QStringLiteral("pixel"));
        QCOMPARE(ids(filter), QStringList{QStringLiteral("a")});
        QCOMPARE(filtered.count(), 1); // only the first filter changed the state

        filter.clearFilters();
        QCOMPARE(filter.rowCount(), 3);
        QVERIFY(!filter.filtered());
        QCOMPARE(filtered.count(), 2);
    }

    void followsTheSourceWhileFiltered()
    {
        TransferModel model;
        TransferFilterModel filter(&model);
        QSignalSpy count(&filter, &TransferFilterModel::sourceCountChanged);
        filter.setStatus(QStringLiteral("active"));
        add(model, QStringLiteral("a"), QStringLiteral("outbound"), QStringLiteral("SendingFiles"), QStringLiteral("Pixel"));
        QCOMPARE(filter.rowCount(), 1);
        QVERIFY(count.count() >= 1);
        add(model, QStringLiteral("a"), QStringLiteral("outbound"), QStringLiteral("Finished"), QStringLiteral("Pixel")); // it ends
        QCOMPARE(filter.rowCount(), 0);
        QCOMPARE(filter.sourceCount(), 1);
    }
};

QTEST_GUILESS_MAIN(TransferFilterModelTest)
#include "transferfiltermodel_test.moc"
