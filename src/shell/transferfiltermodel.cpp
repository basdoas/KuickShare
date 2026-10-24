#include "transferfiltermodel.h"

#include "transfermodel.h"

#include <algorithm>

TransferFilterModel::TransferFilterModel(QAbstractItemModel *source, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(source);
    connect(source, &QAbstractItemModel::rowsInserted, this, &TransferFilterModel::sourceCountChanged);
    connect(source, &QAbstractItemModel::rowsRemoved, this, &TransferFilterModel::sourceCountChanged);
    connect(source, &QAbstractItemModel::modelReset, this, &TransferFilterModel::sourceCountChanged);
}

void TransferFilterModel::setDirection(const QString &direction)
{
    change(m_direction, direction, &TransferFilterModel::directionChanged);
}

void TransferFilterModel::setStatus(const QString &status)
{
    change(m_status, status, &TransferFilterModel::statusChanged);
}

void TransferFilterModel::setText(const QString &text)
{
    change(m_text, text, &TransferFilterModel::textChanged);
}

void TransferFilterModel::clearFilters()
{
    setDirection(QStringLiteral("all"));
    setStatus(QStringLiteral("all"));
    setText({});
}

void TransferFilterModel::change(QString &field, const QString &value, void (TransferFilterModel::*changed)())
{
    if (value == field) {
        return;
    }
    const bool wasFiltered = filtered();
    beginFilterChange();
    field = value;
    endFilterChange();
    emit(this->*changed)();
    if (filtered() != wasFiltered) {
        emit filteredChanged();
    }
}

bool TransferFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const QModelIndex row = sourceModel()->index(sourceRow, 0, sourceParent);

    if (m_direction != QLatin1String("all") && row.data(TransferModel::KindRole).toString() != m_direction) {
        return false;
    }

    if (m_status != QLatin1String("all")) {
        const QString state = row.data(TransferModel::StateRole).toString();
        const bool final = TransferModel::isFinal(state);
        const bool finished = state == QLatin1String("Finished");
        if ((m_status == QLatin1String("active") && final) || (m_status == QLatin1String("done") && !finished)
            || (m_status == QLatin1String("failed") && (!final || finished))) {
            return false;
        }
    }

    const QString needle = m_text.trimmed();
    if (needle.isEmpty()) {
        return true;
    }
    if (row.data(TransferModel::DeviceRole).toString().contains(needle, Qt::CaseInsensitive)
        || row.data(TransferModel::TextRole).toString().contains(needle, Qt::CaseInsensitive)) {
        return true;
    }
    const QStringList files = row.data(TransferModel::FilesRole).toStringList();
    return std::any_of(files.cbegin(), files.cend(), [&needle](const QString &file) { return file.contains(needle, Qt::CaseInsensitive); });
}
