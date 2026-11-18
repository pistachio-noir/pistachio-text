#include "Document.h"
#include "SharedTheme.h"
#include "FolderLister.h"
#include "Highlighter.h"
#include "WheelZoom.h"

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

static void registerTypes()
{
    qmlRegisterTypesAndRevisions<Document>("Pistachio.Text", 1);
    qmlRegisterTypesAndRevisions<FolderLister>("Pistachio.Text", 1);
    qmlRegisterTypesAndRevisions<Highlighter>("Pistachio.Text", 1);
    qmlRegisterTypesAndRevisions<WheelZoom>("Pistachio.Text", 1);
    qmlRegisterTypesAndRevisions<SharedTheme>("Pistachio.Text", 1);
    qmlRegisterModule("Pistachio.Text", 1, 0);
}

static const QQmlModuleRegistration registration("Pistachio.Text", registerTypes);
