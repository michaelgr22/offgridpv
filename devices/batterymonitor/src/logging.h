#ifndef LOGGING_H
#define LOGGING_H

#include <HTTPClient.h>
#include <ezTime.h>
#include <string>

using std::string;

class Logging
{
public:
    string server;
    Timezone tz;

    Logging(string server)
        : server(server)
    {
    }

    int sendLog(string device, string message)
    {
        if (WiFi.isConnected())
        {

            const char *servername = server.c_str();
            HTTPClient http;

            http.begin(servername);
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");

            string time = createTimeString();
            string httpMessage = convertToHttpString(message.c_str());
            string body = "device=" + device + "&time=" + time + "&message=" + httpMessage;

            return http.POST(body.c_str());
        }

        return -1;
    }

    void syncTime()
    {
        if (WiFi.isConnected())
            waitForSync();
    }

private:
    string createTimeString()
    {
        String time = tz.dateTime("Y-m-d H:i:s.v");
        return convertToHttpString(time);
    }

    string convertToHttpString(String str)
    {
        str.replace(" ", "%20");
        str.replace(":", "%3A");
        return str.c_str();
    }
};

#endif