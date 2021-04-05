import QtQuick 2.15

import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.1
import Qt.labs.settings 1.0

import app 1.0

Window {
    title: "qTsConverter " + version

    minimumHeight: 220
    minimumWidth: 800

    height: minimumHeight
    width: minimumWidth

    visible: ApplicationWindow.Windowed

    readonly property bool isCsvFormat: comboType.currentIndex === ConverterGuiProxy.Ts2Csv
                                        || comboType.currentIndex === ConverterGuiProxy.Csv2Ts

    readonly property bool conversionToTs: comboType.currentIndex === ConverterGuiProxy.Csv2Ts
                                           || comboType.currentIndex === ConverterGuiProxy.Xlsx2Ts

    Settings {
        id: settings
        property string lastSourceInput
        property string lastSourceOutput
    }

    GridLayout {
        anchors {
            fill: parent
            margins: 20
        }

        columns: 1
        rows: 3

        RowLayout {

            Text {
                text: "Source filename:"
            }

            Text {
                id: sourceInput

                color: Material.color(Material.Grey)
                text: conversionModel.sourceMsg
                Layout.fillWidth: true
            }

            Button {
                text: "Browse"
                highlighted: true
                onClicked: {
                    loadFileDialog.folder = settings.lastSourceInput;
                    loadFileDialog.open();
                }
            }
        }

        RowLayout {

            Text {
                text: "Destination filename:"
            }

            Text {
                id: sourceOutput

                color: Material.color(Material.Grey)
                Layout.fillWidth: true
            }

            Button {
                text: "Browse"
                highlighted: true
                onClicked: saveFileDialog.visible = true
            }
        }

        RowLayout {
            spacing: 20

            ComboBox {
                id: comboType
                model: conversionModel
                onActivated: conversionModel.setIndex(comboType.currentIndex)
            }

            Row {
                visible: isCsvFormat

                Text {
                    text: "Field separator:"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    border.width: 0.5
                    border.color: "black"
                    width: 30
                    height: 30

                    TextInput {
                        id: fieldSeparator
                        text: ";"
                        anchors.centerIn: parent
                    }
                }
            }

            Row {
                visible: isCsvFormat

                Text {
                    text: "String separator:"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    border.width: 0.5
                    border.color: "black"
                    color: "transparent"
                    width: 30
                    height: 30

                    TextInput {
                        id: stringSeparator
                        text: "\""
                        anchors.centerIn: parent
                    }
                }
            }

            Row {
                visible: conversionToTs

                Text {
                    text: "TS version:"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    border.width: 0.5
                    border.color: "black"
                    color: "transparent"
                    width: 30
                    height: 30

                    TextInput {
                        id: tsVersion
                        text: "2.1"
                        anchors.centerIn: parent
                    }
                }
            }
        }

        Button {
            Layout.fillWidth: true
            text: "Convert"
            highlighted: true
            Material.background: Material.Orange
            enabled: comboType.currentIndex !== ConverterGuiProxy.Dummy
                     && sourceInput.text.length !== 0
                     && sourceOutput.text.length !== 0
                     && fieldSeparator.text.length !== 0
                     && stringSeparator.text.length !== 0
            onClicked: {
                converter.convert(comboType.currentIndex, conversionModel.input(),
                                  sourceOutput.text, fieldSeparator.text,
                                  stringSeparator.text, tsVersion.text)
                finishDialog.visible = true
            }
        }
    }

    LoadFileDialog {
        id: loadFileDialog

        objectName: "loadFileDialog"
        onAccepted: {
            conversionModel.clearInput()

            if (loadFileDialog.files.length > 1) {
                saveFileDialog.selectFolder = true
            }

            if (loadFileDialog.files.length === 1) {
                saveFileDialog.selectFolder = false
            }

            for (var i = 0; i < loadFileDialog.files.length; i++) {
                conversionModel.addInput(loadFileDialog.files[i])
            }
        }
    }

    SaveFileDialog {
        id: saveFileDialog

        objectName: "saveFileDialog"
        selectFolder: false
        visible: false
        onOk: {
            conversionModel.setOutput(path)
            sourceOutput.text = path
            visible = false
        }
    }

    FinishDialog {
        id: finishDialog

        objectName: "finishDialog"
        onAccepted: visible = false
    }

    Connections {
        function onSetComboBoxIndex(index) {
            comboType.currentIndex = index;
            conversionModel.setIndex(comboType.currentIndex);
        }
        target: conversionModel
    }

    Shortcut {
        sequence: "Ctrl+Q"
        onActivated: Qt.quit()
    }

    Component.onCompleted: comboType.currentIndex = ConverterGuiProxy.Dummy
}
