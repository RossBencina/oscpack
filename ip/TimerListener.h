#ifndef INCLUDED_TIMERLISTENER_H
#define INCLUDED_TIMERLISTENER_H


class TimerListener{
public:
    virtual ~TimerListener() {}
    virtual void TimerExpired() = 0;
};

#endif /* INCLUDED_TIMERLISTENER_H */
