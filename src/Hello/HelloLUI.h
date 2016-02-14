#ifndef HelloLUI_h
#define HelloLUI_h

class HelloLUI: public LogicalUnitInfo {

public:

    HelloLUI() : LogicalUnitInfo() {
        what = strdup("Hello");
        who = strdup("Earthlings");
    }

    HelloLUI(const char *to) : LogicalUnitInfo() {
        what = strdup("Hello");
        who = strdup(to);
    }

    ~HelloLUI() {
        free(what);
        free(who);
    }

    void setWhat(const char *newWhat) {
        free(what);
        what = strdup(newWhat);
    }

    const char *getWhat() {
        return what;
    }

    void setWho(const char *newWho) {
        free(who);
        who = strdup(newWho);
    }

    const char *getWho() {
        return who;
    }

private:
    char *what;
    char *who;
};

#endif
