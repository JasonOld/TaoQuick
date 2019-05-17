﻿#include "TaoView.h"
#include "Logger/Logger.h"
#include <QTranslator>
#include <QQmlEngine>
TaoView::TaoView(QWindow *parent) : QQuickView(parent)
{
    setFlag(Qt::FramelessWindowHint);
    setResizeMode(SizeRootObjectToView);
    setColor(QColor(Qt::transparent));

    m_languageList << u8"中文简体"
                   << u8"English"
                   << u8"日本語"
                   << u8"한국어"
                   << u8"Français"
                   << u8"Español"
                   << u8"Portugués"
                   << u8"In Italiano"
                   << u8"русский язык"
                   << u8"Tiếng Việt"
                   << u8"Deutsch"
                   << u8" عربي ، ";
    QStringList fileList;
    fileList << "trans_zh.qm"
            << "trans_en.qm"
            << "trans_ja.qm"
            << "trans_ko.qm"
            << "trans_fr.qm"
            << "trans_es.qm"
            << "trans_pt.qm"
            << "trans_it.qm"
            << "trans_ru.qm"
            << "trans_vi.qm"
            << "trans_de.qm"
            << "trans_ar.qm";

    for (auto i = 0; i < m_languageList.length(); ++i)
    {
        auto trans = std::make_shared<QTranslator>();
        bool ok = trans->load(fileList.at(i));
        LOG_INFO << m_languageList.at(i) << fileList.at(i) << ok;
        m_transMap[m_languageList.at(i)] = trans;
    }
    m_pLastLang = m_transMap[m_languageList.at(0)].get();
    QCoreApplication::installTranslator(m_pLastLang);
    m_lang = m_languageList.at(0);
    emit languageListChanged();
}

TaoView::~TaoView()
{
    qDeleteAll(m_pluginList);
}

void TaoView::reTrans(const QString &lang)
{
    if (m_lang == lang)
    {
        return;
    }

    QCoreApplication::removeTranslator(m_pLastLang);
    m_pLastLang = m_transMap[lang].get();
    QCoreApplication::installTranslator(m_pLastLang);
    for (auto pPlugin : m_pluginList)
    {
        pPlugin->replaceTranslater(m_lang, lang);
    }
    m_lang = lang;
    engine()->retranslate();
    emit reTransed();
}

void TaoView::loadPlugin(const QString &pluginPath)
{
    QDir dir(pluginPath);

    auto list = dir.entryInfoList({"*"}, QDir::Files);

    for (auto info : list) {
        if (QLibrary::isLibrary(info.absoluteFilePath()))
        {
            m_loader = std::make_unique<QPluginLoader>();
            m_loader->setFileName(info.absoluteFilePath());
            if (!m_loader->load())
            {
                LOG_WARN << m_loader->fileName() << m_loader->errorString();
                continue;
            }
            QObject *pObj = m_loader->instance();
            auto pPlugin = qobject_cast<ITaoQuickPlugin *>(pObj);
            if (!pPlugin)
            {
                continue;
            }
            pPlugin->init();
            emit pluginReady(QString(QJsonDocument(pPlugin->infos()).toJson()));
            m_pluginList.append(pPlugin);
            LOG_INFO << "loaded plugin " << info.absoluteFilePath();
        }
    }
}
