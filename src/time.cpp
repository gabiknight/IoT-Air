#include <header.h>

String get_date_ntp()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return "";
    }

    int reintentos = 3;
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", -10800, 60000); // el -10800 es para que quede en GMT -3

    const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t yOff, m, d, hh, mm, ss;
    unsigned long t;

    while (reintentos > 0)
    {
        timeClient.begin();
        timeClient.update();
        t = timeClient.getEpochTime();
        if (t >= 1514764800 and t <= 4102444800)
        {
            break;
        }
        t = 1514764800;
        reintentos--;
        timeClient.end();
        delay(200);
    }

    timeClient.end();

    if (reintentos == 0)
    {
        return "";
    }

    t -= 946684800; // bring to 2000 timestamp from 1970
    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0;; ++yOff)
    {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1;; ++m)
    {
        uint8_t daysPerMonth = daysInMonth[m - 1];
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;

    String yearStr = yOff < 10 ? "0" + String(yOff) : String(yOff);
    String monthStr = m < 10 ? "0" + String(m) : String(m);
    String dayStr = d < 10 ? "0" + String(d) : String(d);
    String hoursStr = hh < 10 ? "0" + String(hh) : String(hh);
    String minuteStr = mm < 10 ? "0" + String(mm) : String(mm);
    String secondStr = ss < 10 ? "0" + String(ss) : String(ss);

    return "20" + yearStr + monthStr + dayStr + hoursStr + minuteStr + secondStr;
}