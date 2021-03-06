#include <list>

#ifndef OBSERVER_PATTERN
#define OBSERVER_PATTERN 1

template<typename T, typename K>
class Subject;

template<typename T, typename K>
class Observer;

template<typename T, typename K>
class Subject {
private:
    T _subject;
    std::list<Observer<T, K>> _observers;
    bool _updated;

public:
    Subject(const T&);
    void SetUpdated(bool);
    void NotifyObservers();
    void InsertObserver(Observer<T, K>&&);
    void RemoveObserver(Observer<T, K>&);
};

template<typename T, typename K>
Subject<T, K>::Subject(const T& subject) {
    this->_subject = subject;
    this->_updated = false;
}

template<typename T, typename K>
void Subject<T, K>::SetUpdated(bool updated) {
    this->_updated = updated;
}

template<typename T, typename K>
void Subject<T, K>::NotifyObservers() {
    if (this->_updated) {
        for (auto itr = this->_observers.begin(); itr != this->_observers.end();) {
            if (itr->Update(this->_subject)) {
                this->_observers.erase(itr++);
            } else {
                itr++;
            }
        }
        this->_updated = false;
    }
}

template<typename T, typename K>
void Subject<T, K>::InsertObserver(Observer<T, K>&& observer) {
    for (auto itr = this->_observers.begin(); itr != this->_observers.end(); itr++) {
        if (observer == *itr) {
            return;
        }
    }
    this->_observers.push_back(std::move(observer));
}

template<typename T, typename K>
void Subject<T, K>::RemoveObserver(Observer<T, K>& observer) {
    for (auto itr = this->_observers.begin(); itr != this->_observers.end(); itr++) {
        if (observer == *itr) {
            this->_observers.erase(itr);
            return;
        }
    }
}

template<typename T, typename K>
class Observer {
private:
    K _observer;
    bool (*_update)(const T&, K&);

public:
    Observer(K& observer, bool(*update)(const T&, K&));
    Observer(K&& observer, bool(*update)(const T&, K&));
    // return value: wether to remove itsalf from the observer list
    bool Update(const T&);
    bool operator== (const Observer& observer) const;
};

template<typename T, typename K>
Observer<T, K>::Observer(K& observer, bool(*update)(const T&, K&)) {
    this->_observer = observer;
    this->_update = update;
}

template<typename T, typename K>
Observer<T, K>::Observer(K&& observer, bool(*update)(const T&, K&)) {
    this->_observer = std::move(observer);        
    this->_update = update;
}

template<typename T, typename K>
bool Observer<T, K>::Update(const T& subject) {
    return this->_update(subject, this->_observer);
}

template<typename T, typename K>
bool Observer<T, K>::operator== (const Observer& observer) const {
    return this->_observer == observer._observer;
}

#endif