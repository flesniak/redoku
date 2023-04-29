#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QDebug>
#include <QPainter>

#include "crossword.h"
#include "drawablecell.h"

#include <time.h>
#include "cwc/symbol.hh"

#ifdef REMARKABLE_DEVICE
#include <epframebuffer.h>

#include <QtPlugin>
Q_IMPORT_PLUGIN(QsgEpaperPlugin)
#endif

// hack around crappy stylus/tablet support in qt 5.6^W qt at least up to and including 5.11
class TabletWindow : public QQuickWindow
{
protected:

    // Ugly hack, don't do this
    void tabletEvent(QTabletEvent *event) override {
        QEvent::Type fakeType;
        switch(event->type()) {
        case QEvent::TabletMove:
            fakeType = QEvent::MouseMove;
            break;
        case QEvent::TabletPress:
            fakeType = QEvent::MouseButtonPress;
            break;
        case QEvent::TabletRelease:
            fakeType = QEvent::MouseButtonRelease;
            break;
        default:
            qDebug() << "Unhandled";
            return;
        }

        QPointF scenePos(event->globalPosF().y() / height() * width(), (1.0 - event->globalPosF().x() / width()) * height());
        QMouseEvent fakeEvent(fakeType,
                              scenePos,
                              scenePos,
                              fakeType == QEvent::MouseMove ? Qt::NoButton : Qt::LeftButton,
                              Qt::NoButton,
                              Qt::NoModifier
                              );
        switch(fakeType) {
        case QEvent::MouseMove:
            mouseMoveEvent(&fakeEvent);
            break;
        case QEvent::MouseButtonPress:
            mousePressEvent(&fakeEvent);
            break;
        case QEvent::MouseButtonRelease:
            mouseReleaseEvent(&fakeEvent);
            break;
        default:
            break;
        }
    }
};

int main(int argc, char *argv[])
{
    setlocale(LC_CTYPE, "");
    // Symbol::buildindex();
    qsrand(time(0));

#ifdef REMARKABLE_DEVICE
    qputenv("QMLSCENE_DEVICE", "epaper");
    qputenv("QT_QPA_PLATFORM", "epaper:enable_fonts");
    qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS", "rotate=180");
    qputenv("QT_QPA_GENERIC_PLUGINS", "evdevtablet");
#else
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);

#ifdef REMARKABLE_DEVICE
    {
        EPFrameBuffer::framebuffer()->fill(Qt::white);
        QPainter painter(EPFrameBuffer::framebuffer());
        painter.drawText(EPFrameBuffer::framebuffer()->rect(), Qt::AlignCenter, "Generating puzzle...");
        painter.end();
        EPFrameBuffer::sendUpdate(EPFrameBuffer::framebuffer()->rect(), EPFrameBuffer::Grayscale, EPFrameBuffer::FullUpdate, true);
    }
#endif

// hack around crappy stylus/tablet support in qt 5.6
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTabletEvents);
#endif

    QQmlApplicationEngine engine;
    qmlRegisterType<DrawableCell>("com.iskrembilen", 1, 0, "DrawableCell");
    qmlRegisterType<TabletWindow>("com.iskrembilen", 1, 0, "TabletWindow");
    qmlRegisterSingletonType<Crossword>("com.iskrembilen", 1, 0, "Crossword", [](QQmlEngine *engine, QJSEngine*) -> QObject* {
        Crossword *crossword = new Crossword();
        engine->setObjectOwnership(crossword, QQmlEngine::JavaScriptOwnership);
        return crossword;
    });
    // engine.rootContext()->setProperty("Crossword", &crossword);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
