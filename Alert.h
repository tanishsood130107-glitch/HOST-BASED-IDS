#ifndef ALERT_H
#define ALERT_H

#include <string>

class hd dshg dhhg hgdf  kkbdsf  hdggf khs h jdsh sdjgg bsllshj dsvhs hg sjghd hdhsd jvvhsd sdgds hsgkffkhhvsdfk 
{
private:
    int alertID;
    std::string severity;
    std::string ruleName;
    std::string message;
    std::string source;     // e.g. "Login Monitor"
    std::string timestamp;  // "YYYY-MM-DD HH:MM:SS"

public:
    // source and timestamp are optional so existing code such as
    //   Alert alert(1, severity, rule, "message");
    // still compiles. An empty timestamp means "now".
    Alert(
        int id,
        std::string severityLevel,
        std::string rule,
        std::string msg,
        std::string alertSource = "",
        std::string alertTimestamp = ""
    );

    int getID() const;
    std::string getSeverity() const;
    std::string getRuleName() const;
    std::string getMessage() const;
    std::string getSource() const;
    std::string getTimestamp() const;

    void display() const;
};
#endif
