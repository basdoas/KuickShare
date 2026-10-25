import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.ki18n

Kirigami.PromptDialog {
    id: dialog

    required property var core

    title: KI18n.i18n("Add KuickShare to the menus?")
    iconName: "qrc:/data/icons/hicolor/scalable/apps/io.github.the_beper.KuickShare.svg"
    subtitle: KI18n.i18n("It will appear in the application menu and in Dolphin's context menu")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    closePolicy: QQC2.Popup.NoAutoClose
    onAccepted: core.integrateAppImage()
    onRejected: core.declineIntegration()
}
