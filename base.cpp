#include <iostream>

using namespace std;

enum EState {Ework, Egraffity, Erotate, Emove, Esleep, Eclean};

class sender {
public:
    void send_rotate () {
        cout << "Rotating" << endl;
    }
    void send_moving () {
        cout << "Moving" << endl;
    }
    void send_cleaning () {
        cout << "Cleaning" << endl;
    }
};

class detector {
public:
    bool graffity_exist () {
        cout << "Graffity?: ";
        bool a;
        cin >> a;
        cout << endl;
        if (a == true) return true;
        else return false;
    };
    bool angle (float alpha){
        cout << "Angle?: ";
        float degree;
        cin >> degree;
        cout << endl;
        if (degree > alpha) return true;
        else return false;
    };
    bool distance (float dist){
        cout << "Distance?: ";
        float distance;
        cin >> distance;
        cout << endl;
        if (distance > dist) return true;
        else return false;

    };
};

class robot {
    EState state;
    detector det;
    sender snd;
    const float a = 5, b = 10;
    bool exit = false;
public:
    robot() {
        state = Ework;
    }
    void Events() {
        switch(state) {
            case Ework:
                if (det.graffity_exist()) state = Egraffity;
                else state = Esleep;
                break;
            case Egraffity:
                if(det.angle(a)) state = Erotate;
                else if(det.distance(b)) state = Emove;
                else state = Eclean;
                break;
            case Erotate:
                snd.send_rotate();
                state = Ework;
                break;
            case Emove:
                snd.send_moving();
                state = Ework;
                break;
            case Eclean:
                snd.send_cleaning();
                state = Esleep;
                break;
            case Esleep:
                exit = true;
                break;
        }
    }
    void run() {
        while(!exit) Events();
        cout << "End" << endl;
        exit = false;
    }
};

int main() {
    bool a = true;
    robot bot;
    while (a) {
        bot.run();
        cout << "Continue?: ";
        cin >> a;
        cout << endl;
    }
};