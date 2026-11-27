#pragma once

#include <string>
#include <optional>
#include <vector>
#include <functional>
#include <cstdint>

#include "src/app/repositories/FileStorage.hpp"
#include "src/data_structures/Dictionary.hpp"

using std::function;
using std::optional;
using std::string;
using std::vector;

struct NotificationEntity
{
    uint64_t userId;
    uint64_t id;
    string title;
    string content;
};

// schema: USER_ID - ID - TITLE - CONTENT
struct NotificationRepository
{
private:
    // Config
    static constexpr char DELIMITER = '|';
    static constexpr int FIELD_COUNT = 4;

    static constexpr int USER_ID_INDEX = 0;
    static constexpr int ID_INDEX = 1;
    static constexpr int TITLE_INDEX = 2;
    static constexpr int CONTENT_INDEX = 3;

    const string filePath;

    // DB principal indexado por id de la notificacion
    Dictionary<uint64_t, NotificationEntity> notificationsById;

    // Mapea: userId -> [notificationId]
    Dictionary<uint64_t, vector<uint64_t>> userToNotificationIds;

private:
    void indexUserNotification(uint64_t userId, uint64_t notificationId)
    {
        if (userToNotificationIds.contains(userId))
        {
            userToNotificationIds[userId].push_back(notificationId);
        }
        else
        {
            userToNotificationIds.add(userId, { notificationId });
        }
    }

    void load()
    {
        FileStorage::forEach(filePath, FIELD_COUNT, DELIMITER, [this](const auto &row)
        {
            NotificationEntity notification;

            notification.userId = static_cast<uint64_t>(std::stoull(row[USER_ID_INDEX]));
            notification.id = static_cast<uint64_t>(std::stoull(row[ID_INDEX]));
            notification.title = row[TITLE_INDEX];
            notification.content = row[CONTENT_INDEX];

            notificationsById.add(notification.id, notification);

            indexUserNotification(notification.userId, notification.id);
        });
    }

    void persist(const NotificationEntity &notification) const
    {
        const string id = std::to_string(notification.id);

        FileStorage::upsert(filePath, FIELD_COUNT, DELIMITER,
        {
            std::to_string(notification.userId),
            id,
            notification.title,
            notification.content
        },
        [&id](const auto &row) -> bool
        {
            return id == row[ID_INDEX];
        });
    }

public:
    NotificationRepository(const string &filePath) : filePath(filePath)
    {
        load();
    }

    bool create(const NotificationEntity &notification)
    {
        if (notificationsById.contains(notification.id))
        {
            return false;
        }

        persist(notification);
        notificationsById.add(notification.id, notification);
        indexUserNotification(notification.userId, notification.id);

        return true;
    }

    bool update(const NotificationEntity &notification)
    {
        if (!notificationsById.contains(notification.id))
        {
            return false;
        }

        persist(notification);
        notificationsById.add(notification.id, notification);

        return true;
    }

    optional<NotificationEntity> findById(uint64_t id) const
    {
        return notificationsById.get(id);
    }

    vector<NotificationEntity> findByUserId(uint64_t userId) const
    {
        vector<NotificationEntity> result;
        auto notificationIds = userToNotificationIds.get(userId);

        if (!notificationIds)
        {
            return result;
        }

        for (uint64_t notificationId : *notificationIds)
        {
            auto notification = notificationsById.get(notificationId);

            if (notification)
            {
                result.push_back(*notification);
            }
        }

        return result;
    }

    size_t count() const
    {
        return notificationsById.count();
    }

    void visitAll(function<void(const NotificationEntity &)> onEachVisit) const
    {
        for (const auto &entry : notificationsById)
        {
            onEachVisit(entry.value);
        }
    }
};
