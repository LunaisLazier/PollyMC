/* Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "net/NetJob.h"
#include "GoUpdate.h"
#include "ExternalUpdater.h"

#ifdef Q_OS_MAC
#include "MacSparkleUpdater.h"
#endif

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    UpdateChecker(shared_qobject_ptr<QNetworkAccessManager> nam, QString channelUrl, QString currentChannel, int currentBuild);
    void checkForUpdate(const QString& updateChannel, bool notifyNoUpdate);

    /*!
     * Causes the update checker to download the channel list from the URL specified in config.h (generated by CMake).
     * If this isn't called before checkForUpdate(), it will automatically be called.
     */
    void updateChanList(bool notifyNoUpdate);

    /*!
     * An entry in the channel list.
     */
    struct ChannelListEntry
    {
        QString id;
        QString name;
        QString description;
        QString url;
    };

    /*!
     * Returns a the current channel list.
     * If the channel list hasn't been loaded, this list will be empty.
     */
    QList<ChannelListEntry> getChannelList() const;

    /*!
     * Returns false if the channel list is empty.
     */
    bool hasChannels() const;

    /*!
     * Returns a pointer to an object that controls the external updater, or nullptr if an external updater is not used.
     */
    ExternalUpdater *getExternalUpdater();

signals:
    //! Signal emitted when an update is available. Passes the URL for the repo and the ID and name for the version.
    void updateAvailable(GoUpdate::Status status);

    //! Signal emitted when the channel list finishes loading or fails to load.
    void channelListLoaded();

    void noUpdateFound();

private slots:
    void updateCheckFinished(bool notifyNoUpdate);
    void updateCheckFailed();

    void chanListDownloadFinished(bool notifyNoUpdate);
    void chanListDownloadFailed(QString reason);

private:
    friend class UpdateCheckerTest;

    shared_qobject_ptr<QNetworkAccessManager> m_network;

    NetJob::Ptr indexJob;
    QByteArray indexData;
    NetJob::Ptr chanListJob;
    QByteArray chanlistData;

    QString m_channelUrl;

    QList<ChannelListEntry> m_channels;

    /*!
     * True while the system is checking for updates.
     * If checkForUpdate is called while this is true, it will be ignored.
     */
    bool m_updateChecking = false;

    /*!
     * True if the channel list has loaded.
     * If this is false, trying to check for updates will call updateChanList first.
     */
    bool m_chanListLoaded = false;

    /*!
     * Set to true while the channel list is currently loading.
     */
    bool m_chanListLoading = false;

    /*!
     * Set to true when checkForUpdate is called while the channel list isn't loaded.
     * When the channel list finishes loading, if this is true, the update checker will check for updates.
     */
    bool m_checkUpdateWaiting = false;

    /*!
     * if m_checkUpdateWaiting, this is the last used update channel
     */
    QString m_deferredUpdateChannel;

    int m_currentBuild = -1;
    QString m_currentChannel;
    QString m_currentRepoUrl;

    QString m_newRepoUrl;

    /*!
     * If not a nullptr, then the updater here will be used instead of the old updater that uses GoUpdate when
     * checking for updates.
     *
     * As a result, signals from this class won't be emitted, and most of the functions in this class other
     * than checkForUpdate are not useful. Call functions from this external updater object instead.
     */
    ExternalUpdater *m_externalUpdater = nullptr;
};

