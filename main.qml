import QtQuick 2.6
import QtQuick.Window 2.2
import com.iskrembilen 1.0

TabletWindow {
    visible: true
    width: 1404
    height: 1872

    // title: "reDoku"
    title: "sudokable"
    flags: Qt.Dialog

    // Rectangle {
    //     anchors.fill: parent
    // }
    Text {
        id: currentHint
        anchors {
            bottom: parent.bottom
            bottomMargin: 20
            horizontalCenter: parent.horizontalCenter
        }
        font.pointSize: 20
        width: parent.width / 3
    }

    Rectangle {
        anchors.fill: mainGrid
        anchors.margins: -3
        color: "transparent"
        border.width: 5
    }

    Grid {
        id: mainGrid
        anchors {
            centerIn: parent
            margins: 20
        }

        spacing: 0
        rows: 3
        columns: 3

        // TEST
        // width: parent.width
        // height: parent.height

        // property int cellSize: Math.max(Math.floor(Math.min(width / columns, height / rows)), 240);
        property int cellSize: 240+2*2;
        // property int cellSize: 80*3;

        Repeater {
            id: groups
            model: parent.rows * parent.columns
            delegate: Rectangle {
                // anchors.fill: parent
                // anchors.margins: -(border.width + 2)
                // anchors.margins: border.width - 2
                // anchors {
                //     top: cells.itemAt(index*3).top
                //     left: cells.itemAt(index*3).left
                //     right: item.right
                //     bottom: item.bottom
                // }
                color: "transparent"
                // color: "blue"
                // width: parent.width
                // height: parent.height
                // property int index: index
                width: parent.cellSize
                height: parent.cellSize
                border.width: 3
                // border.color: "red"
                Grid {
                    id: cellGrid
                    anchors {
                    //     //right: parent.right
                        centerIn: parent
                        fill: parent
                        margins: parent.border.width-1
                    //     //bottom: parent.bottom
                    //     //left: parent.left
                    //     //top: parent.top
                    //     //left: downHints.right
                    //     //top: acrossHints.bottom
                    }

                    spacing: 0
                    // rows: Crossword.rows
                    // columns: Crossword.columns
                    rows: 3
                    columns: 3
                    // width: 80*3
                    // height: 80*3

                    // property int cellSize: Math.max(Math.floor(Math.min(width / Crossword.columns, height / Crossword.rows)), 80);
                    // property int cellSize: Math.max(Math.floor(Math.min(parent.width / columns, parent.height / rows)), 80);
                    property int cellSize: 80;
                    property int groupIndex: index

                    // Rectangle {
                    //     // anchors.fill: parent
                    //     // anchors.margins: -(border.width + 2)
                    //     //anchors.margins: -3
                    //     // anchors {
                    //     //     top: cells.itemAt(index*3).top
                    //     //     left: cells.itemAt(index*3).left
                    //     //     right: item.right
                    //     //     bottom: item.bottom
                    //     // }
                    //     color: "transparent"
                    //     // color: "blue"
                    //     // width: parent.width
                    //     // height: parent.height
                    //     width: parent.cellSize*3
                    //     height: parent.cellSize*3
                    //     border.width: 5
                    //     border.color: "black"
                    // }

                    Repeater {
                        id: cells
                        model: parent.rows * parent.columns
                        // delegate: puzzleCell { index }
                        delegate: DrawableCell {
                            width: parent.cellSize
                            height: parent.cellSize
                            //onWidthChanged: console.log("cell widtH:" + width)
                            // property int globalIndex: 3*parent.parent.groupIndex+index
                            property int globalIndex: Math.floor(index/3)*6 + parent.groupIndex*3 + index + Math.floor(parent.groupIndex/3)*18

                            enabled: !Crossword.isFixed(globalIndex) //&& !correctText.visible

                            Rectangle {
                                anchors.fill: parent
                                border.width: 1
                                color: "transparent"
                            }

                            Text {
                                id: numberCell
                                anchors.centerIn: parent
                                // anchors.fill: parent
                                font.bold: Crossword.isFixed(globalIndex)
                                visible: parent.recognized === text || Crossword.isFixed(globalIndex)
                                text: Crossword.numberAt(globalIndex)
                                // visible: true
                                // text: globalIndex
                                minimumPointSize: 10
                                font.pointSize: 32
                                fontSizeMode: Text.Fit
                            }
                        }
                    }
                }
            }
        }
    }

    // Rectangle {
    //     anchors.fill: downHints
    //     anchors.margins: -(border.width  + 2)
    //     color: "transparent"
    //     border.width: 5
    //     border.color: "black"
    // }

    // Column {
    //     id: downHints
    //     anchors {
    //         top: acrossHints.bottom
    //         topMargin: 20
    //         left: parent.left
    //         leftMargin: 10
    //     }

    //     Text {
    //         text: "Down"
    //         font.bold: true
    //     }

    //     Repeater {
    //         model: Crossword.hintsDown()
    //         delegate: Text {
    //             text: modelData
    //             font.pixelSize: 20
    //             width: 200
    //             wrapMode: Text.WordWrap
    //         }
    //     }
    // }

    // Rectangle {
    //     anchors.fill: acrossHints
    //     anchors.margins: -(border.width  + 2)
    //     color: "transparent"
    //     border.width: 5
    //     border.color: "black"
    // }

    // Grid {
    //     id: acrossHints
    //     anchors {
    //         right: parent.right
    //         left: parent.left
    //         top: parent.top
    //         margins: 10
    //     }

    //     Text {
    //         text: "Across"
    //         font.bold: true
    //     }

    //     Repeater {
    //         model: Crossword.hintsAcross()
    //         delegate: Text {
    //             text: modelData
    //             font.pixelSize: 20
    //         }
    //     }
    // }
}
