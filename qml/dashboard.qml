/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4

Window {
    id: root
    visible: true
    width: 1024
    height: 600

    color: "#161616"
    title: "Qt Quick Extras Demo"

    ValueSource {
        id: valueSource
    }

    // Dashboards are typically in a landscape orientation, so we need to ensure
    // our height is never greater than our width.
    Item {
        id: container
        width: root.width
        height: Math.min(root.width, root.height)
        anchors.centerIn: parent

        Row {
            id: gaugeRow
            width: 968
            height: 600
            spacing: container.width * 0.02
            anchors.centerIn: parent

            Item {
                width: height
                height: container.height * 0.25 - gaugeRow.spacing
                anchors.verticalCenter: parent.verticalCenter

                CircularGauge {
                    id: fuelGauge
                    value: valueSource.fuel
                    maximumValue: 1
                    y: parent.height / 2 - height / 2 - container.height * 0.01
                    width: parent.width
                    height: parent.height * 0.7

                    style: IconGaugeStyle {
                        id: fuelGaugeStyle

                        icon: "qrc:/images/fuel-icon.png"
                        minWarningColor: Qt.rgba(0.5, 0, 0, 1)

                        tickmarkLabel: Text {
                            color: "white"
                            visible: styleData.value === 0 || styleData.value === 1
                            font.pixelSize: fuelGaugeStyle.toPixels(0.225)
                            text: styleData.value === 0 ? "E" : (styleData.value === 1 ? "F" : "")
                        }
                    }
                }

                CircularGauge {
                    value: udp_data.disp_Temperature
                    maximumValue: 80
                    width: parent.width
                    height: parent.height * 0.7
                    y: parent.height / 2 + container.height * 0.01

                    style: IconGaugeStyle {
                        id: tempGaugeStyle

                        icon: "qrc:/images/temperature-icon2.png"
                        maxWarningColor: Qt.rgba(0.5, 0, 0, 1)

                        tickmarkStepSize: 40
                        labelStepSize: 40

                        minorTickmarkCount: 3


                        tickmarkLabel: Text {
                            color: "white"
                            visible: styleData.value === 0 || styleData.value === 1
                            font.pixelSize: tempGaugeStyle.toPixels(0.225)
                            text: styleData.value === 0 ? "C" : (styleData.value === 1 ? "H" : "")
                        }
                    }
                }
            }

            CircularGauge {
                id: speedometer
                x: 500
                width: 500
                height: 500
                value: udp_data.disp_Throttle //valueSource.kph
                anchors.verticalCenter: parent.verticalCenter
                maximumValue: 90
                // We set the width to the height, because the height will always be
                // the more limited factor. Also, all circular controls letterbox
                // their contents to ensure that they remain circular. However, we
                // don't want to extra space on the left and right of our gauges,
                // because they're laid out horizontally, and that would create
                // large horizontal gaps between gauges on wide screens.

                style: DashboardGaugeStyle {}
            }

            CircularGauge {
                id: tachometer
                width: 200
                height: 200
                value: udp_data.disp_RPM//valueSource.rpm
                maximumValue: 8
                anchors.verticalCenter: parent.verticalCenter

                style: TachometerStyle {}
            }

        }

        TurnIndicator {
            id: leftIndicator
            x: 51
            y: 100
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: container.height * 0.1 - gaugeRow.spacing
            anchors.verticalCenterOffset: -171

            direction: Qt.LeftArrow
            on: valueSource.turnSignal == Qt.LeftArrow
        }

        TurnIndicator {
            id: rightIndicator
            x: 764
            y: 280
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: container.height * 0.1 - gaugeRow.spacing
            anchors.verticalCenterOffset: -184

            direction: Qt.RightArrow
            on: valueSource.turnSignal == Qt.RightArrow
        }

        Text {
            id: text_Volt
            x: 713
            y: 430
            width: 142
            height: 42
            color: "#eb0b0b"
            text: udp_data.disp_pack_voltage //qsTr("Voltage_Here")
            styleColor: "#eb0b0b"
            font.pixelSize: 30
        }
        Text {
            id: text_Error_Msg
            x: 178
            y: 550
            width: 501
            height: 42
            color: "#eb0b0b"
            text: udp_data.disperr //qsTr("Voltage_Here")
            styleColor: "#eb0b0b"
            font.pixelSize: 30
        }
        Text {
            id: text_RunTime
            x: 28
            y: 8
            width: 142
            height: 42
            color: "#eb0b0b"
            text: udp_data.disp_RunTime //qsTr("Voltage_Here")
            styleColor: "#eb0b0b"
            font.pixelSize: 30
        }

    }
}
